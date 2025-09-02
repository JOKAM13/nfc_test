/**
  ******************************************************************************
  * @file    st25_bluenrg_ble.h
  * @author  MMY Application Team
  * @brief   
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
#ifndef ST25_BLUENRG_BLE_H
#define ST25_BLUENRG_BLE_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/ 
#include "stm32l4xx_hal.h"
#include "st25_discovery_bluenrg.h"
#include "st25_discovery.h"
#define SYSCLK_FREQ 80000000
#define SYSCLK_FREQ_SLEEP 32000

/** @addtogroup BSP
 *  @{
 */

/** @addtogroup ST25_DISCOVERY_BSP
 *  @{
 */
 
/** @addtogroup ST25_BLUENRG_BLE
 *  @{
 */

  
// FIXME: add prototypes for BlueNRG here
void BNRG_SPI_Init(void);
void BlueNRG_RST(void);
uint8_t BlueNRG_DataPresent(void);
void    BlueNRG_HW_Bootloader(void);
int32_t BlueNRG_SPI_Read_All(uint8_t *buffer,
                             uint8_t buff_size);
int32_t BlueNRG_SPI_Write(uint8_t* data1,
                          uint8_t* data2,
                          uint8_t Nb_bytes1,
                          uint8_t Nb_bytes2);
#ifdef OPTIMIZED_SPI
/* Optimized functions for throughput test */
/* Used by the server (L0 and F4, not L4) */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_Opt(const uint8_t *pTxData, uint8_t *pRxData, uint8_t Size);
HAL_StatusTypeDef HAL_SPI_Transmit_Opt(const uint8_t *pTxData, uint8_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_Opt(uint8_t *pRxData, uint8_t Size);
#endif /* OPTIMIZED_SPI */



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

#endif /* ST25_BLUENRG_BLE_H */
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
