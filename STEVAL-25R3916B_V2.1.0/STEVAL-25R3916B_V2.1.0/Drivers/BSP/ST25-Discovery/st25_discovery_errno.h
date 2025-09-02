/**
  ******************************************************************************
  * @file    st25_discovery_errno.h
  * @author  MMY Application Team
  * @brief   This file contains definitions for ST25Dx-Discovery Kit LEDs,
  *          push-buttons hardware resources.
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
#ifndef ST25_DISCOVERY_ERRNO_H
#define ST25_DISCOVERY_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
   
/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup ST25_DISCOVERY_BSP
  * @{
  */
   
/** @addtogroup ST25_DISCOVERY_LOW_LEVEL
  * @{
  */

/* Common Error codes */
#define BSP_ERROR_NONE 0
#define BSP_ERROR_NO_INIT -1
#define BSP_ERROR_WRONG_PARAM -2
#define BSP_ERROR_BUSY -3
#define BSP_ERROR_PERIPH_FAILURE -4
#define BSP_ERROR_COMPONENT_FAILURE -5
#define BSP_ERROR_UNKNOWN_FAILURE -6
#define BSP_ERROR_UNKNOWN_COMPONENT -7
#define BSP_ERROR_BUS_FAILURE -8
#define BSP_ERROR_CLOCK_FAILURE -9
#define BSP_ERROR_MSP_FAILURE -10
#define BSP_ERROR_FEATURE_NOT_SUPPORTED -11

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

#endif /* ST25_DISCOVERY_ERRNO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
