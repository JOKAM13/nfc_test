/**
  ******************************************************************************
  * @file    st25_discovery_dpot.h 
  * @author  MMY Application Team
  * @brief   This file contains all the functions prototypes for the
  *          st25_discovery_dpot.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ST25_DISCOVERY_DPOT_H
#define ST25_DISCOVERY_DPOT_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include "st25_discovery.h"
#include "ad5112.h"
   
/** @addtogroup BSP
  * @{
  */

/** @addtogroup ST25_DISCOVERY_BSP
  * @{
  */ 

/** @addtogroup ST25_DISCOVERY_DPOT
  * @{
  */

typedef enum 
{
  DPOT_OK       = 0x00,
  DPOT_ERROR    = 0x01,
}DPOT_StatusTypeDef;

uint8_t BSP_DPOT_Init( void );
uint32_t BSP_DPOT_GetResistor( void );
void BSP_DPOT_SetResistor( uint32_t Val );
void BSP_DPOT_Reset( void );
void BSP_DPOT_Shutdown( void );
DPOT_ExtDrvTypeDef *BSP_DPOT_GetExtended_Drv( void );

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* ST25_DISCOVERY_DPOT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
