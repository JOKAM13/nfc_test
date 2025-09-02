/**
  ******************************************************************************
  * @file    st25_discovery_st25r.h
  * @author  MMY Application Team
  * @brief   This file provides set of driver functions to manage communication 
  * @brief   between MCU and ST25R chip 
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
#ifndef ST25_DISCOVERY_ST25R
#define ST25_DISCOVERY_ST25R

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "rfal_nfc.h"
#include "rfal_nfca.h"
#include "rfal_nfcb.h"
#include "rfal_nfcf.h"
#include "rfal_nfcv.h"
#include "rfal_st25tb.h"
#include "rfal_isoDep.h"
#include "rfal_nfcDep.h"

/** Type for Callback for long waiting time */
typedef uint8_t (BSP_NFCTAG_WaitingCallback_t)(void);
/** Callback for NFCA long waiting time */
extern BSP_NFCTAG_WaitingCallback_t *BSP_NFCTAG_WaitingCb;

/** Porotcol API structure */
typedef struct {
bool *NdefSupport;
uint8_t (*activate)(void);                              /** Activation phase API */
uint8_t (*read_data)(uint8_t* , uint32_t , uint32_t );  /** Read method */
uint8_t (*write_data)(uint8_t* , uint32_t , uint32_t ); /** Write method */
} BSP_NFCTAG_Protocol_t;

uint8_t BSP_NFCTAG_Init(rfalNfcDevice * device);
uint8_t BSP_NFCTAG_Activate(rfalNfcDevice * device);
uint8_t BSP_NFCTAG_ReadData(uint8_t* buffer, uint32_t offset, uint32_t length);
uint8_t BSP_NFCTAG_WriteData(uint8_t* buffer, uint32_t offset, uint32_t length);
uint16_t BSP_NFCTAG_GetByteSize(void);
uint8_t BSP_NFCTAG_T4_SelectFile(uint16_t fileId);
uint8_t BSP_NFCTAG_CheckVicinity(void);
bool BSP_NFCTAG_GetNdefSupport(void);

#ifdef __cplusplus
  }
#endif

#endif /* ST25_DISCOVERY_ST25R */

/******************* (C) COPYRIGHT 2021 STMicroelectronics *****END OF FILE****/
