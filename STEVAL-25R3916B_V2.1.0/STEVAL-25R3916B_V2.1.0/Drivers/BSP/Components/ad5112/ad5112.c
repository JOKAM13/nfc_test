/**
  ******************************************************************************
  * @file    ad5112.c 
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @brief   This file provides a set of functions needed to manage the AD5112
  *          Digital Potentiometer devices.
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

/* Includes ------------------------------------------------------------------*/
#include "ad5112.h"

/* Private function prototypes -----------------------------------------------*/
uint8_t ad5112_ReadSavedResistor( uint8_t Addr );
uint8_t ad5112_ReadResistorTolerance( uint8_t Addr );
/* Global variables ----------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
DPOT_DrvTypeDef ad5112_dpot_drv = 
{	
  ad5112_Init,
  0,
  ad5112_Reset,
  ad5112_GetResistor,
  ad5112_SetResistor,
  ad5112_Shutdown,
  0
};

DPOT_ExtDrvTypeDef ad5112_dpot_Extdrv =
{
  ad5112_Start,
  ad5112_ReadSavedResistor,
  ad5112_ReadResistorTolerance
};

/* Private variables ---------------------------------------------------------*/
/* Public functions ---------------------------------------------------------*/


/**
 * @brief   This function init the HW for AD5112 
 * @param   Addr	I2C address of the AD5112 device
 * @retval  None
 */
void ad5112_Init( uint8_t Addr )
{
  DPOT_IO_Init( Addr );
}

/**
 * @brief   This function reads the resistor value from AD5112 
 * @param   Addr	I2C address of the AD5112 device
 * @return  Return value read
 */
uint8_t ad5112_GetResistor( uint8_t Addr )
{
  uint16_t reg = ((uint16_t)AD5112_READRDAC) << 8;
  return DPOT_IO_Read( Addr, reg );
}

/**
 * @brief   This function write the resistor value to AD5112
 * @param   Addr	I2C address of the AD5112 device
 * @param   Data	Resistor value to be written
 * @retval	None
 */
void ad5112_SetResistor( uint8_t Addr, uint8_t Data )
{
  uint8_t buffer[2] = { AD5112_WRITE2RDAC, Data };
  DPOT_IO_WriteBuffer( Addr, buffer, 2 ); 
}

/**
 * @brief   This function set the AD5112 in Reset mode 
 * @param   Addr	I2C address of the AD5112 device
 * @retval	None
 */
void ad5112_Reset( uint8_t Addr )
{
  uint8_t buffer[2] = { AD5112_RESET, 0 };
  DPOT_IO_WriteBuffer( Addr, buffer, 2 );
}

/**
 * @brief   This function set the AD5112 in Shutdown mode 
 * @param   Addr	I2C address of the AD5112 device
 * @retval	None
 */
void ad5112_Shutdown( uint8_t Addr )
{
  uint8_t buffer[2] = { AD5112_SHUTDOWN, 1 };
  DPOT_IO_WriteBuffer( Addr,buffer, 2 );
}

/**
 * @brief   This function start the AD5112 
 * @param   Addr	I2C address of the AD5112 device
 * @retval	None
 */
void ad5112_Start( uint8_t Addr )
{
  uint8_t buffer[2] = { AD5112_SHUTDOWN, 0 };
  DPOT_IO_WriteBuffer( Addr,buffer, 2 );
}

/**
 * @brief   This function reads the saved resistor value from AD5112
 * @param   Addr	I2C address of the AD5112 device
 * @return	None
 */
uint8_t ad5112_ReadSavedResistor( uint8_t Addr )
{
  uint16_t reg = ((uint16_t)AD5112_READEEPROM) << 8;
  return DPOT_IO_Read( Addr, reg ); 
}

/**
 * @brief   This function reads the resistor tolerance value from AD5112
 * @param   Addr	I2C address of the AD5112 device
 * @return	None
 */
uint8_t ad5112_ReadResistorTolerance( uint8_t Addr )
{
  uint16_t reg = ((uint16_t)AD5112_READEEPROM) << 8;
  reg |= 1;
  return DPOT_IO_Read( Addr, reg ); 
}
