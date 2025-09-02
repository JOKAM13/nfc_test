/**
  ******************************************************************************
  * @file    st25_discovery.h
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
#ifndef ST25_DISCOVERY_H
#define ST25_DISCOVERY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

#if defined(USE_NFCTAG) && defined(USE_ST25DV)
#include "st25dv.h"
#include "st25dvxxkc.h"
#endif /* USE_NFCTAG && USE_ST25DV */
   
/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup ST25_DISCOVERY_BSP
  * @{
  */
   
/** @addtogroup ST25_DISCOVERY_LOW_LEVEL
  * @{
  */

/**
  * @brief 
  */
typedef enum 
{
  LED2 = 0,
  LED3
}Led_TypeDef;

typedef enum 
{  
  BUTTON_KEY = 0,
  JOYSTICK_SEL = 1,
  JOYSTICK_LEFT = 2,
  JOYSTICK_RIGHT = 3,
  JOYSTICK_UP = 4,
  JOYSTICK_DOWN = 5,
}Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef; 
    
/**
 * @brief  ST25_DISCOVERY_I2CANSW Ack Nack enumerator definition
 */
typedef enum 
{
  I2CANSW_ACK = 0,
  I2CANSW_NACK
}ST25_DISCOVERY_I2CANSW_TypeDef;


#define ST25_DISCOVERY_LEDn                                       2

#define ST25_DISCOVERY_LED2_PIN                                  GPIO_PIN_4
#define ST25_DISCOVERY_LED2_GPIO_PORT                            GPIOC
#define ST25_DISCOVERY_LED2_GPIO_CLK_ENABLE()                    __HAL_RCC_GPIOC_CLK_ENABLE()  
#define ST25_DISCOVERY_LED2_GPIO_CLK_DISABLE()                   __HAL_RCC_GPIOC_CLK_DISABLE()  

#define ST25_DISCOVERY_LED3_PIN                                  GPIO_PIN_5
#define ST25_DISCOVERY_LED3_GPIO_PORT                            GPIOE
#define ST25_DISCOVERY_LED3_GPIO_CLK_ENABLE()                    __HAL_RCC_GPIOE_CLK_ENABLE()  
#define ST25_DISCOVERY_LED3_GPIO_CLK_DISABLE()                   __HAL_RCC_GPIOE_CLK_DISABLE()

#define ST25_DISCOVERY_LEDx_GPIO_CLK_ENABLE(__INDEX__)  do{if((__INDEX__) == 0) ST25_DISCOVERY_LED2_GPIO_CLK_ENABLE();  \
                                                         if((__INDEX__) == 1) ST25_DISCOVERY_LED3_GPIO_CLK_ENABLE();  \
                                                        }while(0)
#define ST25_DISCOVERY_LEDx_GPIO_CLK_DISABLE(__INDEX__) do{if((__INDEX__) == 0) ST25_DISCOVERY_LED2_GPIO_CLK_DISABLE();  \
                                                         if((__INDEX__) == 0) ST25_DISCOVERY_LED3_GPIO_CLK_DISABLE();  \
                                                        }while(0)

/* JOYSTICK */
#define ST25_DISCOVERY_BUTTONn                                    6

/**
 * @brief Wakeup push-button
 */
#define ST25_DISCOVERY_KEY_BUTTON_PIN                            GPIO_PIN_6
#define ST25_DISCOVERY_KEY_BUTTON_GPIO_PORT                      GPIOE
#define ST25_DISCOVERY_KEY_BUTTON_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOE_CLK_ENABLE()
#define ST25_DISCOVERY_KEY_BUTTON_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOE_CLK_DISABLE()
#define ST25_DISCOVERY_KEY_BUTTON_EXTI_IRQn                      EXTI9_5_IRQn

#define ST25_DISCOVERY_JOYSTICK_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOE_CLK_ENABLE()
#define ST25_DISCOVERY_JOYSTICK_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOE_CLK_DISABLE()

#define ST25_DISCOVERY_JOYSTICK_SEL_GPIO_PIN                      GPIO_PIN_8
#define ST25_DISCOVERY_JOYSTICK_SEL_GPIO_PORT                     GPIOE

#define ST25_DISCOVERY_JOYSTICK_LEFT_GPIO_PIN                     GPIO_PIN_9
#define ST25_DISCOVERY_JOYSTICK_LEFT_GPIO_PORT                    GPIOE

#define ST25_DISCOVERY_JOYSTICK_RIGHT_GPIO_PIN                    GPIO_PIN_11
#define ST25_DISCOVERY_JOYSTICK_RIGHT_GPIO_PORT                   GPIOE

#define ST25_DISCOVERY_JOYSTICK_UP_GPIO_PIN                       GPIO_PIN_10
#define ST25_DISCOVERY_JOYSTICK_UP_GPIO_PORT                      GPIOE

#define ST25_DISCOVERY_JOYSTICK_DOWN_GPIO_PIN                     GPIO_PIN_12
#define ST25_DISCOVERY_JOYSTICK_DOWN_GPIO_PORT                    GPIOE


#define ST25_DISCOVERY_BUTTONx_GPIO_CLK_ENABLE(__INDEX__)  do{if((__INDEX__) == 0) ST25_DISCOVERY_KEY_BUTTON_GPIO_CLK_ENABLE(); else \
                                                            if((__INDEX__) >= 1) ST25_DISCOVERY_JOYSTICK_GPIO_CLK_ENABLE(); \
                                                           }while(0)
#define ST25_DISCOVERY_BUTTONx_GPIO_CLK_DISABLE(__INDEX__) do{if((__INDEX__) == 0) ST25_DISCOVERY_KEY_BUTTON_GPIO_CLK_DISABLE(); \
                                                            if((__INDEX__) >= 1) (ST25_DISCOVERY_JOYSTICK_GPIO_CLK_DISABLE); \
                                                           }while(0)


/* Exported constants --------------------------------------------------------*/
#define ST25_DISCOVERY_TS_I2C_ADDRESS                             0x82


/*############################### I2Cx #######################################*/
/* User can use this section to tailor I2C1 instance used and associated 
   resources */
/*********************** TSCR I2C ***********************/
#if defined(HAL_I2C_MODULE_ENABLED)
#define ST25_DISCOVERY_I2C1                                       I2C1
#define ST25_DISCOVERY_I2C1_CLOCK_ENABLE()                        __HAL_RCC_I2C1_CLK_ENABLE()
#define ST25_DISCOVERY_I2C1_FORCE_RESET()                         __HAL_RCC_I2C1_FORCE_RESET()
#define ST25_DISCOVERY_I2C1_RELEASE_RESET()                       __HAL_RCC_I2C1_RELEASE_RESET()
#define ST25_DISCOVERY_I2C1_SDA_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define ST25_DISCOVERY_I2C1_SCL_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE() 
#define ST25_DISCOVERY_I2C1_SDA_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()

/* Definition for DISCO I2C1 Pins */      
#define ST25_DISCOVERY_I2C1_SCL_PIN                              GPIO_PIN_8
#define ST25_DISCOVERY_I2C1_SCL_GPIO_PORT                        GPIOB
#define ST25_DISCOVERY_I2C1_SCL_SDA_AF                           GPIO_AF4_I2C1
#define ST25_DISCOVERY_I2C1_SDA_PIN                              GPIO_PIN_9
#define ST25_DISCOVERY_I2C1_SDA_GPIO_PORT                        GPIOB

#define ST25_DISCOVERY_I2C1_SPEED                                0xA0711C23 /* 100KHz */

/* Definition for TS I2C's NVIC */      
#define ST25_DISCOVERY_I2C1_EV_IRQn                               I2C1_EV_IRQn
#define ST25_DISCOVERY_I2C1_ER_IRQn                               I2C1_ER_IRQn

/*********************** NFCTAG I2C ***********************/
#define ST25_DISCOVERY_I2C2                                       I2C2
#define ST25_DISCOVERY_I2C2_CLOCK_ENABLE()                        __HAL_RCC_I2C2_CLK_ENABLE()
#define ST25_DISCOVERY_I2C2_FORCE_RESET()                         __HAL_RCC_I2C2_FORCE_RESET()
#define ST25_DISCOVERY_I2C2_RELEASE_RESET()                       __HAL_RCC_I2C2_RELEASE_RESET()
#define ST25_DISCOVERY_I2C2_SDA_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define ST25_DISCOVERY_I2C2_SCL_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE() 
#define ST25_DISCOVERY_I2C2_SDA_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()
#define ST25_DISCOVERY_I2C2_SCL_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()
        
/* Definition for DISCO I2C2 Pins */        
#define ST25_DISCOVERY_I2C2_SCL_PIN                               GPIO_PIN_10
#define ST25_DISCOVERY_I2C2_SCL_GPIO_PORT                         GPIOB
#define ST25_DISCOVERY_I2C2_SDA_PIN                               GPIO_PIN_11
#define ST25_DISCOVERY_I2C2_SDA_GPIO_PORT                         GPIOB
#define ST25_DISCOVERY_I2C2_SCL_AF                                GPIO_AF4_I2C2
#define ST25_DISCOVERY_I2C2_SDA_AF                                GPIO_AF4_I2C2
        
#define ST25_DISCOVERY_I2C2_TIMINGS                               0x10B5163A /* 400KHz */
#define ST25_DISCOVERY_I2C2_SPEED                                 400000 /* 400KHz */

#if defined(USE_ST25DV)  
#define NFCMEM_I2C_TIMEOUT                                        ST25DV_I2C_TIMEOUT

#endif

/*********************** DPOT I2C ***********************/
#define ST25_DISCOVERY_I2C3                                       I2C3
#define ST25_DISCOVERY_I2C3_CLOCK_ENABLE()                        __HAL_RCC_I2C3_CLK_ENABLE()
#define ST25_DISCOVERY_I2C3_FORCE_RESET()                         __HAL_RCC_I2C3_FORCE_RESET()
#define ST25_DISCOVERY_I2C3_RELEASE_RESET()                       __HAL_RCC_I2C3_RELEASE_RESET()
#define ST25_DISCOVERY_I2C3_SDA_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOC_CLK_ENABLE()
#define ST25_DISCOVERY_I2C3_SCL_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOC_CLK_ENABLE() 
#define ST25_DISCOVERY_I2C3_SDA_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOC_CLK_DISABLE()
#define ST25_DISCOVERY_I2C3_SCL_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOC_CLK_DISABLE()

/* Definition for DPOT DISCO I2Cx Pins */
#define ST25_DISCOVERY_I2C3_SCL_PIN                               GPIO_PIN_0
#define ST25_DISCOVERY_I2C3_SCL_GPIO_PORT                         GPIOC
#define ST25_DISCOVERY_I2C3_SCL_SDA_AF                            GPIO_AF4_I2C3
#define ST25_DISCOVERY_I2C3_SDA_PIN                               GPIO_PIN_1
#define ST25_DISCOVERY_I2C3_SDA_GPIO_PORT                         GPIOC

/* Definition for DPOT I2C's NVIC */
#define ST25_DISCOVERY_I2C3_EV_IRQn                               I2C3_EV_IRQn
#define ST25_DISCOVERY_I2C3_ER_IRQn                               I2C3_ER_IRQn

#define ST25_DISCOVERY_DPOT_I2C_ADDR                              AD5112_ADDR1

/* Definition for DPOT I2C speed */
#define ST25_DISCOVERY_I2C3_SPEED                                 0xA0711C23 /* 100KHz */

/*<! The value of the timeout for I2C waiting loops */
#define ST25_DISCOVERY_I2Cx_TIMEOUT                               0x64

#endif /* HAL_I2C_MODULE_ENABLED */
/*############################### SPIx #######################################*/
#if defined(HAL_SPI_MODULE_ENABLED)
#define ST25_DISCOVERY_SPI2                                       SPI2
#define ST25_DISCOVERY_SPI2_CLK_ENABLE()                          __HAL_RCC_SPI2_CLK_ENABLE()
#define ST25_DISCOVERY_SPI2_CLK_DISABLE()                         __HAL_RCC_SPI2_CLK_DISABLE()
#define ST25_DISCOVERY_SPI2_MISO_MOSI_GPIO_PORT                   GPIOC
#define ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_PORT                     GPIOB
#define ST25_DISCOVERY_SPI2_AF                                    GPIO_AF5_SPI2
#define ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()
#define ST25_DISCOVERY_SPI2_MISO_MOSI_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define ST25_DISCOVERY_SPI2_MISO_MOSI_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()
#define ST25_DISCOVERY_SPI2_SCK_PIN                               GPIO_PIN_13
#define ST25_DISCOVERY_SPI2_NSS_PIN                               GPIO_PIN_12
#define ST25_DISCOVERY_SPI2_MISO_PIN                              GPIO_PIN_2
#define ST25_DISCOVERY_SPI2_MOSI_PIN                              GPIO_PIN_3

#define ST25_DISCOVERY_SPI3                                       SPI3
#define ST25_DISCOVERY_SPI3_CLK_ENABLE()                          __HAL_RCC_SPI3_CLK_ENABLE()
#define ST25_DISCOVERY_SPI3_CLK_DISABLE()                         __HAL_RCC_SPI3_CLK_DISABLE()
#define ST25_DISCOVERY_SPI3_GPIO_PORT                             GPIOC
#define ST25_DISCOVERY_SPI3_AF                                    GPIO_AF6_SPI3
#define ST25_DISCOVERY_SPI3_GPIO_CLK_ENABLE()                     __HAL_RCC_GPIOC_CLK_ENABLE()
#define ST25_DISCOVERY_SPI3_GPIO_CLK_DISABLE()                    __HAL_RCC_GPIOC_CLK_DISABLE()
#define ST25_DISCOVERY_SPI3_SCK_PIN                               GPIO_PIN_10
#define ST25_DISCOVERY_SPI3_MISO_PIN                              GPIO_PIN_11
#define ST25_DISCOVERY_SPI3_MOSI_PIN                              GPIO_PIN_12

#define ST25_DISCOVERY_SPI3_NSS_PIN                               GPIO_PIN_15
#define ST25_DISCOVERY_SPI3_NSS_PORT                              GPIOA
#define ST25_DISCOVERY_SPI3_NSS_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOA_CLK_ENABLE()
#define ST25_DISCOVERY_SPI3_NSS_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOA_CLK_DISABLE()

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define ST25_DISCOVERY_SPIx_TIMEOUT_MAX                           ((uint32_t)0x1000)

#endif /* HAL_SPI_MODULE_ENABLED */

/*################################ ADC1 ######################################*/
#if defined(HAL_OPAMP_MODULE_ENABLED)
#define ST25_DISCOVERY_EH_OPAMP                                   OPAMP1
#define ST25_DISCOVERY_EH_OPAMP_CLK_ENABLE()                      __HAL_RCC_OPAMP_CLK_ENABLE()
#define ST25_DISCOVERY_EH_OPAMP_CLK_DISABLE()                     __HAL_RCC_OPAMP_CLK_DISABLE()

#endif /* HAL_OPAMP_MODULE_ENABLED */

#if defined(HAL_ADC_MODULE_ENABLED)
/**
* @brief  ADC Interface pins
*         used to detect motion of Joystick available on Adafruit 1.8" TFT shield
*/
typedef enum 
{
  ADC1_CHANNEL_5 = 0,
  ADC1_CHANNEL_11 = 1,
  ADC1_CHANNEL_14 = 2
} ST25_DISCOVERY_ADC1_CHANNEL_TypeDef;

#define ST25_DISCOVERY_EH_ADC                                     ADC1
#define ST25_DISCOVERY_EH_ADC_CLK_ENABLE()                        __HAL_RCC_ADC_CLK_ENABLE()
#define ST25_DISCOVERY_EH_ADC_CLK_DISABLE()                       __HAL_RCC_ADC_CLK_DISABLE()
#define ST25_DISCOVERY_EH_MODE                                    GPIO_MODE_ANALOG_ADC_CONTROL

#define ST25_DISCOVERY_EH_CHANNELn                                1
#define ST25_DISCOVERY_EH_CHANNEL_LIST                            {ADC_CHANNEL_5}

#define ST25_DISCOVERY_EH_1ST_CHANNEL_PIN                         GPIO_PIN_0
#define ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_PORT                   GPIOA
#define ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

#define ST25_DISCOVERY_EH_2ND_CHANNEL_PIN                         GPIO_PIN_6
#define ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_PORT                   GPIOA
#define ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

#define ST25_DISCOVERY_EH_3RD_CHANNEL_PIN                         GPIO_PIN_5
#define ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_PORT                   GPIOC
#define ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()

#endif /* HAL_ADC_MODULE_ENABLED */

#define ST25_DISCOVERY_EH_CHANNEL_GPIO_CLK_ENABLE(__INDEX__)      do{if((__INDEX__) == 0) (ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_CLK_ENABLE()); else \
                                                                     if((__INDEX__) == 1) (ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_CLK_ENABLE()); \
                                                                     if((__INDEX__) == 2) (ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_CLK_ENABLE()); \
                                                                  }while(0)
#define ST25_DISCOVERY_EH_CHANNEL_GPIO_CLK_DISABLE(__INDEX__)     do{if((__INDEX__) == 0) (ST25_DISCOVERY_EH_1ST_CHANNEL_GPIO_CLK_DISABLE()); else \
                                                                     if((__INDEX__) == 1) (ST25_DISCOVERY_EH_2ND_CHANNEL_GPIO_CLK_DISABLE()); \
                                                                     if((__INDEX__) == 2) (ST25_DISCOVERY_EH_3RD_CHANNEL_GPIO_CLK_DISABLE()); \
                                                                  }while(0)
/*################################ IOE #######################################*/
/** 
  * @brief  IOE Control pin  
  */ 
/* Definition for external IT for STMPE811 */
#define ST25_DISCOVERY_STMPE811_INT_PIN                           GPIO_PIN_0
#define ST25_DISCOVERY_STMPE811_INT_GPIO_PORT                     GPIOE
#define ST25_DISCOVERY_STMPE811_INT_CLK_ENABLE()                  __HAL_RCC_GPIOE_CLK_ENABLE()
#define ST25_DISCOVERY_STMPE811_INT_CLK_DISABLE()                 __HAL_RCC_GPIOE_CLK_DISABLE()
#define ST25_DISCOVERY_STMPE811_INT_EXTI                          EXTI0_IRQn
#define ST25_DISCOVERY_STMPE811_INT_EXTIHandler                   EXTI0_IRQHandler

/*################################ LCD #######################################*/
#if defined(HAL_SPI_MODULE_ENABLED) && defined(USE_LCD)
/** 
* @brief  LCD Command/data pin
*/
#define ST25_DISCOVERY_LCD_BKLIGHT_PIN                            GPIO_PIN_15
#define ST25_DISCOVERY_LCD_BKLIGHT_GPIO_PORT                      GPIOB
#define ST25_DISCOVERY_LCD_BKLIGHT_CLK_ENABLE()                   do{__HAL_RCC_TIM15_CLK_ENABLE(); \
                                                                     __HAL_RCC_GPIOB_CLK_ENABLE(); \
                                                                  }while(0)
#define ST25_DISCOVERY_LCD_BKLIGHT_CLK_DISABLE()                  do{__HAL_RCC_TIM15_CLK_DISABLE(); \
                                                                     __HAL_RCC_GPIOB_CLK_DISABLE(); \
                                                                  }while(0)

/** 
* @brief  LCD Backlight setting
*/
#define LCD_BKLIGHT_BRIGHT  100
#define LCD_BKLIGHT_DIM     2                                                                  

/** 
* @brief  LCD Command/data pin
*/
#define ST25_DISCOVERY_LCD_WRX_PIN                                GPIO_PIN_3
#define ST25_DISCOVERY_LCD_WRX_GPIO_PORT                          GPIOE
#define ST25_DISCOVERY_LCD_WRX_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOE_CLK_ENABLE()
#define ST25_DISCOVERY_LCD_WRX_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOE_CLK_DISABLE()

/* Set WRX High to send data */
#define ST25_DISCOVERY_LCD_WRX_LOW()                              HAL_GPIO_WritePin(ST25_DISCOVERY_LCD_WRX_GPIO_PORT, ST25_DISCOVERY_LCD_WRX_PIN, GPIO_PIN_RESET)
#define ST25_DISCOVERY_LCD_WRX_HIGH()                             HAL_GPIO_WritePin(ST25_DISCOVERY_LCD_WRX_GPIO_PORT, ST25_DISCOVERY_LCD_WRX_PIN, GPIO_PIN_SET)

/* Use HW control for CS, defines empty SW support */
#define ST25_DISCOVERY_LCD_CS_LOW()                               HAL_GPIO_WritePin(ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_PORT, ST25_DISCOVERY_SPI2_NSS_PIN, GPIO_PIN_RESET)         
#define ST25_DISCOVERY_LCD_CS_HIGH()                              HAL_GPIO_WritePin(ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_PORT, ST25_DISCOVERY_SPI2_NSS_PIN, GPIO_PIN_SET) 
#define ST25_DISCOVERY_LCD_NCS_GPIO_CLK_ENABLE()                  ST25_DISCOVERY_SPI2_SCK_NSS_GPIO_CLK_ENABLE()

#endif /* HAL_SPI_MODULE_ENABLED && USE_LCD */

/*################################ GPO NFCMEM #######################################*/
/** 
  * @brief  GPO Input pin
  */
#if defined(USE_NFCTAG)
#define NFCMEM_GPO_PIN                                            GPIO_PIN_15
#define NFCMEM_GPO_PIN_PORT                                       GPIOE
#define NFCMEM_GPO_CLK_ENABLE()                                   __HAL_RCC_GPIOE_CLK_ENABLE()
#define NFCMEM_GPO_CLK_DISABLE()                                  __HAL_RCC_GPIOE_CLK_DISABLE()
#define NFCMEM_GPO_EXTI                                           EXTI15_10_IRQn
#define NFCMEM_GPO_EXTIHandler                                    EXTI15_10_IRQHandler

/*################################ SPARE NFCMEM #######################################*/
/** 
  * @brief  SPARE Control pin
  */
#define NFCMEM_SPARE_PIN                                          GPIO_PIN_7
#define NFCMEM_SPARE_PIN_PORT                                     GPIOE
#define NFCMEM_SPARE_CLK_ENABLE()                                 __HAL_RCC_GPIOE_CLK_ENABLE()
#define NFCMEM_SPARE_CLK_DISABLE()                                __HAL_RCC_GPIOE_CLK_DISABLE()
#define NFCMEM_SPARE_EXTI                                         EXTI9_5_IRQn
#define NFCMEM_SPARE_EXTIHandler                                  EXTI9_5_IRQHandler
                
#if defined(USE_ST25DV)
/**
  * @brief  ST25DV LPD wrapper
  */
#define NFCMEM_LPD_PIN                                            NFCMEM_SPARE_PIN
#define NFCMEM_LPD_PIN_PORT                                       NFCMEM_SPARE_PIN_PORT
#define NFCMEM_LPD_CLK_ENABLE()                                   NFCMEM_SPARE_CLK_ENABLE()
#define NFCMEM_LPD_CLK_DISABLE()                                  NFCMEM_SPARE_CLK_DISABLE()
#define NFCMEM_LPD_EXTI                                           NFCMEM_SPARE_EXTI
#define NFCMEM_LPD_EXTIHandler                                    NFCMEM_SPARE_EXTIHandler

#endif /* USE_ST25DV */

/*########################## MEMORY NFCMEM WRAPPER #################################*/
#if defined(USE_ST25DV)

#endif /* USE_ST25DV */

#endif /* USE_NFCTAG */

uint32_t      BSP_GetVersion(void);  
void          BSP_LED_Init(Led_TypeDef Led);
void          BSP_LED_DeInit(Led_TypeDef Led);
void          BSP_LED_On(Led_TypeDef Led);
void          BSP_LED_Off(Led_TypeDef Led);
void          BSP_LED_Toggle(Led_TypeDef Led);
void          BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
uint32_t      BSP_PB_GetState(Button_TypeDef Button);
#if defined(USE_NFCTAG) && defined(USE_ST25DV)
void          BSP_GPO_Init( uint32_t it_mode );
void          BSP_LPD_Init( void );
GPIO_PinState BSP_LPD_ReadPin( void );
void          BSP_LPD_WritePin( GPIO_PinState LpdPinState );

int32_t        NFCMEM_IO_Init( void );
int32_t        NFCMEM_IO_DeInit( void );

int32_t        NFCMEM_IO_MemWrite( uint16_t DevAddr, uint16_t TarAddr, const uint8_t * const pData, uint16_t Size );
int32_t        NFCMEM_IO_MemRead( uint16_t DevAddr, uint16_t TarAddr, uint8_t * const pData,  uint16_t Size); 
int32_t        NFCMEM_IO_Read( uint8_t * const pData, const uint8_t DevAddr, const uint16_t Size );
uint8_t        NFCMEM_IO_IsNacked( void );
int32_t        NFCMEM_IO_IsDeviceReady( uint16_t DevAddress,  uint32_t Trials );
int32_t        NFCMEM_IO_Tick(void);

#endif /* USE_NFCTAG && USE_ST25DV */

#if defined(HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV)
uint8_t       BSP_ST25DV_EH_Init( uint32_t *vdda);
void          BSP_ST25DV_EH_Measure(uint16_t *values);
void          BSP_ST25DV_EH_DeInit(void);

#endif /* HAL_ADC_MODULE_ENABLED) && defined(USE_ST25DV */

#if defined(HAL_SPI_MODULE_ENABLED) && defined(USE_LCD)
void          BSP_LCD_LowFreq(void);
void          BSP_LCD_HighFreq(void);

#endif /* HAL_SPI_MODULE_ENABLED && USE_LCD */

#ifdef HAL_I2C_MODULE_ENABLED
void          BSP_I2C2_ReInit( void );

#endif /* HAL_I2C_MODULE_ENABLED */

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

#endif /* ST25_DISCOVERY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
