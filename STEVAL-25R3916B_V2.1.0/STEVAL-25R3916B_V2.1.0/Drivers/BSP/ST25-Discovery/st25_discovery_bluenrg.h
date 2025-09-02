/**
  ******************************************************************************
  * @file    st25_discovery_bluenrg.h
  * @author  MMY Application Team
  * @brief   This file contains definitions for SPI communication 
  *          BLE BlueNRG Moduke Board.
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
#ifndef ST25_DISCOVERY_BLUENRG_H
#define ST25_DISCOVERY_BLUENRG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "st25_discovery.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup ST25_DISCOVERY
  * @{
  */

/** @defgroup ST25_BLUENRG_BLE
  * @{
  */ 
  
/**
* @brief SPI communication details between Nucleo F4 and BlueNRG
*        Expansion Board.
*/
// SPI Instance
#define BNRG_SPI_INSTANCE           SPI3
#define BNRG_SPI_CLK_ENABLE()       __HAL_RCC_SPI3_CLK_ENABLE()

// SPI Configuration
#define BNRG_SPI_MODE               SPI_MODE_MASTER
#define BNRG_SPI_DIRECTION          SPI_DIRECTION_2LINES
#define BNRG_SPI_DATASIZE           SPI_DATASIZE_8BIT
#define BNRG_SPI_CLKPOLARITY        SPI_POLARITY_LOW
#define BNRG_SPI_CLKPHASE           SPI_PHASE_1EDGE
#define BNRG_SPI_NSS                SPI_NSS_SOFT
#define BNRG_SPI_FIRSTBIT           SPI_FIRSTBIT_MSB
#define BNRG_SPI_TIMODE             SPI_TIMODE_DISABLE
#define BNRG_SPI_CRCPOLYNOMIAL      7
#define BNRG_SPI_BAUDRATEPRESCALER  SPI_BAUDRATEPRESCALER_8
#define BNRG_SPI_CRCCALCULATION     SPI_CRCCALCULATION_DISABLE

// SPI Reset Pin: PD.1
#define BNRG_SPI_RESET_PIN          GPIO_PIN_1
#define BNRG_SPI_RESET_MODE         GPIO_MODE_OUTPUT_PP
#define BNRG_SPI_RESET_PULL         GPIO_PULLUP
#define BNRG_SPI_RESET_SPEED        GPIO_SPEED_FREQ_LOW
#define BNRG_SPI_RESET_ALTERNATE    0
#define BNRG_SPI_RESET_PORT         GPIOD
#define BNRG_SPI_RESET_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

// SCLK: PC.10
#define BNRG_SPI_SCLK_PIN           GPIO_PIN_10
#define BNRG_SPI_SCLK_MODE          GPIO_MODE_AF_PP
#define BNRG_SPI_SCLK_PULL          GPIO_PULLDOWN
#define BNRG_SPI_SCLK_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_SCLK_ALTERNATE     GPIO_AF6_SPI3
#define BNRG_SPI_SCLK_PORT          GPIOC
#define BNRG_SPI_SCLK_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

// MISO (Master Input Slave Output): PC.11
#define BNRG_SPI_MISO_PIN           GPIO_PIN_11
#define BNRG_SPI_MISO_MODE          GPIO_MODE_AF_PP
#define BNRG_SPI_MISO_PULL          GPIO_NOPULL
#define BNRG_SPI_MISO_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_MISO_ALTERNATE     GPIO_AF6_SPI3
#define BNRG_SPI_MISO_PORT          GPIOC
#define BNRG_SPI_MISO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

// MOSI (Master Output Slave Input): PC.12
#define BNRG_SPI_MOSI_PIN           GPIO_PIN_12
#define BNRG_SPI_MOSI_MODE          GPIO_MODE_AF_PP
#define BNRG_SPI_MOSI_PULL          GPIO_NOPULL
#define BNRG_SPI_MOSI_SPEED         GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_MOSI_ALTERNATE     GPIO_AF6_SPI3
#define BNRG_SPI_MOSI_PORT          GPIOC
#define BNRG_SPI_MOSI_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

// NSS/CSN/CS: PD.0
#define BNRG_SPI_CS_PIN             GPIO_PIN_15
#define BNRG_SPI_CS_MODE            GPIO_MODE_OUTPUT_PP
#define BNRG_SPI_CS_PULL            GPIO_PULLUP
#define BNRG_SPI_CS_SPEED           GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_CS_ALTERNATE       0
#define BNRG_SPI_CS_PORT            GPIOA
#define BNRG_SPI_CS_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
// IRQ: PD.2
#define BNRG_SPI_IRQ_PIN            GPIO_PIN_2
#define BNRG_SPI_IRQ_MODE           GPIO_MODE_IT_RISING
#define BNRG_SPI_IRQ_PULL           GPIO_NOPULL
#define BNRG_SPI_IRQ_SPEED          GPIO_SPEED_FREQ_VERY_HIGH
#define BNRG_SPI_IRQ_ALTERNATE      0
#define BNRG_SPI_IRQ_PORT           GPIOD
#define BNRG_SPI_IRQ_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()

// EXTI External Interrupt for SPI
// NOTE: if you change the IRQ pin remember to implement a corresponding handler
// function like EXTI0_IRQHandler() in the user project
#define BNRG_SPI_EXTI_IRQn          EXTI2_IRQn
#define BNRG_SPI_EXTI_IRQHandler    EXTI2_IRQHandler
#define BNRG_SPI_EXTI_PIN           BNRG_SPI_IRQ_PIN
#define BNRG_SPI_EXTI_PORT          BNRG_SPI_IRQ_PORT
#define RTC_WAKEUP_IRQHandler       RTC_WKUP_IRQHandler

  
void Enable_SPI_IRQ(void);
void Disable_SPI_IRQ(void);
void Clear_SPI_IRQ(void);
void Clear_SPI_EXTI_Flag(void);

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

#endif /* ST25_DISCOVERY_BLUENRG_H */
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
