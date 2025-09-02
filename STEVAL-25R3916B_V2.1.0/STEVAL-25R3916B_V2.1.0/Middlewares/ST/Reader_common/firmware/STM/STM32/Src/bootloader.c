/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2016 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/

/*
 *      PROJECT:   
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author 
 *
 *  \brief 
 *
 */
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "bootloader.h"
#include "stm32l4xx_hal.h"
#include "string.h"

#define BOOTLOADER_MAGIC_INFO_ADDR		0x20017ff0
const char* BtlMagicInfo = ".EnterBooloader.";

void bootloaderEnter( void )
{
  #define SYS_MEM_ADDR    0x1FFF0000    
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
  __set_MSP(*(__IO uint32_t*) SYS_MEM_ADDR);
  void (*Bootloader)(void) = (void(*)(void)) *((uint32_t *) (SYS_MEM_ADDR + 4));
  Bootloader();

  while(1)
    ;
}

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
void bootloaderReboot( void )
{  
  memcpy((void*)BOOTLOADER_MAGIC_INFO_ADDR, BtlMagicInfo, strlen(BtlMagicInfo));
  HAL_NVIC_SystemReset();
	while (1)
		;
}

void bootloaderCheck4Enter( void )
{
  if(0 == memcmp((const void*)BOOTLOADER_MAGIC_INFO_ADDR, BtlMagicInfo, strlen(BtlMagicInfo)))
  {
      *(uint32_t *)BOOTLOADER_MAGIC_INFO_ADDR = 0;
      bootloaderEnter();
  }
}
