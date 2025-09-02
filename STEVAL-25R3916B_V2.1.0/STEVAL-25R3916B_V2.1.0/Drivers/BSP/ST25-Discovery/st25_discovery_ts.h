/**
  ******************************************************************************
  * @file    st25_discovery_ts.h
  * @author  MCD Application Team
  * @version V2.1.3
  * @date    13-January-2016
  * @brief   This file contains all the functions prototypes for the
  *          st25_discovery_ts.c driver.
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
#ifndef ST25_DISCOVERY_TS_H
#define ST25_DISCOVERY_TS_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include "st25_discovery.h"
/* Include TouchScreen component driver */
#include "../Components/stmpe811/stmpe811.h"   
   
/** @addtogroup BSP
  * @{
  */

/** @addtogroup ST25_DISCOVERY_BSP
  * @{
  */ 

/** @addtogroup ST25_DISCOVERY_TS ST25 Discovery Touchscreen
  * @{
  */

typedef struct
{
  uint16_t TouchDetected;
  uint16_t X;
  uint16_t Y;
  uint16_t Z;
}TS_StateTypeDef;

#define TS_SWAP_NONE                    0x00
#define TS_SWAP_X                       0x01
#define TS_SWAP_Y                       0x02
#define TS_SWAP_XY                      0x04

typedef enum 
{
  TS_OK       = 0x00,
  TS_ERROR    = 0x01,
  TS_TIMEOUT  = 0x02
}TS_StatusTypeDef;
uint8_t BSP_TS_Init(uint16_t XSize, uint16_t YSize);
void    BSP_TS_GetState(TS_StateTypeDef *TsState);
uint8_t BSP_TS_ITConfig(void);
uint8_t BSP_TS_ITGetStatus(void);
void    BSP_TS_ITClear(void);


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

#endif /* ST25_DISCOVERY_TS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
