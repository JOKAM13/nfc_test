/**
  ******************************************************************************
  * @file    stm32l4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "st25_bluenrg_ble.h"
#include "st25_spwf_wifi.h"
#include "main.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
 __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
}

/**
  * @brief  Initializes the low level hardware: GPIO, CLOCK, NVIC for SPI.
  * @param  None
  * @retval None
  */
void HAL_SPI_MspInit( SPI_HandleTypeDef* hspi )
{
  GPIO_InitTypeDef gpio_initstruct;
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI
    PE14     ------> SPI1_MISO 
    */
    gpio_initstruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_initstruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio_initstruct);

    gpio_initstruct.Pin = GPIO_PIN_14;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_initstruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOE, &gpio_initstruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }

  if( hspi->Instance == ST25_DISCOVERY_SPI3 )
  {
    /* Peripheral clock enable */
    ST25_DISCOVERY_SPI3_CLK_ENABLE( );
    ST25_DISCOVERY_SPI3_NSS_GPIO_CLK_ENABLE( );
    
    /**SPI3 GPIO Configuration    
    PA15     ------> SPI3_NSS
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI 
    */
    gpio_initstruct.Pin = ST25_DISCOVERY_SPI3_SCK_PIN | ST25_DISCOVERY_SPI3_MISO_PIN | ST25_DISCOVERY_SPI3_MOSI_PIN;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_initstruct.Alternate = ST25_DISCOVERY_SPI3_AF;
    HAL_GPIO_Init( ST25_DISCOVERY_SPI3_GPIO_PORT, &gpio_initstruct );

    gpio_initstruct.Pin = ST25_DISCOVERY_SPI3_NSS_PIN;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull = GPIO_PULLUP;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( ST25_DISCOVERY_SPI3_NSS_PORT, &gpio_initstruct );

    /* Configure the NVIC for SPI */  
    HAL_NVIC_SetPriority(BNRG_SPI_EXTI_IRQn, 3, 0);    
    HAL_NVIC_EnableIRQ(BNRG_SPI_EXTI_IRQn);
  }
}

/**
  * @brief  DeInitializes the low level hardware: GPIO, CLOCK, NVIC for UART.
  * @param  None
  * @retval None
  */
void HAL_SPI_MspDeInit( SPI_HandleTypeDef* hspi )
{
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI
    PE14     ------> SPI1_MISO 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_14);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  if( hspi->Instance == ST25_DISCOVERY_SPI3 )
  {
    /* Peripheral clock disable */
    ST25_DISCOVERY_SPI3_CLK_DISABLE( );
  
    /**SPI3 GPIO Configuration
    PA15     ------> SPI3_NSS
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI 
    */
    HAL_GPIO_DeInit( ST25_DISCOVERY_SPI3_NSS_PORT, ST25_DISCOVERY_SPI3_NSS_PIN);
    HAL_GPIO_DeInit( ST25_DISCOVERY_SPI3_GPIO_PORT, ST25_DISCOVERY_SPI3_SCK_PIN | ST25_DISCOVERY_SPI3_MISO_PIN | ST25_DISCOVERY_SPI3_MOSI_PIN );
  }

}

/**
  * @brief  Initializes the low level hardware: GPIO, CLOCK, NVIC for UART.
  * @param  None
  * @retval None
  */
void HAL_UART_MspInit( UART_HandleTypeDef* huart )
{
  GPIO_InitTypeDef gpio_initstruct;
if( huart->Instance == WB_WIFI_UART )
  {
    /* Peripheral clock enable */
    USARTx_CLK_ENABLE( );
  
    /**USART3 GPIO Configuration    
    PB14     ------> USART3_RTS
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    PD11     ------> USART3_CTS 
    */
    gpio_initstruct.Pin = WiFi_USART_RTS_PIN;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_initstruct.Alternate = WiFi_USARTx_RX_AF;
    HAL_GPIO_Init( WiFi_USART_RTS_GPIO_PORT, &gpio_initstruct );

    gpio_initstruct.Pin = WiFi_USART_TX_PIN | WiFi_USART_RX_PIN;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_PULLUP;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_initstruct.Alternate = WiFi_USARTx_TX_AF;
    HAL_GPIO_Init( WiFi_USART_TX_GPIO_PORT, &gpio_initstruct );

    gpio_initstruct.Pin = WiFi_USART_CTS_PIN;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_initstruct.Alternate = WiFi_USARTx_RX_AF;
    HAL_GPIO_Init( WiFi_USART_CTS_GPIO_PORT, &gpio_initstruct );

    HAL_NVIC_SetPriority( USARTx_IRQn, 3, 0 );

    HAL_NVIC_EnableIRQ( USARTx_IRQn );
  }
  else if( huart->Instance == USART1 )
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    
    gpio_initstruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio_initstruct.Mode = GPIO_MODE_AF_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_initstruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init( GPIOB, &gpio_initstruct );
  }
}

/**
  * @brief  DeInitializes the low level hardware: GPIO, CLOCK, NVIC for UART.
  * @param  None
  * @retval None
  */
void HAL_UART_MspDeInit( UART_HandleTypeDef* huart )
{
  if( huart->Instance == WB_WIFI_UART )
  {
    /* Peripheral clock disable */
    USARTx_CLK_ENABLE( );
  
    /**USART3 GPIO Configuration    
    PB14     ------> USART3_RTS
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    PD11     ------> USART3_CTS 
    */
    HAL_GPIO_DeInit( WiFi_USART_RTS_GPIO_PORT, WiFi_USART_RTS_PIN );

    HAL_GPIO_DeInit( WiFi_USART_TX_GPIO_PORT, WiFi_USART_TX_PIN | WiFi_USART_RX_PIN | WiFi_USART_CTS_PIN );
  }
}

/**
  * @brief  Initializes the low level hardware: GPIO, CLOCK, NVIC for DMA.
  * @param  None
  * @retval None
  */
void HAL_DMA_MspInit( void )
{
  /* Peripheral clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
}

/**
  * @brief  DeInitializes the low level hardware: GPIO, CLOCK, NVIC for DMA.
  * @param  None
  * @retval None
  */
void HAL_DMA_MspDeInit( void )
{
  /* Peripheral clock enable */
  __HAL_RCC_DMA1_CLK_DISABLE();
}

/**
  * @brief  Initializes the low level hardware: GPIO, CLOCK, NVIC for CRC.
  * @param  hcrc: pointer to a CRC_HandleTypeDef structure that contains
  *         the configuration information for CRC module.
  * @retval None
  */
void HAL_CRC_MspInit( CRC_HandleTypeDef *hcrc )
{
  __HAL_RCC_CRC_CLK_ENABLE( );
}

/**
  * @brief  DeInitializes the low level hardware: GPIO, CLOCK, NVIC for CRC.
  * @param  hcrc: pointer to a CRC_HandleTypeDef structure that contains
  *         the configuration information for CRC module.
  * @retval None
  */
void HAL_CRC_MspDeInit( CRC_HandleTypeDef *hcrc )
{
  __HAL_RCC_CRC_CLK_DISABLE( );
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 1
  *            PLL_N                          = 20
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLLSAI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4; 
  PeriphClkInitStruct.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1ClockOut= RCC_PLLSAI1_48M2CLK;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Enable Power Clock*/
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Enable USB power on Pwrctrl CR2 register */
  HAL_PWREx_EnableVddUSB();
}



/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA10   ------> USB_OTG_FS_ID
     PA11   ------> USB_OTG_FS_DM
     PA12   ------> USB_OTG_FS_DP
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure BLE_SPI_CSN GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  
  /*Configure BLE_RESET and WIFI_RESET GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure WIFI RESET GPIO pin : PD10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  /*Configure BLE RESET GPIO pin : PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure BLE SPI CSN GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure BLE SPI IRQ GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/*  3916 inits */


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SPI1_CS_Pin|GPO_3_Pin|GPO_2_Pin|USB_ID_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_FIELD_GPIO_Port, LED_FIELD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SPI1_CS_Pin GPO_3_Pin GPO_2_Pin USB_ID_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin|GPO_3_Pin|GPO_2_Pin|USB_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_FIELD_Pin */
  GPIO_InitStruct.Pin = LED_FIELD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_FIELD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ST25DX_INT_1_Pin */
  GPIO_InitStruct.Pin = ST25DX_INT_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(ST25DX_INT_1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);


}




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
