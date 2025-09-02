/**
  ******************************************************************************
  * @file    st25_discovery.c 
  * @author  MMY Application Team
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on ST25-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2021 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "st25_discovery.h"
#if defined(USE_NFCTAG)
#include "st25_discovery_nfctag.h"
#endif

/** @defgroup BSP BSP
  * @{
  */ 

/** @addtogroup ST25_DISCOVERY_BSP ST25 DISCOVERY
  * @brief    The Board support package for the ST25 Discovery.
  * @details  Provides all the requires functions to abstract from the board HW.
  * @{
  */   
    
/** @defgroup ST25_DISCOVERY_LOW_LEVEL ST25 DISCOVERY LOW LEVEL
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on ST25 Discovery Kit from STMicroelectronics.
  * @{
  */ 


/**
  * @brief ST25 DISCOVERY BSP Driver version number V2.0.0
  * @{
  */
#define __ST25_DISCOVERY_BSP_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __ST25_DISCOVERY_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __ST25_DISCOVERY_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __ST25_DISCOVERY_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __ST25_DISCOVERY_BSP_VERSION        ((__ST25_DISCOVERY_BSP_VERSION_MAIN << 24)\
                                             |(__ST25_DISCOVERY_BSP_VERSION_SUB1 << 16)\
                                             |(__ST25_DISCOVERY_BSP_VERSION_SUB2 << 8 )\
                                             |(__ST25_DISCOVERY_BSP_VERSION_RC))   
/**
  * @}
  */
  

/**
  * @brief List of GPIO ports used for the LEDs.
  */
GPIO_TypeDef* GPIO_PORT[ST25_DISCOVERY_LEDn] = {ST25_DISCOVERY_LED2_GPIO_PORT, ST25_DISCOVERY_LED3_GPIO_PORT};

/**
  * @brief List of GPIO pins used for the LEDs.
  */
const uint16_t GPIO_PIN[ST25_DISCOVERY_LEDn] = {ST25_DISCOVERY_LED2_PIN,ST25_DISCOVERY_LED3_PIN};

/**
  * @brief List of GPIO ports used for the Joystick & Button.
  */
GPIO_TypeDef* BUTTON_PORT[ST25_DISCOVERY_BUTTONn] = {ST25_DISCOVERY_KEY_BUTTON_GPIO_PORT,
                                                     ST25_DISCOVERY_JOYSTICK_SEL_GPIO_PORT,
                                                     ST25_DISCOVERY_JOYSTICK_LEFT_GPIO_PORT,
                                                     ST25_DISCOVERY_JOYSTICK_RIGHT_GPIO_PORT,
                                                     ST25_DISCOVERY_JOYSTICK_UP_GPIO_PORT,
                                                     ST25_DISCOVERY_JOYSTICK_DOWN_GPIO_PORT}; 

/**
  * @brief List of GPIO pins used for the Joystick & Button.
  */
const uint16_t BUTTON_PIN[ST25_DISCOVERY_BUTTONn] = {ST25_DISCOVERY_KEY_BUTTON_PIN,
                                                     ST25_DISCOVERY_JOYSTICK_SEL_GPIO_PIN,
                                                     ST25_DISCOVERY_JOYSTICK_LEFT_GPIO_PIN,
                                                     ST25_DISCOVERY_JOYSTICK_RIGHT_GPIO_PIN,
                                                     ST25_DISCOVERY_JOYSTICK_UP_GPIO_PIN,
                                                     ST25_DISCOVERY_JOYSTICK_DOWN_GPIO_PIN}; 

/**
  * @brief List of interrupt lines used for the Joystick & Button.
  */
const uint8_t BUTTON_IRQn[ST25_DISCOVERY_BUTTONn] = {ST25_DISCOVERY_KEY_BUTTON_EXTI_IRQn,
                                                     EXTI0_IRQn,
                                                     EXTI0_IRQn,
                                                     EXTI0_IRQn,
                                                     EXTI0_IRQn,
                                                     EXTI0_IRQn};


#ifdef HAL_SPI_MODULE_ENABLED
uint32_t SpixTimeout = ST25_DISCOVERY_SPIx_TIMEOUT_MAX; /*<! Value of Timeout when SPI communication fails */
#endif /* HAL_SPI_MODULE_ENABLED */
                                                     
#ifdef HAL_I2C_MODULE_ENABLED
uint32_t I2cxTimeout = ST25_DISCOVERY_I2Cx_TIMEOUT; /*<! Value of Timeout when I2C communication fails */
#endif /* HAL_I2C_MODULE_ENABLED */
                                                     
#if defined(HAL_SPI_MODULE_ENABLED) && defined(USE_LCD)
static uint8_t Is_LCD_IO_Initialized = 0;
#endif /* HAL_SPI_MODULE_ENABLED && USE_LCD */
                                                     
#if defined(HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV)
/**
* @brief List of Analog to Digital converter IOs.
*/
const uint32_t ADC1_Channel_list[ST25_DISCOVERY_EH_CHANNELn] = ST25_DISCOVERY_EH_CHANNEL_LIST;
static ADC_ChannelConfTypeDef sConfig;
#endif /* HAL_ADC_MODULE_ENABLED && USE_ST25DV */

/**
 * @brief BUS variables
 */
#ifdef HAL_SPI_MODULE_ENABLED
static SPI_HandleTypeDef hspi2;
#endif /* HAL_SPI_MODULE_ENABLED */

#if defined(HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV)
static ADC_HandleTypeDef hadc1;
#endif /* HAL_ADC_MODULE_ENABLED */

#if defined(HAL_OPAMP_MODULE_ENABLED) && defined(USE_ST25DV)
static OPAMP_HandleTypeDef hOpamp1;
#endif /* HAL_OPAMP_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
static I2C_HandleTypeDef hi2c1;
#if defined(USE_NFCTAG)
static I2C_HandleTypeDef hi2c2;
#endif
static I2C_HandleTypeDef hi2c3;

/* I2Cx bus function */
static void                 STM32_I2C1_Init( void );
static void                 STM32_I2C1_ITConfig( void );
static void                 STM32_I2C1_WriteData( uint8_t Addr, uint8_t Reg, uint8_t Value );
static void                 STM32_I2C1_WriteBuffer( uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length );
static uint8_t              STM32_I2C1_ReadData( uint8_t Addr, uint8_t Reg );
static uint8_t              STM32_I2C1_ReadBuffer( uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length );
static void                 STM32_I2C1_Error( void );
static void                 STM32_I2C1_MspInit( I2C_HandleTypeDef *hi2c );  

#if defined(USE_NFCTAG)
static HAL_StatusTypeDef    STM32_I2C2_Init( void );
static HAL_StatusTypeDef    STM32_I2C2_DeInit( void );
static HAL_StatusTypeDef    STM32_I2C2_MemWrite( const uint8_t * const pData, const uint8_t DevAddr, 
                                                 const uint16_t TarAddr, const uint16_t Size );
static HAL_StatusTypeDef    STM32_I2C2_MemRead( uint8_t * const pData, const uint8_t DevAddr, const uint16_t TarAddr, 
                                                const uint16_t Size );
static HAL_StatusTypeDef    STM32_I2C2_Read( uint8_t * const pData, const uint8_t DevAddr, const uint16_t Size );
static uint8_t              STM32_I2C2_IsNacked( void );
static HAL_StatusTypeDef    STM32_I2C2_IsDeviceReady( const uint8_t DevAddr, const uint32_t Trials );
static void                 STM32_I2C2_MspInit( void );
static void                 STM32_I2C2_MspDeInit( void );
#endif /* USE_NFCTAG */

static void                 STM32_I2C3_Init( void );
static void                 STM32_I2C3_WriteBuffer( uint8_t Addr, uint8_t *pBuffer, uint16_t Length );
static uint8_t              STM32_I2C3_ReadData( uint8_t Addr, uint16_t Reg );
static void                 STM32_I2C3_Error( void );
static void                 STM32_I2C3_MspInit( I2C_HandleTypeDef *hi2c );
#endif /* HAL_I2C_MODULE_ENABLED */

#if defined(HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV)
static void                 STM32_ADC1_Init( void );
static void                 STM32_ADC1_DeInit( void );
static void                 STM32_ADC1_MspInit( ADC_HandleTypeDef *hadc );
static void                 STM32_ADC1_MspDeInit( ADC_HandleTypeDef *hadc );
#endif /* HAL_ADC_MODULE_ENABLED && USE_ST25DV */

#if defined(HAL_OPAMP_MODULE_ENABLED) && defined(USE_ST25DV)
static void                 STM32_OPAMP1_Init( void );
static void                 STM32_OPAMP1_DeInit( void );
static void                 STM32_OPAMP1_MspInit( OPAMP_HandleTypeDef *hOpamp );
static void                 STM32_OPAMP1_MspDeInit( OPAMP_HandleTypeDef *hOpamp );
#endif /* HAL_OPAMP_MODULE_ENABLED && USE_ST25DV && HAL_OPAMP_MODULE_ENABLED */

#if defined(HAL_SPI_MODULE_ENABLED) && defined(USE_LCD)
static void                 STM32_SPI2_Init( void );
static uint32_t             STM32_SPI2_Read( uint8_t ReadSize, uint8_t *buffer );
static void                 STM32_SPI2_Write( uint16_t Value );
static void                 STM32_SPI2_Error( void );
static void                 STM32_SPI2_MspInit( SPI_HandleTypeDef *hspi );
#endif /* HAL_SPI_MODULE_ENABLED && USE_LCD */

#if defined(HAL_SPI_MODULE_ENABLED) && defined(USE_LCD)
/* LCD IO functions */
void                        LCD_IO_Init( void );
void                        LCD_IO_WriteData( uint8_t Data );
void                        LCD_IO_WriteData16( uint16_t RegValue );
void                        LCD_IO_WriteMultipleData( uint8_t *pData, uint32_t Size );
uint32_t                    LCD_IO_ReadData( uint16_t RegValue, uint8_t ReadSize );
void                        LCD_IO_WriteReg( uint8_t LCDReg );
void                        LCD_Delay( uint32_t delay );
#endif /* HAL_SPI_MODULE_ENABLED && USE_LCD */

#ifdef HAL_I2C_MODULE_ENABLED
/* IOExpander IO functions */
void                        IOE_Init( void );
void                        IOE_ITConfig( void );
void                        IOE_Delay( uint32_t Delay );
void                        IOE_Write( uint8_t Addr, uint8_t Reg, uint8_t Value );
uint8_t                     IOE_Read( uint8_t Addr, uint8_t Reg );
uint16_t                    IOE_ReadMultiple( uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length );
void                        IOE_WriteMultiple( uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length );

/* DPOT IO functions */
void                        DPOT_IO_Init( void );
uint8_t                     DPOT_IO_Read( uint8_t Addr, uint16_t Reg );
void                        DPOT_IO_WriteBuffer( uint8_t Addr, uint8_t *pBuf, uint16_t Len );
#endif /* HAL_I2C_MODULE_ENABLED */

#if defined(USE_NFCTAG)
/* NFCTAG IO functions */
static int32_t NFCTAG_IO_Init( void );
static int32_t NFCTAG_IO_DeInit( void );
int32_t        NFCTAG_IO_MemWrite( const uint8_t * const pData, const uint8_t DevAddr, 
                                                const uint16_t TarAddr, const uint16_t Size );
int32_t        NFCTAG_IO_MemRead( uint8_t * const pData, const uint8_t DevAddr, const uint16_t TarAddr, 
                                               const uint16_t Size );
int32_t        NFCTAG_IO_Read( uint8_t * const pData, const uint8_t DevAddr, const uint16_t Size );
uint8_t                     NFCTAG_IO_IsNacked( void );
int32_t        NFCTAG_IO_IsDeviceReady( const uint8_t DevAddr, const uint32_t Trials );

#endif /* USE_NFCTAG */

/**
  * @brief  This method returns the ST25 DISCOVERY BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion( void )
{
  return __ST25_DISCOVERY_BSP_VERSION;
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_Init( Led_TypeDef Led )
{
  GPIO_InitTypeDef gpio_initstruct;
  
  /* Enable the GPIO_LED Clock */
  ST25_DISCOVERY_LEDx_GPIO_CLK_ENABLE( Led );
  
  /* Configure the GPIO_LED pin */
  gpio_initstruct.Pin = GPIO_PIN[Led];
  gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_initstruct.Pull = GPIO_NOPULL;
  gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init( GPIO_PORT[Led], &gpio_initstruct );
  
  HAL_GPIO_WritePin( GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET ); 
}

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be de-init. 
  *   This parameter can be one of the following values:
  *     @arg  LED2
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  */
void BSP_LED_DeInit( Led_TypeDef Led )
{
  GPIO_InitTypeDef gpio_initstruct;

  /* Turn off LED */
  HAL_GPIO_WritePin( GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET );
  /* DeInit the GPIO_LED pin */
  gpio_initstruct.Pin = GPIO_PIN[Led];
  HAL_GPIO_DeInit( GPIO_PORT[Led], gpio_initstruct.Pin );
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_On( Led_TypeDef Led )
{
  HAL_GPIO_WritePin( GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET );
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_Off( Led_TypeDef Led )
{
  HAL_GPIO_WritePin( GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET );
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_Toggle( Led_TypeDef Led )
{
  HAL_GPIO_TogglePin( GPIO_PORT[Led], GPIO_PIN[Led] );
}

/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter should be: BUTTON_KEY
  * @param  ButtonMode: Specifies Button mode.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability  
  */
void BSP_PB_Init( Button_TypeDef Button, ButtonMode_TypeDef ButtonMode )
{
  GPIO_InitTypeDef gpio_initstruct;
  
  /* Enable the BUTTON Clock */
  ST25_DISCOVERY_BUTTONx_GPIO_CLK_ENABLE( Button );
  
  if( ButtonMode == BUTTON_MODE_GPIO )
  {
    /* Configure Button pin as input */
    gpio_initstruct.Pin = BUTTON_PIN[Button];
    gpio_initstruct.Mode = GPIO_MODE_INPUT;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( BUTTON_PORT[Button], &gpio_initstruct );
  }
  
  if( ButtonMode == BUTTON_MODE_EXTI )
  {
    /* Configure Button pin as input with External interrupt */
    gpio_initstruct.Pin = BUTTON_PIN[Button];
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Mode = GPIO_MODE_IT_FALLING; 
    HAL_GPIO_Init( BUTTON_PORT[Button], &gpio_initstruct );
    
    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority( (IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00 );
    HAL_NVIC_EnableIRQ( (IRQn_Type)(BUTTON_IRQn[Button]) );
  }
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *   This parameter should be: BUTTON_KEY
  * @note PB DeInit does not disable the GPIO clock
  */
void BSP_PB_DeInit( Button_TypeDef Button )
{
    GPIO_InitTypeDef gpio_initstruct;

    gpio_initstruct.Pin = BUTTON_PIN[Button];
    HAL_NVIC_DisableIRQ( (IRQn_Type)(BUTTON_IRQn[Button]) );
    HAL_GPIO_DeInit( BUTTON_PORT[Button], gpio_initstruct.Pin );
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter should be: BUTTON_KEY  
  * @retval The Button GPIO pin value.
  */
uint32_t BSP_PB_GetState( Button_TypeDef Button )
{
  return !HAL_GPIO_ReadPin( BUTTON_PORT[Button], BUTTON_PIN[Button] );
}

#if defined(USE_NFCTAG) && defined(USE_ST25DV)
/**
  * @brief  This function initialize the GPIO to manage the NFCTAG GPO pin
  * @param  None
  * @retval None
  */
void BSP_GPO_Init( uint32_t it_mode )
{
  GPIO_InitTypeDef gpio_initstruct;
  NFCMEM_GPO_CLK_ENABLE( );

  gpio_initstruct.Pin   = NFCMEM_GPO_PIN;
  gpio_initstruct.Mode  = it_mode;
  gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_initstruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init( NFCMEM_GPO_PIN_PORT, &gpio_initstruct );
}

/**
  * @brief  This function initialize the GPIO to manage the NFCTAG LPD pin
  * @param  None
  * @retval None
  */
void BSP_LPD_Init( void )
{
  GPIO_InitTypeDef gpio_initstruct;
  NFCMEM_LPD_CLK_ENABLE( );

  gpio_initstruct.Pin   = NFCMEM_LPD_PIN;
  gpio_initstruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_initstruct.Speed = GPIO_SPEED_FREQ_LOW;
  gpio_initstruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init( NFCMEM_LPD_PIN_PORT, &gpio_initstruct );
  
  HAL_GPIO_WritePin( NFCMEM_LPD_PIN_PORT, NFCMEM_LPD_PIN, GPIO_PIN_RESET );
}

/**
  * @brief  This function get the GPIO value through GPIO
  * @param  None
  * @retval HAL GPIO pin status
  */
GPIO_PinState BSP_LPD_ReadPin( void )
{
  return HAL_GPIO_ReadPin( NFCMEM_LPD_PIN_PORT, NFCMEM_LPD_PIN );
}

/**
  * @brief  This function get the GPIO value through GPIO
  * @param  None
  * @retval HAL GPIO pin status
  */
void BSP_LPD_WritePin( GPIO_PinState LpdPinState )
{
  HAL_GPIO_WritePin( NFCMEM_LPD_PIN_PORT, NFCMEM_LPD_PIN, LpdPinState );
}
#endif /* USE_NFCTAG && USE_ST25DV */

/******************************** LINK EEPROM COMPONENT *****************************/
#if defined(USE_NFCTAG)
/**
  * @brief  Initializes peripherals used by the I2C NFCTAG driver
  * @param  None
  * @retval NFCTAG enum status
  */
int32_t NFCMEM_IO_Init( void )
{
  return NFCTAG_IO_Init( );
}

/**
  * @brief  DeInitializes peripherals used by the I2C NFCTAG driver
  * @param  None
  * @retval NFCTAG enum status
  */
int32_t NFCMEM_IO_DeInit( void )
{
  return NFCTAG_IO_DeInit( );
}

__weak void NFCMEM_IO_MemWriteCompleted_Callback( uint32_t size )
{
  /* This callback can be implemented elsewhere */ 
}

/**
  * @brief  Write data, at specific address, through i2c to the ST25DV
  * @param  DevAddr : Target device address
  * @param  TarAddr : I2C data memory address to write
  * @param  pData: pointer to the data to write
  * @param  Size : Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t NFCMEM_IO_MemWrite( uint16_t DevAddr, uint16_t TarAddr, const uint8_t * const pData, uint16_t Size )
{
  int32_t pollstatus;
  int32_t ret;
  uint32_t tickstart;
  
  ret = NFCTAG_IO_MemWrite( pData, DevAddr, TarAddr, Size );
  if( ret == NFCTAG_OK )
  {
    /* Poll until EEPROM is available */
    tickstart = HAL_GetTick( );
    /* Wait until ST25DV is ready or timeout occurs */
    do
    {
      pollstatus = NFCMEM_IO_IsDeviceReady( DevAddr, 1 );
    } while( ( (HAL_GetTick() - tickstart) < ST25DV_WRITE_TIMEOUT) && (pollstatus != NFCTAG_OK) );
    
    if( pollstatus != NFCTAG_OK )
    {
      ret = NFCTAG_TIMEOUT;
    }
  }
  else
  {
    /* Check if Write was NACK */
    if( NFCMEM_IO_IsNacked() == I2CANSW_NACK )
    {
      ret = NFCTAG_NACK;
    }
  }
  
  NFCMEM_IO_MemWriteCompleted_Callback( Size );
  
  return ret;
}

/**
  * @brief  Reads data at a specific address from the NFCTAG.
  * @param  DevAddr : Target device address
  * @param  TarAddr : I2C data memory address to read
  * @param  pData: pointer to store read data
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t NFCMEM_IO_MemRead( uint16_t DevAddr, uint16_t TarAddr, uint8_t * const pData,  uint16_t Size)
{
  return NFCTAG_IO_MemRead( pData, DevAddr, TarAddr, Size );
}

/**
  * @brief  Reads data at current address from the NFCTAG.
  * @param  pData: pointer to store read data
  * @param  DevAddr : Target device address
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t NFCMEM_IO_Read( uint8_t * const pData, const uint8_t DevAddr, const uint16_t Size )
{
  return NFCTAG_IO_Read( pData, DevAddr, Size );
}

/**
  * @brief  Checks if NACK was received from I2C Slave
  * @param  None
  * @retval 0 ACK, 1 NACK
  */
uint8_t NFCMEM_IO_IsNacked( void )
{
  return NFCTAG_IO_IsNacked( );
}

/**
* @brief  Checks if target device is ready for communication
* @note   This function is used with Memory devices
* @param  DevAddr : Target device address
* @param  Trials : Number of trials
* @retval NFCTAG enum status
*/
int32_t NFCMEM_IO_IsDeviceReady( uint16_t DevAddr,  uint32_t Trials )
{
  return NFCTAG_IO_IsDeviceReady( DevAddr, Trials );
}

/**
  * @brief  Tick function used in NFC device low level driver.
  * @retval Current Tick
  */
int32_t NFCMEM_IO_Tick(void)
{
  return HAL_GetTick();
}
/******************************** LINK NFCTAG *****************************/
/**
  * @brief  This functions converts HAL status to NFCTAG status
  * @param  status : HAL status to convert
  * @retval NFCTAG enum status
  */
int32_t NFCTAG_ConvertStatus( const HAL_StatusTypeDef status )
{
  switch( status )
  {
    case HAL_OK:
      return NFCTAG_OK;
    case HAL_ERROR:
      return NFCTAG_ERROR;
    case HAL_BUSY:
      return NFCTAG_BUSY;
    case HAL_TIMEOUT:
      return NFCTAG_TIMEOUT;
    
    default:
      return NFCTAG_TIMEOUT;
  }
}

/**
  * @brief  Configures nfctag I2C interface
  * @param  None
  * @retval NFCTAG enum status
  */
static int32_t NFCTAG_IO_Init( void )
{
#if defined(USE_ST25DV)
  /* Set IT raising by default, as we don't know the correct value yet */
  BSP_GPO_Init( GPIO_MODE_IT_RISING );
  
  BSP_LPD_Init( );
#endif /* USE_ST25DV */
  return NFCTAG_ConvertStatus( STM32_I2C2_Init() );
}


static int32_t NFCTAG_IO_DeInit( void )
{
  return STM32_I2C2_DeInit() ;
}

/**
  * @brief  Write at specific address nfctag memory
  * @param  pData : pointer to the data to write
  * @param  DevAddr : Target device address
  * @param  TarAddr : I2C data memory address to write
  * @param  Size : Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t NFCTAG_IO_MemWrite( const uint8_t * const pData, const uint8_t DevAddr, 
                                         const uint16_t TarAddr, const uint16_t Size )
{
  return NFCTAG_ConvertStatus( STM32_I2C2_MemWrite(pData, DevAddr, TarAddr, Size) );
}

/**
  * @brief  Read at specific address on nfctag
  * @param  pData : pointer to store read data
  * @param  DevAddr : Target device address
  * @param  TarAddr : I2C data memory address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t NFCTAG_IO_MemRead( uint8_t * const pData, const uint8_t DevAddr, const uint16_t TarAddr, 
                                        const uint16_t Size )
{
  return NFCTAG_ConvertStatus( STM32_I2C2_MemRead(pData, DevAddr, TarAddr, Size) );
}

/**
  * @brief  Read at current address on nfctag
  * @param  pData : pointer to store read data
  * @param  DevAddr : Target device address
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t NFCTAG_IO_Read( uint8_t * const pData, const uint8_t DevAddr, const uint16_t Size )
{
  return NFCTAG_ConvertStatus( STM32_I2C2_Read(pData, DevAddr, Size) );
}

/**
  * @brief  Checks if NACK was received from I2C Slave
  * @param  None
  * @retval 0 ACK, 1 NACK
  */
uint8_t NFCTAG_IO_IsNacked( void )
{
  return STM32_I2C2_IsNacked( );
}

/**
  * @brief  Check nfctag availability
  * @param  DevAddr : Target device address
  * @param  Trials : Number of trials
  * @retval NFCTAG enum status
  */
int32_t NFCTAG_IO_IsDeviceReady( const uint8_t DevAddr, const uint32_t Trials )
{
  return NFCTAG_ConvertStatus( STM32_I2C2_IsDeviceReady(DevAddr, Trials) );
}
#endif /* USE_NFCTAG */
/******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* I2C Routines *********************************/
#if defined(HAL_I2C_MODULE_ENABLED)
/**
  * @brief  I2Cx MSP Initialization
  * @param  hi2c: I2C handle
  */
static void STM32_I2C1_MspInit( I2C_HandleTypeDef *hi2c )
{
  GPIO_InitTypeDef  gpio_initstruct;

  if( hi2c->Instance == ST25_DISCOVERY_I2C1 )
  {
    /* Configure the GPIOs ---------------------------------------------------*/
    /* Enable GPIO clock */
    ST25_DISCOVERY_I2C1_SDA_GPIO_CLK_ENABLE( );
    ST25_DISCOVERY_I2C1_SCL_GPIO_CLK_ENABLE( );
    
    /* Configure I2C Tx as alternate function  */
    gpio_initstruct.Pin       = ST25_DISCOVERY_I2C1_SCL_PIN;
    gpio_initstruct.Mode      = GPIO_MODE_AF_OD;
    gpio_initstruct.Pull      = GPIO_NOPULL;
    gpio_initstruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio_initstruct.Alternate = ST25_DISCOVERY_I2C1_SCL_SDA_AF;
    HAL_GPIO_Init( ST25_DISCOVERY_I2C1_SCL_GPIO_PORT, &gpio_initstruct );
    
    /* Configure I2C Rx as alternate function  */
    gpio_initstruct.Pin = ST25_DISCOVERY_I2C1_SDA_PIN;
    HAL_GPIO_Init( ST25_DISCOVERY_I2C1_SDA_GPIO_PORT, &gpio_initstruct );
    
    /*Configure TSCR IRQ GPIO pin : PE0 */
    gpio_initstruct.Pin = ST25_DISCOVERY_STMPE811_INT_PIN;
    gpio_initstruct.Mode = GPIO_MODE_INPUT;
    gpio_initstruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( ST25_DISCOVERY_STMPE811_INT_GPIO_PORT, &gpio_initstruct );
    /* Configure the Discovery I2Cx peripheral -------------------------------*/
    /* Enable I2C3 clock */
    ST25_DISCOVERY_I2C1_CLOCK_ENABLE( );
    
    /* Force the I2C Peripheral Clock Reset */
    ST25_DISCOVERY_I2C1_FORCE_RESET( );
    
    /* Release the I2C Peripheral Clock Reset */
    ST25_DISCOVERY_I2C1_RELEASE_RESET( );
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority( ST25_DISCOVERY_I2C1_EV_IRQn, 0x00, 0 );
    HAL_NVIC_EnableIRQ( ST25_DISCOVERY_I2C1_EV_IRQn );
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority( ST25_DISCOVERY_I2C1_ER_IRQn, 0x00, 0 );
    HAL_NVIC_EnableIRQ( ST25_DISCOVERY_I2C1_ER_IRQn );  
  }
}

/**
  * @brief  I2Cx Bus initialization.
  */
static void STM32_I2C1_Init( void )
{
  GPIO_InitTypeDef  gpio_initstruct;
  if( HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_RESET )
  {
    /* workaround to avoid blocking the bus if has occured during an i2c transfer  */
    /* Enable GPIO clock */
    ST25_DISCOVERY_I2C1_SDA_GPIO_CLK_ENABLE( );
    ST25_DISCOVERY_I2C1_SCL_GPIO_CLK_ENABLE( );
    
    /* Configure I2C Tx as alternate function  */
    gpio_initstruct.Pin       = ST25_DISCOVERY_I2C1_SCL_PIN;
    gpio_initstruct.Pull      = GPIO_NOPULL;
    gpio_initstruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio_initstruct.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init( ST25_DISCOVERY_I2C1_SCL_GPIO_PORT, &gpio_initstruct );
    
    /* Configure I2C Rx as alternate function  */
    gpio_initstruct.Pin = ST25_DISCOVERY_I2C1_SDA_PIN;
    gpio_initstruct.Mode      = GPIO_MODE_INPUT;
    HAL_GPIO_Init( ST25_DISCOVERY_I2C1_SDA_GPIO_PORT, &gpio_initstruct );
    uint8_t timeout = 100;
    while( (HAL_GPIO_ReadPin( ST25_DISCOVERY_I2C1_SDA_GPIO_PORT, ST25_DISCOVERY_I2C1_SDA_PIN ) == GPIO_PIN_RESET) \
            && timeout-- )
    {
      HAL_GPIO_TogglePin( ST25_DISCOVERY_I2C1_SCL_GPIO_PORT, ST25_DISCOVERY_I2C1_SCL_PIN );
      HAL_Delay( 5 );
    }

    hi2c1.Instance              = ST25_DISCOVERY_I2C1;
    hi2c1.Init.Timing           = ST25_DISCOVERY_I2C1_SPEED;
    hi2c1.Init.OwnAddress1      = 0;
    hi2c1.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2      = 0;
    hi2c1.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;  

    /* Init the I2C */
    STM32_I2C1_MspInit( &hi2c1 );
    HAL_I2C_Init( &hi2c1 );
  }
}

/**
  * @brief  Configures Interruption pin for I2C communication.
  */
static void STM32_I2C1_ITConfig(void)
{
  GPIO_InitTypeDef gpio_initstruct;
  
  /* Enable the GPIO EXTI Clock */
  ST25_DISCOVERY_STMPE811_INT_CLK_ENABLE( );
  
  gpio_initstruct.Pin   = ST25_DISCOVERY_STMPE811_INT_PIN;
  gpio_initstruct.Pull  = GPIO_PULLUP;
  gpio_initstruct.Speed = GPIO_SPEED_FREQ_LOW;
  gpio_initstruct.Mode  = GPIO_MODE_IT_FALLING;
  HAL_GPIO_Init( ST25_DISCOVERY_STMPE811_INT_GPIO_PORT, &gpio_initstruct );
}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  Value: The target register value to be written 
  */
static void STM32_I2C1_WriteData( uint8_t Addr, uint8_t Reg, uint8_t Value )
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write( &hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout );
  
  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Re-Initialize the BUS */
    STM32_I2C1_Error( );
  }
}

/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  pBuffer: The target register value to be written 
  * @param  Length: buffer size to be written
  */
static void STM32_I2C1_WriteBuffer( uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length )
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write( &hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, I2cxTimeout );

  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Re-Initialize the BUS */
    STM32_I2C1_Error( );
  }
}

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @retval Data read at register address
  */
static uint8_t STM32_I2C1_ReadData( uint8_t Addr, uint8_t Reg )
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read( &hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout );
 
  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Re-Initialize the BUS */
    STM32_I2C1_Error( );
  
  }
  return value;
}

/**
  * @brief  Reads multiple data on the BUS.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
static uint8_t STM32_I2C1_ReadBuffer( uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length )
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read( &hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, I2cxTimeout );
  
  /* Check the communication status */
  if( status == HAL_OK )
  {
    return 0;
  }
  else
  {
    /* Re-Initialize the BUS */
    STM32_I2C1_Error( );

    return 1;
  }
}


/**
  * @brief  I2Cx error treatment function
  */
static void STM32_I2C1_Error( void )
{
  /* De-initialize the SPI communication BUS */
  HAL_I2C_DeInit( &hi2c1 );
  
  /* Re-Initialize the SPI communication BUS */
  STM32_I2C1_Init( );
}

#if defined(USE_NFCTAG)
/**
  * @brief  Configures I2C interface.
  * @param  None
  * @retval HAL status
  */
static HAL_StatusTypeDef STM32_I2C2_Init( void )
{
  HAL_StatusTypeDef ret_val = HAL_OK;
  
  if( HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_RESET )
  {
    /* I2C2 peripheral configuration */
    hi2c2.Instance = ST25_DISCOVERY_I2C2;
    hi2c2.Init.Timing = ST25_DISCOVERY_I2C2_TIMINGS;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    /* Init the I2C */
    STM32_I2C2_MspInit( );
    ret_val = HAL_I2C_Init( &hi2c2 );
  }
  
  return ret_val;
}

HAL_StatusTypeDef STM32_I2C2_DeInit( void )
{
  /* DeInit the I2C */
  STM32_I2C2_MspDeInit( );
  return HAL_I2C_DeInit( &hi2c2 );
  
}

void BSP_I2C2_ReInit( void )
{
  /* DeInit the I2C */
  STM32_I2C2_DeInit();
  
  /* ReInit the I2C */
  STM32_I2C2_Init( );
}


/**
  * @brief  Write data in a register of the device through the bus
  * @param  pData : pointer to the data to write
  * @param  DevAddr : Target device address
  * @param  TarAddr : I2C data memory address to write
  * @param  Size : Size in bytes of the value to be written
  * @retval HAL status
  */

static HAL_StatusTypeDef STM32_I2C2_MemWrite( const uint8_t * const pData, const uint8_t DevAddr, 
                                              const uint16_t TarAddr, const uint16_t Size )
{
  uint8_t *pbuffer = (uint8_t *)pData;
  HAL_StatusTypeDef     ret = HAL_ERROR;

  if ((pbuffer == NULL) && (Size == 0U)) {
    ret = HAL_I2C_Master_Transmit( &hi2c2, DevAddr, pbuffer, Size, ST25_DISCOVERY_I2Cx_TIMEOUT );
  }
  else {
    ret = HAL_I2C_Mem_Write( &hi2c2, DevAddr, TarAddr, I2C_MEMADD_SIZE_16BIT, pbuffer, Size, 
                             ST25_DISCOVERY_I2Cx_TIMEOUT );
  }
  
  return ret;
}

/**
  * @brief  Read the value of a register of the device through the bus.
  * @param  pData : pointer to store read data
  * @param  DevAddr : Target device address
  * @param  TarAddr : I2C data memory address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval HAL status.
  */
static HAL_StatusTypeDef STM32_I2C2_MemRead( uint8_t * const pData, const uint8_t DevAddr, const uint16_t TarAddr, 
                                             const uint16_t Size )
{
  uint8_t *pbuffer = (uint8_t *)pData;
  HAL_StatusTypeDef ret;
  
  /* I2C Timeout: (transfer size in bytes) * (bits per bytes) * (extra delay) / (I2C speed) */
  uint32_t timeout = (Size * 8 * 1000 * 2) / ST25_DISCOVERY_I2C2_SPEED;
  if( timeout < ST25_DISCOVERY_I2Cx_TIMEOUT )
  {
    timeout = ST25_DISCOVERY_I2Cx_TIMEOUT;
  }
  
  if ((pbuffer == NULL) && (Size == 0U)) {
    ret = HAL_I2C_Master_Receive( &hi2c2, DevAddr, pbuffer, Size, timeout );
  }
  else {
    ret = HAL_I2C_Mem_Read( &hi2c2, DevAddr, TarAddr, I2C_MEMADD_SIZE_16BIT, pbuffer, Size, timeout );
  }
  
  return ret;
}

/**
  * @brief  Read the value of a register of the device through the bus.
  * @param  pData : pointer to store read data
  * @param  DevAddr : the device address on bus
  * @param  Size : Size in bytes of the value to be read
  * @retval HAL status
  */
static HAL_StatusTypeDef STM32_I2C2_Read( uint8_t * const pData, const uint8_t DevAddr, const uint16_t Size )
{
  uint8_t *pbuffer = (uint8_t *)pData;
  HAL_StatusTypeDef ret;
  
  ret = HAL_I2C_Master_Receive( &hi2c2, DevAddr, pbuffer, Size, ST25_DISCOVERY_I2Cx_TIMEOUT );
  
  return ret;
}

/**
* @brief  Checks if NACK was received from I2C Slave
* @param  None
* @retval 0 ACK, 1 NACK
*/
static uint8_t STM32_I2C2_IsNacked( void )
{
  if( hi2c2.ErrorCode == HAL_I2C_ERROR_AF )
  {
    return I2CANSW_NACK;
  }
  return I2CANSW_ACK;
}

/**
* @brief  Checks if target device is ready for communication
* @param  DevAddr : Target device address
* @param  Trials : Number of trials
* @retval HAL status
*/
static HAL_StatusTypeDef STM32_I2C2_IsDeviceReady( const uint8_t DevAddr, const uint32_t Trials )
{ 
  return HAL_I2C_IsDeviceReady( &hi2c2, DevAddr, Trials, ST25_DISCOVERY_I2Cx_TIMEOUT );
}

/**
  * @brief  I2C MSP Initialization 
  *         This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param  None
  * @return None
  */
static void STM32_I2C2_MspInit( void )
{  
  GPIO_InitTypeDef  gpio_initstruct;
  
  /* Enable I2Cx clock */
  ST25_DISCOVERY_I2C2_CLOCK_ENABLE( );

  /* Reset I2Cx */
  ST25_DISCOVERY_I2C2_FORCE_RESET( );
  ST25_DISCOVERY_I2C2_RELEASE_RESET( );
  
  /* Enable GPIO clock */
  ST25_DISCOVERY_I2C2_SDA_GPIO_CLK_ENABLE( );
  ST25_DISCOVERY_I2C2_SCL_GPIO_CLK_ENABLE( );
  
  /* I2C SCL/SDA GPIO pin configuration  */
  gpio_initstruct.Pin       = ST25_DISCOVERY_I2C2_SCL_PIN;
  gpio_initstruct.Mode      = GPIO_MODE_AF_OD;
  gpio_initstruct.Pull      = GPIO_NOPULL;
  gpio_initstruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_initstruct.Alternate = ST25_DISCOVERY_I2C2_SCL_AF;
  
  HAL_GPIO_Init( ST25_DISCOVERY_I2C2_SCL_GPIO_PORT, &gpio_initstruct );
  
  gpio_initstruct.Pin       = ST25_DISCOVERY_I2C2_SDA_PIN;
  gpio_initstruct.Alternate = ST25_DISCOVERY_I2C2_SDA_AF;
  
  HAL_GPIO_Init( ST25_DISCOVERY_I2C2_SDA_GPIO_PORT, &gpio_initstruct );
}

/**
  * @brief  I2C MSP DeInitialization 
  * @param  None
  * @return None
  */
static void STM32_I2C2_MspDeInit( void )
{  
  
  HAL_GPIO_DeInit( ST25_DISCOVERY_I2C2_SCL_GPIO_PORT, ST25_DISCOVERY_I2C2_SCL_PIN );

  HAL_GPIO_DeInit( ST25_DISCOVERY_I2C2_SDA_GPIO_PORT, ST25_DISCOVERY_I2C2_SDA_PIN );
  
    /* Disable I2Cx clock */
  __HAL_RCC_I2C2_CLK_DISABLE( );
}
#endif /* USE_NFCTAG */

/**
  * @brief  I2C3 MSP Initialization
  * @param  hi2c: I2C handle
  */
static void STM32_I2C3_MspInit( I2C_HandleTypeDef *hi2c )
{
  GPIO_InitTypeDef  gpio_initstruct;

  if( hi2c->Instance == ST25_DISCOVERY_I2C3 )
  {
    /* Configure the GPIOs ---------------------------------------------------*/
    /* Enable GPIO clock */
    ST25_DISCOVERY_I2C3_SDA_GPIO_CLK_ENABLE( );
    ST25_DISCOVERY_I2C3_SCL_GPIO_CLK_ENABLE( );
    
    /* Configure I2C Tx as alternate function  */
    gpio_initstruct.Pin       = ST25_DISCOVERY_I2C3_SCL_PIN;
    gpio_initstruct.Mode      = GPIO_MODE_AF_OD;
    gpio_initstruct.Pull      = GPIO_NOPULL;
    gpio_initstruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio_initstruct.Alternate = ST25_DISCOVERY_I2C3_SCL_SDA_AF;
    HAL_GPIO_Init( ST25_DISCOVERY_I2C3_SCL_GPIO_PORT, &gpio_initstruct );
    
    /* Configure I2C Rx as alternate function  */
    gpio_initstruct.Pin = ST25_DISCOVERY_I2C3_SDA_PIN;
    HAL_GPIO_Init( ST25_DISCOVERY_I2C3_SDA_GPIO_PORT, &gpio_initstruct );
    
    
    /* Configure the Discovery I2Cx peripheral -------------------------------*/ 
    /* Enable I2C3 clock */
    ST25_DISCOVERY_I2C3_CLOCK_ENABLE( );
    
    /* Force the I2C Peripheral Clock Reset */
    ST25_DISCOVERY_I2C3_FORCE_RESET( );
    
    /* Release the I2C Peripheral Clock Reset */
    ST25_DISCOVERY_I2C3_RELEASE_RESET( );
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority( ST25_DISCOVERY_I2C3_EV_IRQn, 0x00, 0 );
    HAL_NVIC_EnableIRQ( ST25_DISCOVERY_I2C3_EV_IRQn );
    
    /* Enable and set Discovery I2Cx Interrupt to the highest priority */
    HAL_NVIC_SetPriority( ST25_DISCOVERY_I2C3_ER_IRQn, 0x00, 0 );
    HAL_NVIC_EnableIRQ( ST25_DISCOVERY_I2C3_ER_IRQn );
  }
}

/**
  * @brief  I2Cx Bus initialization.
  */
static void STM32_I2C3_Init( void )
{
  if( HAL_I2C_GetState(&hi2c3) == HAL_I2C_STATE_RESET )
  {
    hi2c3.Instance              = ST25_DISCOVERY_I2C3;
    hi2c3.Init.Timing           = ST25_DISCOVERY_I2C3_SPEED;
    hi2c3.Init.OwnAddress1      = 0;
    hi2c3.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    hi2c3.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    hi2c3.Init.OwnAddress2      = 0;
    hi2c3.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    hi2c3.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;  
    
    /* Init the I2C */
    STM32_I2C3_MspInit( &hi2c3 );
    HAL_I2C_Init( &hi2c3 );
  }
}


/**
  * @brief  Writes a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.  
  * @param  pBuffer: The target register value to be written 
  * @param  Length: buffer size to be written
  */
static void STM32_I2C3_WriteBuffer( uint8_t Addr,  uint8_t *pBuffer, uint16_t Length )
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Master_Transmit( &hi2c3, Addr, pBuffer, Length, I2cxTimeout );

  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Re-Initialize the BUS */
    STM32_I2C3_Error( );
  }
}

/**
  * @brief  Reads a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.
  * @retval Data read at register address
  */
static uint8_t STM32_I2C3_ReadData( uint8_t Addr, uint16_t Reg )
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  status = HAL_I2C_Mem_Read( &hi2c3, Addr, Reg, I2C_MEMADD_SIZE_16BIT, &value, 1, ST25_DISCOVERY_I2Cx_TIMEOUT);

  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Re-Initialize the BUS */
    STM32_I2C3_Error( );
  
  }
  return value;
}

/**
  * @brief  I2Cx error treatment function
  */
static void STM32_I2C3_Error( void )
{
  /* De-initialize the SPI communication BUS */
  HAL_I2C_DeInit( &hi2c3 );
  
  /* Re-Initialize the SPI communication BUS */
  STM32_I2C3_Init( );
}
#endif /* HAL_I2C_MODULE_ENABLED */

/******************************* SPI Routines *********************************/
#if defined(HAL_SPI_MODULE_ENABLED) && defined(USE_LCD)
/**
  * @brief  Initializes SPI MSP.
  */
static void STM32_SPI2_MspInit( SPI_HandleTypeDef *hspi )
{
  GPIO_InitTypeDef  gpio_initstruct;
  
  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_CLK_ENABLE( );
  ST25_DISCOVERY_SPI2_MISO_MOSI_GPIO_CLK_ENABLE( );
  
  /* Configure SPI NSS */
  gpio_initstruct.Pin = ST25_DISCOVERY_SPI2_NSS_PIN;
  gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_initstruct.Pull  = GPIO_NOPULL;
  gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init( ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_PORT, &gpio_initstruct );
  
  /* Configure SPI SCK */
  gpio_initstruct.Pin = ST25_DISCOVERY_SPI2_SCK_PIN;
  gpio_initstruct.Mode = GPIO_MODE_AF_PP;
  gpio_initstruct.Pull  = GPIO_NOPULL;
  gpio_initstruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpio_initstruct.Alternate = ST25_DISCOVERY_SPI2_AF;
  HAL_GPIO_Init( ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_PORT, &gpio_initstruct );
  
  /* Configure SPI MISO and MOSI */
  gpio_initstruct.Pin = ST25_DISCOVERY_SPI2_MOSI_PIN;
  gpio_initstruct.Alternate = ST25_DISCOVERY_SPI2_AF;
  gpio_initstruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init( ST25_DISCOVERY_SPI2_MISO_MOSI_GPIO_PORT, &gpio_initstruct );
  
  gpio_initstruct.Pin = ST25_DISCOVERY_SPI2_MISO_PIN;
  gpio_initstruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init( ST25_DISCOVERY_SPI2_MISO_MOSI_GPIO_PORT, &gpio_initstruct );

  /*** Configure the SPI peripheral ***/
  /* Enable SPI clock */
  ST25_DISCOVERY_SPI2_CLK_ENABLE( );
}

/**
  * @brief  Initializes SPI HAL.
  */
static void STM32_SPI2_Init( void )
{
  if( HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET )
  {
    /* SPI Config */
    hspi2.Instance = ST25_DISCOVERY_SPI2;
   /* SPI baudrate is set to max (PCLK2/SPI_BaudRatePrescaler = 80/8 = 10 MHz, write only) 
       to verify these constraints:
       - ILI9341 LCD SPI interface max baudrate is 10MHz for write and 6.66MHz for read
       - PCLK2 frequency is set to 80 MHz 
    */
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi2.Init.Direction         = SPI_DIRECTION_1LINE;
    hspi2.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi2.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial     = 7;
    hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi2.Init.NSS               = SPI_NSS_SOFT;
    hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi2.Init.Mode              = SPI_MODE_MASTER;

    STM32_SPI2_MspInit( &hspi2 );
    HAL_SPI_Init( &hspi2 );
  }
}


/**
  * @brief  Initializes SPI HAL for 8 bits data.
  */
static void STM32_SPI2_InitConfig_8BitsData( void )
{
  /* SPI Config */
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  HAL_SPI_Init( &hspi2 );
}

/**
  * @brief  Initializes SPI HAL for 16 bits data.
  */
static void STM32_SPI2_InitConfig_16BitsData( void )
{
  /* SPI Config */
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  HAL_SPI_Init( &hspi2 );
}

/**
  * @brief  Initializes SPI HAL read access (lower freq).
  */
static void STM32_SPI2_InitConfig_LowFreq( void )
{
  /* SPI Config */
  if( hspi2.Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_32 )
  {
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    HAL_SPI_Init( &hspi2 );
  }
}

/**
  * @brief  Initializes SPI HAL write access (higher freq).
  */
static void STM32_SPI2_InitConfig_HighFreq( void )
{
  /* SPI Config */
  if( hspi2.Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_2 )
  {
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    HAL_SPI_Init( &hspi2 );
  }
}

/**
  * @brief  Reads 4 bytes from device.
  * @param  ReadSize: Number of bytes to read (max 4 bytes)
  * @retval Value read on the SPI
  */
static uint32_t STM32_SPI2_Read( uint8_t ReadSize, uint8_t *buffer )
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_SPI_Receive( &hspi2, (uint8_t*)buffer, ReadSize, SpixTimeout );

 
  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Re-Initialize the BUS */
    STM32_SPI2_Error( );
  }
  
  return status;
}


/**
  * @brief  SPI Write a byte to device.
  * @param  Value: value to be written
  */
static void STM32_SPI2_Write( uint16_t Value )
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit( &hspi2, (uint8_t*)&Value, 1, SpixTimeout );

    
  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Execute user timeout callback */
    STM32_SPI2_Error( );
  }
}

/**
  * @brief  SPI Write a buffer to device.
  * @param  Values: values to be written
  */
static void STM32_SPI2_WriteBuffer( uint16_t *Values , uint16_t length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit( &hspi2, (uint8_t*)Values, length, SpixTimeout );

  
  /* Check the communication status */
  if( status != HAL_OK )
  {
    /* Execute user timeout callback */
    STM32_SPI2_Error( );
  }
}


/**
  * @brief  SPI error treatment function.
  */
static void STM32_SPI2_Error( void )
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit( &hspi2 );
  
  /* Re-Initiaize the SPI communication BUS */
  STM32_SPI2_Init( );
}


/******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/
/********************************* LINK LCD ***********************************/
/**
  * @brief  Initializes the LCD.
  */
/**
  * @brief  Configures the LCD_SPI interface.
  */

void LCD_IO_Init( void )
{
  GPIO_InitTypeDef gpio_initstructure;
  
  if( Is_LCD_IO_Initialized == 0 )
  {
    Is_LCD_IO_Initialized = 1; 
    
    /* Configure WRX in Output Push-Pull mode */
    ST25_DISCOVERY_LCD_WRX_GPIO_CLK_ENABLE( );
    gpio_initstructure.Pin     = ST25_DISCOVERY_LCD_WRX_PIN;
    gpio_initstructure.Mode    = GPIO_MODE_OUTPUT_PP;
    gpio_initstructure.Pull    = GPIO_NOPULL;
    gpio_initstructure.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( ST25_DISCOVERY_LCD_WRX_GPIO_PORT, &gpio_initstructure );
       
    /* Configure the LCD Backlight ----------------------------------------*/
    TIM_HandleTypeDef THandle;
    TIM_OC_InitTypeDef sConfig;
    uint32_t uhPrescalerValue = 0;
  
    ST25_DISCOVERY_LCD_BKLIGHT_CLK_ENABLE( );
    
    gpio_initstructure.Pin = ST25_DISCOVERY_LCD_BKLIGHT_PIN;
    gpio_initstructure.Mode = GPIO_MODE_AF_PP;
    gpio_initstructure.Pull = GPIO_NOPULL;
    gpio_initstructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_initstructure.Alternate = GPIO_AF14_TIM15;
    HAL_GPIO_Init( ST25_DISCOVERY_LCD_BKLIGHT_GPIO_PORT, &gpio_initstructure );
    
    /* Compute the prescaler value to have TIM15 PWM equal to 2kHz, fpwm = TIMClock / (Presc * Period)  */
    uhPrescalerValue = (uint32_t)(SystemCoreClock / 80000) - 1;
    
    THandle.Instance = TIM15;

    THandle.Init.Prescaler         = uhPrescalerValue;
    THandle.Init.Period            = ((SystemCoreClock / (uhPrescalerValue + 1)) / 2000) - 1;
    THandle.Init.ClockDivision     = 0;
    THandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    THandle.Init.RepetitionCounter = 0;
    THandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&THandle) != HAL_OK)
    {
      /* Initialization Error */
      //Error_Handler();
    }

    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* Set the pulse value for channel 2
       This value sets the brightness of the LCD backlight:
        - Use the LCD_BKLIGHT_BRIGHT divider for more light
        - Use the LCD_BKLIGHT_DIM divider for energy saving */   
    sConfig.Pulse = (THandle.Init.Period) / LCD_BKLIGHT_DIM;
    if (HAL_TIM_PWM_ConfigChannel(&THandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
    {
      /* Configuration Error */
      //Error_Handler();
    }
    
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&THandle, TIM_CHANNEL_2) != HAL_OK)
    {
      /* PWM Generation Error */
      //Error_Handler();
    }
    
    /* Configure the LCD Control pins ----------------------------------------*/
    STM32_SPI2_Init( );
  }
}
/**
  * @brief  Configures the LCD_SPI interface.
  */
void LCD_IO_SetDataWidth( uint32_t w16 )
{
  if( w16 != hspi2.Init.DataSize )
  {
    if( w16 == SPI_DATASIZE_16BIT )
    {
      STM32_SPI2_InitConfig_16BitsData( );
    }
    else
    {
      STM32_SPI2_InitConfig_8BitsData( );
    }
  }
}

/**
  * @brief  Initializes SPI HAL read access (lower freq).
  */
void BSP_LCD_LowFreq( void )
{
  /* SPI Config */
  STM32_SPI2_InitConfig_LowFreq( );
}

/**
  * @brief  Initializes SPI HAL write access (higher freq).
  */
void BSP_LCD_HighFreq( void )
{
  STM32_SPI2_InitConfig_HighFreq( );
}

/**
  * @brief  Writes 8 bits register address.
  */
void LCD_IO_WriteReg( uint8_t Reg ) 
{
  LCD_IO_SetDataWidth( SPI_DATASIZE_8BIT );
  
  /* Reset WRX to send command */
  ST25_DISCOVERY_LCD_WRX_LOW( );
  
  /* Reset LCD control line(/CS) and Send command */
  ST25_DISCOVERY_LCD_CS_LOW( );
  STM32_SPI2_Write( Reg );
  
  /* Deselect: Chip Select high */
  ST25_DISCOVERY_LCD_CS_HIGH( );
}

/**
  * @brief  Writes writes 8 bits data value.
  */
void LCD_IO_WriteData( uint8_t RegValue )
{
  LCD_IO_SetDataWidth( SPI_DATASIZE_8BIT );

  /* Set WRX to send data */
  ST25_DISCOVERY_LCD_WRX_HIGH( );
  
  /* Reset LCD control line(/CS) and Send data */
  ST25_DISCOVERY_LCD_CS_LOW( );
  STM32_SPI2_Write( RegValue );
  
  /* Deselect: Chip Select high */
  ST25_DISCOVERY_LCD_CS_HIGH( );
}

/**
  * @brief  Writes 16bits data value.
  */
void LCD_IO_WriteData16( uint16_t RegValue )
{
  LCD_IO_SetDataWidth( SPI_DATASIZE_16BIT );
  
  /* Set WRX to send data */
  ST25_DISCOVERY_LCD_WRX_HIGH( );
  
  /* Reset LCD control line(/CS) and Send data */
  ST25_DISCOVERY_LCD_CS_LOW( );
  STM32_SPI2_Write( RegValue );
  
  /* Deselect: Chip Select high */
  ST25_DISCOVERY_LCD_CS_HIGH( );
}

/**
  * @brief  Writes 16bits data value.
  */
void LCD_IO_WriteBuffer16( uint16_t* buffer, uint32_t length )
{
  LCD_IO_SetDataWidth( SPI_DATASIZE_16BIT );
  
  /* Set WRX to send data */
  ST25_DISCOVERY_LCD_WRX_HIGH( );
  
  /* Reset LCD control line(/CS) and Send data */
  ST25_DISCOVERY_LCD_CS_LOW( );
  STM32_SPI2_WriteBuffer( buffer, length );
  
  /* Deselect: Chip Select high */
  ST25_DISCOVERY_LCD_CS_HIGH( );
}

/**
  * @brief  Reads register value.
  * @param  RegValue Address of the register to read
  * @param  ReadSize Number of bytes to read (Max = 4)
  * @retval Content of the register value
  */
uint32_t LCD_IO_ReadData( uint16_t RegValue, uint8_t ReadSize )
{
  uint32_t readvalue = 0;

  if( ReadSize > sizeof(readvalue) )
  {
    ReadSize = 4;
  }
  
  LCD_IO_SetDataWidth( SPI_DATASIZE_8BIT );
  
  /* Reset WRX to send command */
  ST25_DISCOVERY_LCD_WRX_LOW( );
  
  /* Select: Chip Select low */
  ST25_DISCOVERY_LCD_CS_LOW( );

  STM32_SPI2_Write( RegValue );
  STM32_SPI2_Read( ReadSize, (uint8_t *)&readvalue );

  /* Deselect: Chip Select high */
  ST25_DISCOVERY_LCD_CS_HIGH( );

/* Set WRX to send data */
  ST25_DISCOVERY_LCD_WRX_HIGH( );

  /* Changing endianess */
  readvalue = ((readvalue & 0xff)       << 24) | 
              ((readvalue & 0xff00)     << 8)  |
              ((readvalue & 0xff0000)   >> 8)  |
              ((readvalue & 0xff000000) >> 24);
  
  return readvalue;
}

/**
  * @brief  Wait for loop in ms.
  * @param  Delay in ms.
  */
void LCD_Delay( uint32_t Delay )
{
  HAL_Delay( Delay );
}
#endif /* HAL_SPI_MODULE_ENABLED && USE_LCD */

/*******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/

/********************************* LINK IOE ***********************************/
#if defined(HAL_I2C_MODULE_ENABLED)
/**
  * @brief  IOE Low Level Initialization.
  */
void IOE_Init( void ) 
{
  STM32_I2C1_Init( );
}

/**
  * @brief  IOE Low Level Interrupt configuration.
  */
void IOE_ITConfig( void )
{
  STM32_I2C1_ITConfig( );
}

/**
  * @brief  IOE Writes single data operation.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  Value: Data to be written
  */
void IOE_Write( uint8_t Addr, uint8_t Reg, uint8_t Value )
{
  STM32_I2C1_WriteData( Addr, Reg, Value );
}

/**
  * @brief  IOE Reads single data.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @retval The read data
  */
uint8_t IOE_Read( uint8_t Addr, uint8_t Reg )
{
  return STM32_I2C1_ReadData( Addr, Reg );
}

/**
  * @brief  IOE Writes multiple data.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to data buffer
  * @param  Length: length of the data
  */
void IOE_WriteMultiple( uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length )
{
  STM32_I2C1_WriteBuffer( Addr, Reg, pBuffer, Length );
}

/**
  * @brief  IOE Reads multiple data.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address 
  * @param  pBuffer: pointer to data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
uint16_t IOE_ReadMultiple( uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length )
{
 return STM32_I2C1_ReadBuffer( Addr, Reg, pBuffer, Length );
}

/**
  * @brief  IOE Delay.
  * @param  Delay in ms
  */
void IOE_Delay( uint32_t Delay )
{
  HAL_Delay( Delay );
}

/********************************* LINK DPOT *****************************/
/**
  * @brief  Initializes the Digital potentiometer.
  */
/**
  * @brief  Configures the DPOT_I2C interface.
  */
void DPOT_IO_Init( void )
{
  STM32_I2C3_Init( );
}

/**
  * @brief  DPOT Reads single data.
  * @param  Addr: I2C Address.
  * @param  Reg: Reg Address.
  * @retval The read data.
  */
uint8_t DPOT_IO_Read( uint8_t Addr, uint16_t Reg )
{
  return STM32_I2C3_ReadData( Addr, Reg );
}

/**
  * @brief  DPOT Writes buffer.
  * @param  Addr: I2C Address.
  * @param  pBuf: pointer to data buffer.
  * @param  Len: length of the data.
  * @return None.
  */
void DPOT_IO_WriteBuffer( uint8_t Addr, uint8_t *pBuf, uint16_t Len )
{
  STM32_I2C3_WriteBuffer( Addr, pBuf, Len );
}
#endif /* HAL_I2C_MODULE_ENABLED */

/******************************* ADC driver ********************************/
#if defined(HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV)
/**
  * @brief  Initializes ADC MSP.
  */
static void STM32_ADC1_MspInit( ADC_HandleTypeDef *hadc )
{
  GPIO_InitTypeDef  gpio_initstruct;
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_CLK_ENABLE( );
  ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_CLK_ENABLE( );
  ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_CLK_ENABLE( );

  /* Configure the selected ADC Channel as analog input */
  gpio_initstruct.Pin = ST25_DISCOVERY_EH_1ST_CHANNEL_PIN ;
  gpio_initstruct.Mode = ST25_DISCOVERY_EH_MODE;
  gpio_initstruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init( ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_PORT, &gpio_initstruct );
  
  gpio_initstruct.Pin = ST25_DISCOVERY_EH_2ND_CHANNEL_PIN ;
  gpio_initstruct.Mode = ST25_DISCOVERY_EH_MODE;
  gpio_initstruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init( ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_PORT, &gpio_initstruct );
  
  gpio_initstruct.Pin = ST25_DISCOVERY_EH_3RD_CHANNEL_PIN ;
  gpio_initstruct.Mode = ST25_DISCOVERY_EH_MODE;
  gpio_initstruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init( ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_PORT, &gpio_initstruct );
  
  /*** Configure the ADC peripheral ***/ 
  /* Enable ADC clock */
  ST25_DISCOVERY_EH_ADC_CLK_ENABLE( );
  
}

/**
  * @brief  DeInitializes ADC MSP.
  * @note ADC DeInit does not disable the GPIO clock
  */
static void STM32_ADC1_MspDeInit( ADC_HandleTypeDef *hadc )
{
  GPIO_InitTypeDef  gpio_initstruct;

  /*** DeInit the ADC peripheral ***/
  /* Disable ADC clock */
  ST25_DISCOVERY_EH_ADC_CLK_DISABLE( );

  /* Configure the selected ADC Channel as analog input */
  gpio_initstruct.Pin =  ST25_DISCOVERY_EH_1ST_CHANNEL_PIN;
  HAL_GPIO_DeInit( ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_PORT, gpio_initstruct.Pin );

  gpio_initstruct.Pin =  ST25_DISCOVERY_EH_2ND_CHANNEL_PIN;
  HAL_GPIO_DeInit( ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_PORT, gpio_initstruct.Pin );

  gpio_initstruct.Pin =  ST25_DISCOVERY_EH_3RD_CHANNEL_PIN;
  HAL_GPIO_DeInit( ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_PORT, gpio_initstruct.Pin );
}

/**
  * @brief  Initializes ADC HAL.
  */
static void STM32_ADC1_Init( void )
{
  if( HAL_ADC_GetState( &hadc1 ) == HAL_ADC_STATE_RESET )
  {
    /* ADC Config */
    hadc1.Instance                   = ST25_DISCOVERY_EH_ADC;
    hadc1.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4; /* (must not exceed 36MHz) */
    hadc1.Init.Resolution            = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode          = ENABLE;
    hadc1.Init.ContinuousConvMode    = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    hadc1.Init.NbrOfConversion       = ST25_DISCOVERY_EH_CHANNELn;
    hadc1.Init.DMAContinuousRequests = DISABLE; 
    hadc1.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;
    hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    hadc1.Init.LowPowerAutoWait      = DISABLE;
    hadc1.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.OversamplingMode      = DISABLE;
   
    STM32_ADC1_MspInit( &hadc1 );
    HAL_ADC_Init( &hadc1 );
  }
}

/**
  * @brief  DeInitializes ADC HAL.
  */
static void STM32_ADC1_DeInit( void )
{
  hadc1.Instance = ST25_DISCOVERY_EH_ADC;
  
  HAL_ADC_DeInit( &hadc1 );
  STM32_ADC1_MspDeInit( &hadc1 );
}
#endif /* HAL_ADC_MODULE_ENABLED && USE_ST25DV */

/******************************* OPAMP driver ********************************/
#if defined(HAL_OPAMP_MODULE_ENABLED) && defined(USE_ST25DV)
/**
  * @brief  Initializes OPAMP GPIO, Clock, Pin.
  */
static void STM32_OPAMP1_MspInit( OPAMP_HandleTypeDef *hOpamp )
{
  GPIO_InitTypeDef  gpio_initstruct;
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_CLK_ENABLE( );

  /* Configure the selected OPAMP pin as analog input */
  gpio_initstruct.Pin = ST25_DISCOVERY_EH_1ST_CHANNEL_PIN ;
  gpio_initstruct.Mode = ST25_DISCOVERY_EH_MODE;
  gpio_initstruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init( ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_PORT, &gpio_initstruct );
  
  /*** Configure the OPAMP peripheral ***/ 
  /* Enable OPAMP clock */
  ST25_DISCOVERY_EH_OPAMP_CLK_ENABLE( );
  
}

/**
  * @brief  DeInitializes OPAMP GPIO, Clock, Pin.
  * @note OPAMP DeInit does not disable the GPIO clock
  */
static void STM32_OPAMP1_MspDeInit( OPAMP_HandleTypeDef *hOpamp )
{
  GPIO_InitTypeDef  gpio_initstruct;

  /*** DeInit the OPAMP peripheral ***/
  /* Disable OPAMP clock */
  ST25_DISCOVERY_EH_OPAMP_CLK_DISABLE( );

  /* Configure the selected GPIO Pin as analog input */
  gpio_initstruct.Pin =  ST25_DISCOVERY_EH_1ST_CHANNEL_PIN;
  HAL_GPIO_DeInit( GPIOA, gpio_initstruct.Pin );
}

/**
  * @brief  Initializes OPAMP HAL peripheral.
  */
static void STM32_OPAMP1_Init( void )
{
  if( HAL_OPAMP_GetState( &hOpamp1 ) == HAL_OPAMP_STATE_RESET )
  {
    /* OPAMP Config */
    hOpamp1.Instance               = ST25_DISCOVERY_EH_OPAMP;
    HAL_OPAMP_DeInit( &hOpamp1 );
    hOpamp1.Init.PowerSupplyRange  = OPAMP_POWERSUPPLY_HIGH; 
    hOpamp1.Init.PowerMode         = OPAMP_POWERMODE_NORMAL; 
    hOpamp1.Init.Mode              = OPAMP_FOLLOWER_MODE;  
    hOpamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
    hOpamp1.Init.InvertingInput    = OPAMP_INVERTINGINPUT_IO0; 
    hOpamp1.Init.UserTrimming      = OPAMP_TRIMMING_FACTORY;

    /* Init */
    STM32_OPAMP1_MspInit( &hOpamp1 );
    HAL_OPAMP_Init( &hOpamp1 );
  }
}

/**
  * @brief  DeInitializes OPAMP HAL peripheral.
  */
static void STM32_OPAMP1_DeInit( void )
{
  hOpamp1.Instance = ST25_DISCOVERY_EH_OPAMP;
  
  HAL_OPAMP_DeInit( &hOpamp1 );
  STM32_OPAMP1_MspDeInit( &hOpamp1 );
}
#endif /* HAL_OPAMP_MODULE_ENABLED && USE_ST25DV */

/******************************* LINK ST25DV EH ********************************/
#if defined(HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV)
/**
  * @brief  Configures ADC for Energy Harvesting measurement from ST25DV
  * @retval (0=> success, 1=> fail) 
  */
uint8_t BSP_ST25DV_EH_Init( uint32_t *vdda )
{
  uint8_t status = HAL_OK;
  uint16_t vref_int = 0;
  
  STM32_OPAMP1_Init( );
  
  HAL_OPAMP_Init( &hOpamp1 );
  
  STM32_ADC1_Init( );
  
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
  sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
  sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */
  
  status |= HAL_ADC_ConfigChannel(&hadc1, &sConfig);
  
  status |= HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  
  HAL_Delay(1);
  
  BSP_ST25DV_EH_Measure( &vref_int );
  
  *vdda = (3000 * (*VREFINT_CAL_ADDR)) / vref_int;
  
  sConfig.Channel = ADC1_Channel_list[0];
  status |= HAL_ADC_ConfigChannel( &hadc1, &sConfig );
  
  /* Return EH meas initialization status */
  return status;
}

/**
 * @brief   This function starts the ADC and poll for conversions, stops when the sequence of conversion is over
 * @param   values	Table to save converted values 
 * @retval  None
 */
void BSP_ST25DV_EH_Measure( uint16_t *values )
{
    int i;
    
    HAL_ADC_Start( &hadc1 );
    for( i = 0; i < hadc1.Init.NbrOfConversion; i++ )
    {
      HAL_ADC_PollForConversion( &hadc1, 10 );
      values[i] =  HAL_ADC_GetValue( &hadc1 );
    }
    HAL_ADC_Stop( &hadc1 );
}

/**
  * @brief  Deconfigures ADC for Energy Harvesting measurement from ST25DV  
  * @retval None 
  */
void BSP_ST25DV_EH_DeInit( void )
{
  STM32_ADC1_DeInit( );
  STM32_OPAMP1_DeInit( );
}

#endif /* HAL_ADC_MODULE_ENABLED && USE_ST25DV */

/**
  * @}
  */

/**
  * @}
  */    

/**
  * @}
  */ 
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
