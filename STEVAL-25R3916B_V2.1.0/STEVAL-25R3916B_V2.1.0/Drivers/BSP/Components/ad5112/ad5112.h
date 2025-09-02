/**
  ******************************************************************************
  * @file    ad5112.h 
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @brief   This file provides set of driver functions to manage the AD5112 chip.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
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


#ifndef __AD5112_H
#define __AD5112_H

/* Includes ------------------------------------------------------------------*/
#include "dpot.h"

/* Exported types ------------------------------------------------------------*/
typedef enum 
{
  AD5112_ADDR0=0x58,
  AD5112_ADDR1=0x5E
} ad5112_addr_t;

typedef enum 
{
  AD5112_NOP = 0,
  AD5112_WRITE2EEPROM = 1,
  AD5112_WRITE2RDAC = 2,
  AD5112_SHUTDOWN = 3,
  AD5112_RESET = 4,
  AD5112_READRDAC = 5,
  AD5112_READEEPROM = 6
} ad5112_instr_t;

typedef struct
{
  void (*ad5112_Start)( uint8_t );
  uint8_t (*ReadSavedResistor)( uint8_t );
  uint8_t (*ReadResistorTolerance)( uint8_t );
} DPOT_ExtDrvTypeDef;

/* Exported constants --------------------------------------------------------*/
extern DPOT_DrvTypeDef ad5112_dpot_drv;
extern DPOT_ExtDrvTypeDef ad5112_dpot_Extdrv;

/* Exported macro ------------------------------------------------------------*/
#define DPOT_I2C_TIMEOUT        0x3000
#define AD5112_MINRANGE         255
#define AD5112_MIDRANGE         191
#define AD5112_MAX              64
#define AD5112_RAB_RESISTANCE   10000
#define AD5112_WIPER_RESISTANCE 70
#define AD5112_WIPER_PRECISION  64

/* Exported functions --------------------------------------------------------*/
void ad5112_Init( uint8_t Addr );
uint8_t ad5112_GetResistor( uint8_t Addr );
void ad5112_SetResistor( uint8_t Addr, uint8_t Data );
void ad5112_Reset( uint8_t Addr );
void ad5112_Shutdown( uint8_t Addr );
void ad5112_Start( uint8_t Addr );
uint8_t ad5112_ReadReg( uint8_t Addr );

/* DPOT IO BUS functions */
void DPOT_IO_Init( uint8_t Addr );
uint8_t DPOT_IO_Read( uint8_t Addr, uint16_t Reg );
void DPOT_IO_WriteBuffer( uint8_t Addr, uint8_t *pBuf, uint16_t Len );

#endif /* __AD5112_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
