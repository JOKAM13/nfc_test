/**
  ******************************************************************************
  * @file    flash_if.h
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @brief   Header file for flash_if.c
  ******************************************************************************
 * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
   
/** @addtogroup Flash_Interface
  * @{
  */
   
   
/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Exported constants --------------------------------------------------------*/
/* Define the flash memory start address */
#define USER_FLASH_STARTADDRESS    ((uint32_t)0x08000000) 

/* Define the address from where firmware will be stored. */
#define FIRMWARE_ADDRESS        0x080A0000      /* Upgraded firmware base address */

/* Last Page Address for firmware */
#define FIRMWARE_FLASH_LAST_PAGE_ADDRESS  0x080DFFFF - 4      /* Upgraded firmware last address */

/* Define the user application size */
#define FIRMWARE_FLASH_SIZE   (FIRMWARE_FLASH_LAST_PAGE_ADDRESS - FIRMWARE_ADDRESS + 1)     /* Upgraded firmware area size */

/* Define the address from where user data will be stored. */
#define USER_DATA_ADDRESS        0x080E0000      /* User data base address */

/* Last Page Address for data */
#define USER_DATA_FLASH_LAST_PAGE_ADDRESS  0x080FFFFF - 4   /* User data last address */

/* Define the user data size */
#define USER_DATA_FLASH_SIZE   (USER_DATA_FLASH_LAST_PAGE_ADDRESS - USER_DATA_ADDRESS + 1)    /* User data area size */

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
void FLASH_If_FlashLock( void );
void FLASH_If_FlashUnlock( void );
FlagStatus FLASH_If_ReadOutProtectionStatus( void );
uint32_t FLASH_If_GetPage(uint32_t Addr);
uint32_t FLASH_If_GetBank(uint32_t Addr);
uint32_t FLASH_If_MassErase( const uint32_t Address );
uint32_t FLASH_If_PageErase( const uint32_t Address, const uint32_t LastAddress );
void FLASH_If_DMA_Init(void);
void FLASH_If_DMA_DeInit( void );
uint32_t FLASH_If_DMA_WriteBuffer( const uint32_t Address, const uint32_t * const pData , const uint32_t Size );
uint32_t FLASH_If_DMA_IT_WriteBuffer( const uint32_t Address, const uint32_t * const pData , const uint32_t Size );
uint32_t FLASH_If_WriteDWord( const uint32_t Address, const uint64_t Data );
uint32_t FLASH_If_WriteBuffer( const uint32_t Address, const uint64_t * const pData , const uint32_t Size );
uint32_t FLASH_If_ConfBFB2( void );

/** @} */
#ifdef __cplusplus
}
#endif

#endif  /* __FLASH_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
