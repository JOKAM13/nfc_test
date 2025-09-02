/**
  ******************************************************************************
  * @file    main.c
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @ingroup ST25R3916_Discovery_Demo
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "Menu_config.h"
#include "Menu_definition.h"
#include "st25_discovery_lcd.h"
#include "st25_discovery_ts.h"
#include "st25_discovery_dpot.h"
#include "st_logo_jpg.h"
#include "../../../Utilities/Fonts/font22.c"
#include "../../../Utilities/Fonts/font16.c"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include "rfal_platform.h"
#include "logger.h"
#include "spi.h"
#include "led.h"

#include "bootloader.h"
#include "usbd_custom_hid_if.h"
#include "ce.h"

#include "stream_dispatcher.h"
#include "dispatcher.h"
#include "rfal_analogConfig.h"
#include "rfal_rf.h"
#include "rfal_nfc.h"
#include "rfal_dpo.h"
#include "rfal_chip.h"
#ifdef ST25R3916B
#include "pac.h"
#endif

/** @mainpage
  * ST25R3916-Discovery firmware documentation
  * ==================================================================================
  * This document describes the ST25 Discovery Firmware for the ST25R3916 demo.
  * @page Introduction
  * @section Firmware_objectives Firmware objectives
  * The purpose of this firmware is to run demos on the ST25R3916 reader.
  * @subsection ST25R3916_Reader The ST25R3916 NFC Reader
  * The ST25R3916 is a NFC reader, thus supporting 3 different modes:
  * - Reader / Writer for all NFC Forum Tag Types.
  * - Card Emulation (this demo only demonstrates Type4A in stand-aone mode).
  * - Peer 2 peer
  *
  * This reader provides a SPI connection to a MCU.
  * @section About_the_hardware About the hardware
  * This Firmware runs on the ST25R3916-Discovery Kit which includes 2 different boards:
  * - The ST25 Discovery Motherboard: embedding a STM32L476 MCU, on which this firmware is intended to run.
  * - The ST25R3916EVAL_ANT boards: having the ST253916 reader and a NFC Antenna.
  * @subsection ST25_Discovery_board ST25 Discovery board
  * The ST25 Discovery board is built around a STM32L476 MCU (running this firmware).
  *
  * The board embeds also:
  * - a LCD display with a touchscreen, to display and control the demo.
  * - 2 USB connectors, the miniUSB for the ST-Link, the microUSB is used to control the discovery Kit with the PC GUI.
  * - a connector for a Antenna daughter board (such as one of the ST25R3916EVAL Antenna boards)
  * - And as optional modules:
  *   - a ST Bluetooth Low Energy module.
  *   - a ST Wifi module.
  * @subsection ST25R3916_Ant_board ST25R3916EVAL Antenna boards
  * These boards are built around the ST25R3916 NFC Reader and different NFC antennas.
  * @section Demo_overview Demo overview
  * The firmware implements 5 demo categories:
  * - The Reader/Writer Mode demos:
  *   - Detect and display tags.
  *   - When selected by the user, display tag information.
  *   - Reads & display ndef tag content.
  *   - write URI NDEF message to the tag
  * - The Card Emulation demo:
  *   - The ST25R3916 acts as a NFC Forum Type4 tag (content may be a URI or a vCard)
  *   - Data can be read from the emulated tag (using a smartphone or any reader such as the ST25R3916)
  *   - Data can be written, and then displayed on the LCD
  * - The peer to peer demo:
  *   - The demo waits a peer to be detected and sends either a URI or a vCard.
  *   - When data is received it may be displayed on LCD
  * - The Antenna Tuning demo
  *   - The demo displays the current antenna setting and measurement as a needle (for phase & amplitude)
  *   - when touching the screen, the antenna is tuned to increase amplitude
  * - The USB mode, used to control the discovery kit with the PC GUI
  *   - Connect the microUSB to a PC, starts the GUI and reset the discovery board while pushing the user button
  *   - In this mode the GUI takes full control over the discovery kit for many advanced demos
  * @section Cube_Methodology STM32Cube methodology
  * This firmware has been designed to specifically runs on the STM32L476 MCU embedded on the ST25 Discovery board.
  * However, thanks to the STM32Cube methodology, the structure of the firmware enables both an easy full porting to an other STM32 MCU and an easy reuse of parts of the demo.
  * Two important elements have been particularly designed to make their reuse as straightforward as possible:
  * - The RFAL library, which implements the functions to control the ST25R readers from its SPI or I2C interface,  is completely independant from the MCU.\n
  *   This driver can thus be easily reused in any other project based on the STM32Cube methodology, but can also be reused on any other HW solution interacting with a ST25R3916.
  * - The NDEF library, which implements the standard NDEF protocol, is provided as a Cube Middleware fully independant from the HW.
  *
  * @section Documentation_Structure Firmware documentation structure
  * This document has been generated from the Doxygen comments embedded in the firmware source code.\n
  * The structure of this document follows the firmware structure, and uses the standard doxygen implementation based on Modules.\n
  * @subsection Main_Modules Main Demo modules
  * - @ref Reader_Writter_Demo : Detect tags in the field, display tag informations such as bitrate, RSSI, NDEF content, etc...
  * - @ref Card_Emulation_Demo : Emulate a NFC Forum Type4A tag, with URL or vCard NDEF message, displays written message too.
  * - @ref Peer_2_Peer_Demo : Enable peer 2 peer communication, sends URL or vCard NDEF message, displays received NDEF message too.
  * - @ref Antenna_Tuning_Demo : Allow manual tuning of capacitances or runs the automatic antenna tuning
  * @subsection BoardSupportPackageDriver ST25DV Board Support Package and Driver
  * - @ref BSP : @copybrief BSP
  * - @ref ST25_DISCOVERY_READER : @copybrief ST25_DISCOVERY_READER
  * @subsection Menu_Of_The_Demo Menu of the demo
  * - @ref ST25R3916_Menu_Definition : @copybrief ST25R3916_Menu_Definition
  * - @ref Menu_Demo_Interface : @copybrief Menu_Demo_Interface
  * - @ref Jpeg_Decode : @copybrief Jpeg_Decode
  * @subsection MCU_Support_Modules MCU support modules
  * - @ref ST25Discovery_IT : @copybrief ST25Discovery_IT
  * - @ref ST25Discovery_MSP : @copybrief ST25Discovery_MSP
  *
  * @note Doxygen non-default settings to re-generate this document:
  * - OPTIMIZE_OUTPUT_FOR_C=YES
  * - INLINE_SIMPLE_STRUCT=YES
  * - MAX_INITIALIZER_LINES=1
  * - HIDE_UNDOC_MEMBERS=1
  *
  * @section Middlewares_used Middlewares used in this firmware
  *   This firmware relies on several Middlewares, either provided by ST or a third party.
  *   These Middlewares are HW-independant softwares implementing a generic feature.
  *   They will not be detailed in this documentation.\n
  *   Herebelow is the list of the Middlewares used by this firmware.
  *   @subsection LibNDEF LibNDEF
  *     This library provides functions to read & write NDEF messages to a tag.\n
  *     It supports a variety of NDEF records, such as:
  *     - URI record (includes URLs)
  *     - SMS record
  *     - Email record
  *     - vCard record
  *     - Geo location record
  *     - Bluetooth OOB record
  *     - Wifi OOB record
  *     - Android Application Record (AAR)
  *
  *     The library also defines a NFC-Forum Type5 Tag wrapper to comply with the NFC-Forum Type5 Tag specification.
  *   @subsection LibJPEG LibJPEG
  *     This library implements the JPEG codec. This firmware only includes the JPEG decoding part of the library.
  *   @subsection Menu_Demo Menu Demo
  *     This Middleware implements functions to display an icon-and-text-based menu.\n
  *     It also manages inputs from a touchscreen, a joystick and a button, to interact with the user.
  *
  * @section ST25Disco_BSP ST25Discovery Board Support Package
  *   The Board Support Package software (BSP) is defined by the Cube methodology as the abstraction layer for the board specific features.\n
  *   It implements all the functions required to access:
  *     - the components on the board.
  *     - the MCU peripherals requiring a board specific configuration.
  *
  *   The different parts of the BSP are described below.
  *   @subsection IOBus IOBus
  *     This part of the BSP implements the low level functions interfacing between the components drivers and the MCU peripherals (by caling the Cube HAL).\n
  *     The BSP IOBus functions are not detailed in this document.
  *   @subsection High_Level_API High level APIs
  *     This parts of the BSP provides high level functions called by the application or Middlewares to access the component drivers.\n
  *     This layer acts as a bridge between the application and the component drivers.\n
  *
  * @section ST25Disco_Components Components
  *   The ST25 Discovery kit embeds different components requiring specific softwares to be correctly driven.
  *   Unless it is specified differently, these drivers are not detailed in this document.
  *   @subsection ILI9341_DRIVER ILI9341 driver
  *     This driver implements functions to access the LCD display of the ST25-Discovery.
  *   @subsection STMPE811_DRIVER STMPE811 driver
  *     This driver implements functions to access the Touchscreen of the ST25-Discovery.
  */


/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;
/* Private function prototypes -----------------------------------------------*/
void MX_GPIO_Init( void );
void BoardCheck( void );

static void SystemClock_Config_st25r3916(void);
static void MX_SPI1_Init(void);
static void MX_UART1_Init(void);
void _Error_Handler(char * file, int line);

extern void exampleRfalPollerIni(void);

/* Public variables ---------------------------------------------------------*/
uint8_t globalCommProtectCnt;

static rfalDpoEntry dpoSetup[] = {
// new antenna board
#ifdef ST25R3916B
{.rfoRes=0, .inc=255, .dec=115},
{.rfoRes=9, .inc=100, .dec=0x00}
#else
{.rfoRes=0, .inc=255, .dec=115},
{.rfoRes=2, .inc=100, .dec=0x00}
#endif
};

void rfalPreTransceiveCb(void)
{
  rfalDpoAdjust();
}

void InitializeRFAL(void)
{
  /* RFAL initalisation */
  rfalAnalogConfigInitialize();

#ifdef ST25R3916B
  /* Enable persistent analog config */
  persistentAnalogConfig_Enable();
#endif

  if(rfalNfcInitialize() != ERR_NONE)
  {
#ifdef ST25R3916B
    Menu_MsgStatus("Init error!", "To fix this issue:\n1. Connect a ST25R3916B daughter board.\n2. Connect the USB-micro port.", MSG_STATUS_ERROR);
#else
    Menu_MsgStatus("Init error!", "To fix this issue:\n1. Connect a ST25R3916 daughter board.\n2. Connect the USB-micro port.", MSG_STATUS_ERROR);
#endif
    while(1);
  }
    
  /* DPO setup */
  rfalDpoInitialize();
  rfalDpoSetMeasureCallback( rfalChipMeasureAmplitude );
  rfalDpoTableWrite(dpoSetup,sizeof(dpoSetup)/sizeof(rfalDpoEntry));
  rfalDpoSetEnabled(true);
  rfalSetPreTxRxCallback(&rfalPreTransceiveCb);

}

/** @defgroup ST25_Discovery_Demo ST25 Discovery Demo
  * @brief    This module is the main module for all the ST25Discovery board demos.
  * @details  Demos are divided in 3 different sub-modules:
  *           - Fast transfer mode demos
  *           - ST25DV features demos
  *           - NDEF demos
  * @{
  */

/* Public functions -----------------------------------------------------------*/
/** @brief Demo entry point. */
int main( void )
{

  bootloaderCheck4Enter();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init( );

  /* Check the board. */
  BoardCheck( );

  /* Configure the system clock */
  SystemClock_Config_st25r3916( );

  HAL_Delay( 50 );

  BSP_TS_Init( 240, 320 );
  BSP_LCD_Init( );
  BSP_LCD_SetFont( &Font22 );

  /* Configure Buttons */
  for( uint8_t buttonconf = 0; buttonconf <= JOYSTICK_DOWN; buttonconf++ )
  {
    BSP_PB_Init( (Button_TypeDef)buttonconf, BUTTON_MODE_GPIO );
  }
  BSP_LED_Init( LED2 );
  BSP_LED_Init( LED3 );

	/* Configure the GPIOs */
#ifndef ST25R3916B  
  /* DPot removed from STEVAL-M25R3916B bom */
  BSP_DPOT_Init( );
  BSP_DPOT_SetResistor( 0xff );
#endif 
  
  MX_GPIO_Init();
  MX_SPI1_Init();
  spiInit(&hspi1);
  
  MX_UART1_Init();
  logUsartInit(&huart1);

  /* required for crypto */
  __CRC_CLK_ENABLE();

  InitializeRFAL();

  if(!Menu_GetUserInput())
  {
    SplashScreen( );
    Menu_Start( );
  }
  Menu_MsgStatus("USB mode","Reset the board for other demos", MSG_INFO);
  MX_USB_DEVICE_Init();

  StreamDispatcherInit();
  ceInitalize();
  while(1)
  {
      ProcessIO();
      dispatcherWorker(); /* Call all functions need by the dispatcher */
      ceHandler();
  }

}

/**
  * @brief  Display splash screen.
  */
void SplashScreen( void )
{
  Menu_DisplayClear();
  Menu_DisplayPicture( 0, 2, cStLogo );
  HAL_Delay( 500 );

  BSP_LCD_Clear( LCD_COLOR_BLUEST );
  Menu_SetStyle( CLEAR_PLAIN );

#ifdef ST25R3916B
  Menu_DisplayString( Line1, "  >> ST25R3916B JORDAN<<  " );
#else
  Menu_DisplayString( Line1, "   >> ST25R3916 JORDAN<<  " );
#endif
  Menu_DisplayString( Line2, "   NFC Reader demo JORDAN  " );
  Menu_DisplayString( Line3, "    Version "MCU_FW_VERSION"   " );
  Menu_DisplayString( Line5, " STMicroelectronics " );
  Menu_DisplayString( Line6, "   www.st.com/st25r " );
  Menu_DisplayString( Line8, "      MMY Team      " );
  Menu_DisplayString( Line9, "   COPYRIGHT 2021   " );

  HAL_Delay( 1000 );
}

/**
  * @brief Display "_about_" screen.
  */
void MenuAbout( void )
{
#ifdef ST25R3916B
  char *msg = "\n\nST25R3916B Demo\n\n\nVersion "MCU_FW_VERSION"\n\n\f\nSTMicroelectronics\n\nwww.st.com/st25r\n\nMMY Team\nCOPYRIGHT 2021\n\n";
#else
  char *msg = "\n\nST25R3916 Demo\n\n\nVersion "MCU_FW_VERSION"\n\n\f\nSTMicroelectronics\n\nwww.st.com/st25r\n\nMMY Team\nCOPYRIGHT 2019\n\n";
#endif
  Menu_MsgStatus( "Credits :", msg, MSG_ABOUT );
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   */
void assert_failed( uint8_t* file, uint32_t line )
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif


static void SystemClock_Config_st25r3916(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
    while(1);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
    while(1);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 12;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
    while(1);
  }

    /**Configure the main internal regulator output voltage
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
    while(1);
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* UART1 init function */
static void MX_UART1_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
/**
  * @}
  */


/************************ (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
