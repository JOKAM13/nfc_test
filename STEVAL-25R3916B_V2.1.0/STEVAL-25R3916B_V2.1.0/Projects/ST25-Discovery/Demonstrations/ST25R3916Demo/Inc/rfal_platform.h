/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2018 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
******************************************************************************/
/*! \file
 *
 *  \author
 *
 *  \brief Platform header file. Defining platform independent functionality.
 *
 */

#ifndef RFAL_PLATFORM_H
#define RFAL_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#ifdef STM32L476xx
#include "stm32l4xx_hal.h"
#else
#include "stm32f4xx_hal.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "main.h"
#ifdef RFAL_USE_I2C
#include "i2c.h"
#else
#include "spi.h"
#endif
#include "timer.h"
#include "logger.h"


/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/
#define ST25R_SS_PIN             SPI1_CS_Pin              /*!< GPIO pin used for ST25R SPI SS                */
#define ST25R_SS_PORT            SPI1_CS_GPIO_Port        /*!< GPIO port used for ST25R SPI SS port          */

#define ST25R_INT_PIN            ST25DX_INT_1_Pin            /*!< GPIO pin used for ST25R External Interrupt    */
#define ST25R_INT_PORT           ST25DX_INT_1_GPIO_Port      /*!< GPIO port used for ST25R External Interrupt   */

#ifdef LED_FIELD_Pin
#define PLATFORM_LED_FIELD_PIN      LED_FIELD_Pin         /*!< GPIO pin used as field LED                    */
#endif

#ifdef LED_FIELD_GPIO_Port
#define PLATFORM_LED_FIELD_PORT     LED_FIELD_GPIO_Port   /*!< GPIO port used as field LED                   */
#endif


#define PLATFORM_LED_A_PIN           LED_A_Pin             /*!< GPIO pin used for LED A    */
#define PLATFORM_LED_A_PORT          LED_A_GPIO_Port       /*!< GPIO port used for LED A   */
#define PLATFORM_LED_B_PIN           LED_B_Pin             /*!< GPIO pin used for LED B    */
#define PLATFORM_LED_B_PORT          LED_B_GPIO_Port       /*!< GPIO port used for LED B   */
#define PLATFORM_LED_F_PIN           LED_F_Pin             /*!< GPIO pin used for LED F    */
#define PLATFORM_LED_F_PORT          LED_F_GPIO_Port       /*!< GPIO port used for LED F   */
#define PLATFORM_LED_V_PIN           LED_V_Pin             /*!< GPIO pin used for LED V    */
#define PLATFORM_LED_V_PORT          LED_V_GPIO_Port       /*!< GPIO port used for LED V   */
#define PLATFORM_LED_AP2P_PIN        LED_AP2P_Pin          /*!< GPIO pin used for LED AP2P */
#define PLATFORM_LED_AP2P_PORT       LED_AP2P_GPIO_Port    /*!< GPIO port used for LED AP2P*/

#define PLATFORM_USER_BUTTON_PIN     B1_Pin                /*!< GPIO pin user button       */
#define PLATFORM_USER_BUTTON_PORT    B1_GPIO_Port          /*!< GPIO port user button      */


/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/
#define platformProtectST25RComm()                    do{ globalCommProtectCnt++; __DSB();NVIC_DisableIRQ(EXTI15_10_IRQn);__DSB();__ISB();}while(0) /*!< Protect unique access to ST25R communication channel - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment      */
#define platformUnprotectST25RComm()                  do{ if (--globalCommProtectCnt==0) {NVIC_EnableIRQ(EXTI15_10_IRQn);} }while(0)                /*!< Unprotect unique access to ST25R communication channel - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment */

#define platformProtectST25RIrqStatus()               platformProtectST25RComm()                    /*!< Protect unique access to IRQ status var - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment */
#define platformUnprotectST25RIrqStatus()             platformUnprotectST25RComm()                  /*!< Unprotect the IRQ status var - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment         */

#define platformLedOff( port, pin )                   platformGpioClear(port, pin)                  /*!< Turns the given LED Off                     */
#define platformLedOn( port, pin )                    platformGpioSet(port, pin)                    /*!< Turns the given LED On                      */
#define platformLedToggle( port, pin )                platformGpioToggle(port, pin)                 /*!< Toggles the given LED                       */

#define platformGpioSet( port, pin )                  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)    /*!< Turns the given GPIO High                   */
#define platformGpioClear( port, pin )                HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)  /*!< Turns the given GPIO Low                    */
#define platformGpioToggle( port, pin )               HAL_GPIO_TogglePin(port, pin)                 /*!< Toggles the given GPIO                      */
#define platformGpioIsHigh( port, pin )               (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) /*!< Checks if the given LED is High             */
#define platformGpioIsLow( port, pin )                (!platformGpioIsHigh(port, pin))              /*!< Checks if the given LED is Low              */

#define platformTimerCreate( t )                      timerCalculateTimer(t)                        /*!< Create a timer with the given time (ms)     */
#define platformTimerIsExpired( timer )               timerIsExpired(timer)                         /*!< Checks if the given timer is expired        */
#define platformDelay( t )                            HAL_Delay(t)                                  /*!< Performs a delay for the given time (ms)    */

#define platformGetSysTick()                          HAL_GetTick()                                 /*!< Get System Tick ( 1 tick = 1 ms)            */

#define platformAssert( exp )                         assert_param( exp )                           /*!< Asserts whether the given expression is true*/
#define platformErrorHandle()                         _Error_Handler(__FILE__, __LINE__)            /*!< Global error handle\trap                    */


#ifdef RFAL_USE_I2C

#define platformI2CTx( txBuf, len, last, txOnly )     i2cSequentialTx((uint16_t)0xA0, (uint8_t *)(txBuf), (len), last, txOnly ) /*!< I2C Transmit                                */
#define platformI2CRx( txBuf, len )                   i2cSequentialRx((uint16_t)0xA0, rxBuf, len )  /*!< I2C Receive                                 */
#define platformI2CStart()                                                                          /*!< I2C Start condition                         */
#define platformI2CStop()                                                                           /*!< I2C Stop condition                          */
#define platformI2CRepeatStart()                                                                    /*!< I2C Repeat Start                            */
#define platformI2CSlaveAddrWR(add)                                                                 /*!< I2C Slave address for Write operation       */
#define platformI2CSlaveAddrRD(add)                                                                 /*!< I2C Slave address for Read operation        */

#else /* RFAL_USE_I2C */

#define platformSpiSelect()                           platformGpioClear(ST25R_SS_PORT, ST25R_SS_PIN)/*!< SPI SS\CS: Chip|Slave Select                */
#define platformSpiDeselect()                         platformGpioSet(ST25R_SS_PORT, ST25R_SS_PIN)  /*!< SPI SS\CS: Chip|Slave Deselect              */
#define platformSpiTxRx( txBuf, rxBuf, len )          spiTxRx(txBuf, rxBuf, len)                    /*!< SPI transceive                              */

#endif /* RFAL_USE_I2C */

#define platformLog(...)                              logUsart(__VA_ARGS__)                         /*!< Log  method                                 */

/*
******************************************************************************
* GLOBAL VARIABLES
******************************************************************************
*/
extern uint8_t globalCommProtectCnt;                      /* Global Protection Counter provided per platform - instantiated in main.c    */

/*
******************************************************************************
* RFAL FEATURES CONFIGURATION
******************************************************************************
*/

#define RFAL_FEATURE_LISTEN_MODE               true       /*!< Enable/Disable RFAL support for Listen Mode                               */
#define RFAL_FEATURE_WAKEUP_MODE               true       /*!< Enable/Disable RFAL support for the Wake-Up mode                          */
#define RFAL_FEATURE_LOWPOWER_MODE             false      /*!< Enable/Disable RFAL support for the Low Power mode                        */
#define RFAL_FEATURE_NFCA                      true       /*!< Enable/Disable RFAL support for NFC-A (ISO14443A)                         */
#define RFAL_FEATURE_NFCB                      true       /*!< Enable/Disable RFAL support for NFC-B (ISO14443B)                         */
#define RFAL_FEATURE_NFCF                      true       /*!< Enable/Disable RFAL support for NFC-F (FeliCa)                            */
#define RFAL_FEATURE_NFCV                      true       /*!< Enable/Disable RFAL support for NFC-V (ISO15693)                          */
#define RFAL_FEATURE_T1T                       true       /*!< Enable/Disable RFAL support for T1T (Topaz)                               */
#define RFAL_FEATURE_T2T                       true       /*!< Enable/Disable RFAL support for T2T                                       */
#define RFAL_FEATURE_T4T                       true       /*!< Enable/Disable RFAL support for T4T                                       */
#define RFAL_FEATURE_ST25TB                    true       /*!< Enable/Disable RFAL support for ST25TB                                    */
#define RFAL_FEATURE_ST25xV                    true       /*!< Enable/Disable RFAL support for ST25TV/ST25DV                             */
#define RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG     false      /*!< Enable/Disable Analog Configs to be dynamically updated (RAM)             */
#define RFAL_FEATURE_DPO                       true      /*!< Enable/Disable RFAL Dynamic Power Output support                          */
#define RFAL_FEATURE_ISO_DEP                   true       /*!< Enable/Disable RFAL support for ISO-DEP (ISO14443-4)                      */
#define RFAL_FEATURE_ISO_DEP_POLL              true       /*!< Enable/Disable RFAL support for Poller mode (PCD) ISO-DEP (ISO14443-4)    */
#define RFAL_FEATURE_ISO_DEP_LISTEN            true       /*!< Enable/Disable RFAL support for Listen mode (PICC) ISO-DEP (ISO14443-4)   */
#define RFAL_FEATURE_NFC_DEP                   true       /*!< Enable/Disable RFAL support for NFC-DEP (NFCIP1/P2P)                      */

#define RFAL_FEATURE_ISO_DEP_IBLOCK_MAX_LEN    256U       /*!< ISO-DEP I-Block max length. Please use values as defined by rfalIsoDepFSx */
#define RFAL_FEATURE_NFC_DEP_BLOCK_MAX_LEN     254U       /*!< NFC-DEP Block/Payload length. Allowed values: 64, 128, 192, 254           */
#define RFAL_FEATURE_NFC_RF_BUF_LEN            258U       /*!< RF buffer length used by RFAL NFC layer                                   */

#define RFAL_FEATURE_ISO_DEP_APDU_MAX_LEN      512U       /*!< ISO-DEP APDU max length. Please use multiples of I-Block max length       */
#define RFAL_FEATURE_NFC_DEP_PDU_MAX_LEN       512U       /*!< NFC-DEP PDU max length.                                                   */

/*
******************************************************************************
* RFAL CUSTOM SETTINGS
******************************************************************************
  Custom analog configs are used to cope with Automatic Antenna Tuning (AAT)
  that are optimized differently for each board.
*/
//#define RFAL_ANALOG_CONFIG_CUSTOM                         /*!< Use Custom Analog Configs when defined                                    */

#ifndef platformProtectST25RIrqStatus
    #define platformProtectST25RIrqStatus()            /*!< Protect unique access to IRQ status var - IRQ disable on single thread environment (MCU) ; Mutex lock on a multi thread environment */
#endif /* platformProtectST25RIrqStatus */

#ifndef platformUnprotectST25RIrqStatus
    #define platformUnprotectST25RIrqStatus()          /*!< Unprotect the IRQ status var - IRQ enable on a single thread environment (MCU) ; Mutex unlock on a multi thread environment         */
#endif /* platformUnprotectST25RIrqStatus */

#ifndef platformProtectWorker
    #define platformProtectWorker()                    /* Protect RFAL Worker/Task/Process from concurrent execution on multi thread platforms   */
#endif /* platformProtectWorker */

#ifndef platformUnprotectWorker
    #define platformUnprotectWorker()                  /* Unprotect RFAL Worker/Task/Process from concurrent execution on multi thread platforms */
#endif /* platformUnprotectWorker */

#ifndef platformIrqST25RPinInitialize
    #define platformIrqST25RPinInitialize()            /*!< Initializes ST25R IRQ pin                     */
#endif /* platformIrqST25RPinInitialize */

#ifndef platformIrqST25RSetCallback
    #define platformIrqST25RSetCallback( cb )          /*!< Sets ST25R ISR callback                       */
#endif /* platformIrqST25RSetCallback */

#ifndef platformLedsInitialize
    #define platformLedsInitialize()                   /*!< Initializes the pins used as LEDs to outputs  */
#endif /* platformLedsInitialize */

#ifndef platformLedOff
    #define platformLedOff( port, pin )                /*!< Turns the given LED Off                       */
#endif /* platformLedOff */

#ifndef platformLedOn
    #define platformLedOn( port, pin )                 /*!< Turns the given LED On                        */
#endif /* platformLedOn */

#ifndef platformLedToggle
    #define platformLedToggle( port, pin )             /*!< Toggles the given LED                         */
#endif /* platformLedToggle */

#ifndef platformGetSysTick
    #define platformGetSysTick()                       /*!< Get System Tick ( 1 tick = 1 ms)              */
#endif /* platformGetSysTick */

#ifndef platformTimerDestroy
    #define platformTimerDestroy( timer )              /*!< Stops and released the given timer            */
#endif /* platformTimerDestroy */

#ifndef platformLog
    #define platformLog(...)                           /*!< Log method                                    */
#endif /* platformLog */

#ifndef platformAssert
    #define platformAssert( exp )                      /*!< Asserts whether the given expression is true */
#endif /* platformAssert */

#ifndef platformErrorHandle
    #define platformErrorHandle()                      /*!< Global error handler or trap                 */
#endif /* platformErrorHandle */

#ifdef __cplusplus
}
#endif

#endif /* RFAL_PLATFORM_H */
