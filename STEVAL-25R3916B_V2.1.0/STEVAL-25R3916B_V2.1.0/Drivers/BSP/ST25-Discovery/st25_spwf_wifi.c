/**
  ******************************************************************************
  * @file    st25_spwf_wifi.c
  * @author  MMY Application Team
  * @brief   HAL related functionality of ST25-discovery Wifi module
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

/* Includes ------------------------------------------------------------------*/
#include "wifi_module.h"
#include "st25_spwf_wifi.h"
#include "ring_buffer.h"

/** @addtogroup BSP
* @{
*/ 

/** @addtogroup ST25_DISCOVERY_BSP
* @{
*/ 

/** @defgroup  ST25_DISCOVERY_WIFI ST25 DISCOVERY WIFI
  * @brief Wi-Fi_driver modules
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;  

void HAL_GPIO_Init1(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle, PushTimHandle;
extern UART_HandleTypeDef UartMsgHandle;
extern UART_HandleTypeDef UartWiFiHandle;//main USART handle

/* Private variables ---------------------------------------------------------*/

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit_unused(UART_HandleTypeDef *huart)
{ 
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (huart==&UartWiFiHandle)
  {
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	USARTx_TX_GPIO_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();
	
	
	/* Enable USARTx clock */
	USARTx_CLK_ENABLE(); 
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	
	/*##-2- Configure peripheral GPIO ##########################################*/  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = WiFi_USART_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = WiFi_USARTx_TX_AF;  
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(WiFi_USART_TX_GPIO_PORT, &GPIO_InitStruct);
	
	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = WiFi_USART_RX_PIN; 
	GPIO_InitStruct.Alternate = WiFi_USARTx_RX_AF;
	
	HAL_GPIO_Init(WiFi_USART_RX_GPIO_PORT, &GPIO_InitStruct);
	
	
	/* UART RTS GPIO pin configuration  */
	GPIO_InitStruct.Pin = WiFi_USART_RTS_PIN;  
	GPIO_InitStruct.Pull     = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = WiFi_USARTx_RX_AF;
	
	HAL_GPIO_Init(WiFi_USART_RTS_GPIO_PORT, &GPIO_InitStruct);
	
	/*##-3- Configure the NVIC for UART ########################################*/
	/* NVIC for USART */
	HAL_NVIC_SetPriority(USARTx_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(USARTx_IRQn);
  }
}


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit_unused(UART_HandleTypeDef *huart)
{
  if (huart==&UartWiFiHandle)
  {
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(WiFi_USART_TX_GPIO_PORT, WiFi_USART_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(WiFi_USART_RX_GPIO_PORT, WiFi_USART_RX_PIN);
  
  /*##-3- Disable the NVIC for UART ##########################################*/
  HAL_NVIC_DisableIRQ(USARTx_IRQn);
  }
}

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim==&PushTimHandle)
  {
    /* TIMx Peripheral clock enable */
    TIMp_CLK_ENABLE();
    
    /*##-2- Configure the NVIC for TIMx ########################################*/
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(TIMp_IRQn, 3, 0);
    
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIMp_IRQn);
  }
  else
  {    
    /* TIMx Peripheral clock enable */
    TIMx_CLK_ENABLE();
    
    /*##-2- Configure the NVIC for TIMx ########################################*/
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(TIMx_IRQn, 3, 0);
    
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIMx_IRQn);
    
  }
}

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
    since APB1 prescaler is set to 4 (0x100).
       TIM3CLK = PCLK1*2
       PCLK1   = HCLK/2
    => TIM3CLK = PCLK1*2 = (HCLK/2)*2 = HCLK = SystemCoreClock
    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f1xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */
void Timer_Config()
{
  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;
  
  /* Set TIMx instance */
  TimHandle.Instance = TIMx;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 100 - 1;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
}

/**
  * @brief Push_Timer_Config
  *        This function configures the Push Timer
  * @param None
  * @retval None
  */
void Push_Timer_Config()
{
  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;
  
  /* Set TIMx instance */
  PushTimHandle.Instance = TIMp;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  PushTimHandle.Init.Period            = 10 - 1;//10000
  PushTimHandle.Init.Prescaler         = uwPrescalerValue;
  PushTimHandle.Init.ClockDivision     = 0;
  PushTimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;

  if (HAL_TIM_Base_Init(&PushTimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
}

/**
* @brief  USART_Configuration
* WB_WIFI_UART configured as follow:
*      - BaudRate = 115200 baud  
*      - Word Length = 8 Bits
*      - One Stop Bit
*      - No parity
*      - Hardware flow control enabled (RTS and CTS signals)
*      - Receive and transmit enabled
*
* @param  None
* @retval None
*/
void UART_Configuration(uint32_t baud_rate)
{
  UartWiFiHandle.Instance             = WB_WIFI_UART;
  UartWiFiHandle.Init.BaudRate        = baud_rate;
  UartWiFiHandle.Init.WordLength      = UART_WORDLENGTH_8B;
  UartWiFiHandle.Init.StopBits        = UART_STOPBITS_1;
  UartWiFiHandle.Init.Parity          = UART_PARITY_NONE ;
  UartWiFiHandle.Init.HwFlowCtl       = UART_HWCONTROL_RTS;//UART_HWCONTROL_NONE;
  UartWiFiHandle.Init.Mode            = UART_MODE_TX_RX;
  UartWiFiHandle.Init.OverSampling    = UART_OVERSAMPLING_16;
  //UartWiFiHandle.Init.OneBitSampling  = UART_ONEBIT_SAMPLING_ENABLED;
  
  if(HAL_UART_DeInit(&UartWiFiHandle) != HAL_OK)
  {
    Error_Handler();
  }  
  if(HAL_UART_Init(&UartWiFiHandle) != HAL_OK)
  {
    Error_Handler();
  }  
 }


#ifdef USART_PRINT_MSG
void USART_PRINT_MSG_Configuration(uint32_t baud_rate)
{
  UartMsgHandle.Instance             = WIFI_UART_MSG;
  UartMsgHandle.Init.BaudRate        = baud_rate;
  UartMsgHandle.Init.WordLength      = UART_WORDLENGTH_8B;
  UartMsgHandle.Init.StopBits        = UART_STOPBITS_1;
  UartMsgHandle.Init.Parity          = UART_PARITY_NONE ;
  UartMsgHandle.Init.HwFlowCtl       = UART_HWCONTROL_NONE;// USART_HardwareFlowControl_RTS_CTS;
  UartMsgHandle.Init.Mode            = UART_MODE_TX_RX;

  if(HAL_UART_DeInit(&UartMsgHandle) != HAL_OK)
  {
    Error_Handler();
  }  
  if(HAL_UART_Init(&UartMsgHandle) != HAL_OK)
  {
    Error_Handler();
  }
      
}


void UART_Msg_Gpio_Init()
{ 
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_PRINT_TX_GPIO_CLK_ENABLE();
  USARTx_PRINT_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_PRINT_CLK_ENABLE(); 
    __SYSCFG_CLK_ENABLE();
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = WiFi_USART_PRINT_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = PRINTMSG_USARTx_TX_AF;
  HAL_GPIO_Init(WiFi_USART_PRINT_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = WiFi_USART_PRINT_RX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = PRINTMSG_USARTx_RX_AF;
  
  HAL_GPIO_Init(WiFi_USART_PRINT_RX_GPIO_PORT, &GPIO_InitStruct);
  
  #ifdef WIFI_USE_VCOM
    /*##-3- Configure the NVIC for UART ########################################*/
    /* NVIC for USART */
    HAL_NVIC_SetPriority(USARTx_PRINT_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USARTx_PRINT_IRQn);
  #endif
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
