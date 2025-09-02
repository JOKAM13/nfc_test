/**
  ******************************************************************************
  * @file    main.h 
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/myliberty  
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "version.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SplashScreen( void );
void MenuAbout( void );

void rfalPreTransceiveCb(void);

extern void _Error_Handler(char * file, int line);

/* Private define ------------------------------------------------------------*/

#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define LED_FIELD_Pin GPIO_PIN_4
#define LED_FIELD_GPIO_Port GPIOC
#define ST25DX_INT_1_Pin GPIO_PIN_15
#define ST25DX_INT_1_GPIO_Port GPIOE
#define ST25DX_INT_1_EXTI_IRQn EXTI15_10_IRQn
#define GPO_3_Pin GPIO_PIN_8
#define GPO_3_GPIO_Port GPIOA
#define GPO_2_Pin GPIO_PIN_9
#define GPO_2_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
