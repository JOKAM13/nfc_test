  /**
  ******************************************************************************
  * @file    st25_spwf_wifi.h
  * @author  MMY Appli
  * @brief   Header file for HAL related functionality of X-CUBE-WIFI1
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
#ifndef ST25_SPWF_WIFI_H
#define ST25_SPWF_WIFI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "st25_discovery.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_rcc_ex.h"

/* Exported macro ------------------------------------------------------------*/
#define DEBUG_PRINT                              0
#define WiFi_ENABLE
#define FW_UPDATE_PACKET_SIZE                    252
#define MAX_PREFETCH_BUFFER                      20    

/********** TimeOUT *******************/
#define WIFI_TIME_OUT                            40000   //4 sec
#define WIFI_HTTP_REQUEST_INTERVAL               60 //6sec

/********** Wi-Fi *******************/
#ifdef USART_PRINT_MSG

  #define WIFI_UART_MSG                          USART1
  #define USARTx_PRINT_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
  #define USARTx_PRINT_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
  #define USARTx_PRINT_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

  #define USARTx_PRINT_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
  #define USARTx_PRINT_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

  #define PRINTMSG_USARTx_TX_AF                  GPIO_AF7_USART1
  #define PRINTMSG_USARTx_RX_AF                  GPIO_AF7_USART1
  
  #define WiFi_USART_PRINT_TX_PIN                GPIO_PIN_6
  #define WiFi_USART_PRINT_TX_GPIO_PORT          GPIOB
  #define WiFi_USART_PRINT_RX_PIN                GPIO_PIN_7
  #define WiFi_USART_PRINT_RX_GPIO_PORT          GPIOB
  
  /* Definition for USARTx's NVIC */
  #define USARTx_PRINT_IRQn                      USART1_IRQn
  #define USARTx_PRINT_IRQHandler                USART1_IRQHandler

#endif //USART_PRINT_MSG


/* User can use this section to tailor USARTx/UARTx instance used and associated 
resources */
/* Definition for USARTx clock resources */
#define WB_WIFI_UART                             USART3 
#define USARTx_CLK_ENABLE()                      __HAL_RCC_USART3_CLK_ENABLE() 
#define USARTx_RX_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOD_CLK_ENABLE() // not really for RX but for RTS
#define USARTx_TX_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOD_CLK_ENABLE()

#define USARTx_FORCE_RESET()                     __HAL_RCC_USART3_FORCE_RESET() 
#define USARTx_RELEASE_RESET()                   __HAL_RCC_USART3_RELEASE_RESET() 

/* Definition for USARTx Pins */
#define WiFi_USART_TX_PIN                        GPIO_PIN_8
#define WiFi_USART_TX_GPIO_PORT                  GPIOD
#define WiFi_USART_RX_PIN                        GPIO_PIN_9
#define WiFi_USART_RX_GPIO_PORT                  GPIOD

#define WiFi_USART_RTS_PIN                       GPIO_PIN_14
#define WiFi_USART_RTS_GPIO_PORT                 GPIOB
#define WiFi_USART_CTS_PIN                       GPIO_PIN_11
#define WiFi_USART_CTS_GPIO_PORT                 GPIOD  

#define WiFi_USARTx_TX_AF                        GPIO_AF7_USART3
#define WiFi_USARTx_RX_AF                        GPIO_AF7_USART3   

#define  WiFi_RESET_GPIO_PIN                     GPIO_PIN_10
#define  WiFi_RESET_GPIO_PORT                    GPIOD

/* Not used on MB1283 */
#define  WiFi_WAKEUP_GPIO_PIN                    GPIO_PIN_8
#define  WiFi_WAKEUP_GPIO_PORT                   GPIOC /*PC8*/
#define RESET_WAKEUP_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOD_CLK_ENABLE()

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                              USART3_IRQn
#define USARTx_IRQHandler                        USART3_IRQHandler
           
#define USARTx_EXTI_IRQn                         EXTI15_10_IRQn
#define USARTx_EXTI_IRQHandler                   EXTI15_10_IRQHandler

#define TIMx                                     TIM3
#define TIMx_CLK_ENABLE()                        __HAL_RCC_TIM3_CLK_ENABLE()
#define TIMp                                     TIM2
#define TIMp_CLK_ENABLE()                        __HAL_RCC_TIM2_CLK_ENABLE()

/* Definition for TIMx's NVIC */           
#define TIMx_IRQn                                TIM3_IRQn
#define TIMx_IRQHandler                          TIM3_IRQHandler
#define TIMp_IRQn                                TIM2_IRQn
#define TIMp_IRQHandler                          TIM2_IRQHandler

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                             (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                             TXBUFFERSIZE

#define  WiFi_USART_BAUD_RATE                    115200

/* Exported functions ------------------------------------------------------- */
void Push_Timer_Config(void);

#endif /* ST25_SPWF_WIFI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
