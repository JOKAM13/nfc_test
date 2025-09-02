#include <string.h>
#include "st25_discovery.h"


#define BOARDCHECK_MAGIC_INFO_ADDR		0x080FFFE0
const char* BrdMagicInfo = ".EnterBoardcheck";

static void runBoardCheck(void)
{
  uint32_t boardcheck_addr = *(uint32_t*)(BOARDCHECK_MAGIC_INFO_ADDR + strlen(BrdMagicInfo));
  void (*boardCheck)(void);

  /* Jump to user application */
  boardCheck = (void (*)(void))*(uint32_t*)(boardcheck_addr + 4);
  /* Initialize user application's Stack Pointer */
  __set_MSP( *(__IO uint32_t *)boardcheck_addr );
  boardCheck();

  while(1);
}

/** @brief This function test if a magic string is present in Flash.
  * If the magic string is found, and specific joystick position is detected
  * the Board Check program is executed.
  */
void BoardCheck( void )
{
  if(0 == memcmp((const void*)BOARDCHECK_MAGIC_INFO_ADDR, BrdMagicInfo, strlen(BrdMagicInfo)))
  {
    BSP_PB_Init( (Button_TypeDef)JOYSTICK_DOWN, BUTTON_MODE_GPIO );
    int timeout = 100;
    while((BSP_PB_GetState( JOYSTICK_DOWN ) == GPIO_PIN_SET) && (timeout-- > 0));
    
    if(timeout > 0)
      return;
    else
      runBoardCheck();
  }
}
