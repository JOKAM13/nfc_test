/**
  ******************************************************************************
  * @file    fw_command.h 
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @brief   Header file for fw_command.c 
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
#ifndef __COMMAND_H
#define __COMMAND_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#if defined(ST25_DISCOVERY_MB1283_B)
  #include "flashf4_if.h"
#elif defined(ST25_DISCOVERY_MB1283_C)
   #include "flashl4_if.h"
 #endif
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#if defined   (__GNUC__)        /* GNU Compiler */
  #define __ALIGN64_END    __attribute__ ((aligned (8)))
  #define __ALIGN64_BEGIN         
#else                           
  #define __ALIGN64_END
  #if defined   (__CC_ARM)      /* ARM Compiler */
    #define __ALIGN64_BEGIN    __align(8)  
  #elif defined (__ICCARM__)    /* IAR Compiler */
    #define __ALIGN64_BEGIN 
  #elif defined  (__TASKING__)  /* TASKING Compiler */
    #define __ALIGN64_BEGIN    __align(8) 
  #endif /* __CC_ARM */  
#endif /* __GNUC__ */ 
/* Exported functions ------------------------------------------------------- */
uint32_t COMMAND_EraseFlash( const uint32_t Address );
uint32_t Command_WriteBufferToFlash( const uint32_t StartAddress, const uint32_t offset, const uint8_t * const pData, const uint32_t size );
void COMMAND_Jump( void );

#ifdef __cplusplus
}
#endif

#endif  /* __COMMAND_H */

/************************ (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
