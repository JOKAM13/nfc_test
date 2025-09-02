/**
  ******************************************************************************
  * @file    st25_discovery_dpot.c 
  * @author  MMY Application Team
  * @brief   This file provides a set of functions needed to manage Digital 
  *          potentiometer available with ad5161 device mounted on 
  *          ST25-Discovery Kit.
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

#include "st25_discovery_dpot.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup ST25_DISCOVERY_BSP
  * @{
  */ 
  
/** @defgroup ST25_DISCOVERY_DPOT ST25 DISCOVERY DIGITAL POTENTIOMETER
  * @{
  */ 

static DPOT_DrvTypeDef *DpotDrv;
/**
  * @brief  Configures all necessary hardware resources (GPIOs, clocks..).
  * @retval DPOT_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_DPOT_Init( void )
{

  /* Initialize the DPOT driver structure */
  DpotDrv = &ad5112_dpot_drv;
  DpotDrv->pData = &ad5112_dpot_Extdrv;
  
  DpotDrv->Init( ST25_DISCOVERY_DPOT_I2C_ADDR );

  return DPOT_OK;
}


/**
  * @brief  Returns DPOT resistor value.
  * @retval Resistor value
  */
uint32_t BSP_DPOT_GetResistor( void )
{
  uint8_t temp_resistor = 0;
  uint32_t resistor_value = 0;
  
  temp_resistor = DpotDrv->GetResistor( ST25_DISCOVERY_DPOT_I2C_ADDR );
  temp_resistor = temp_resistor >> 1;
  resistor_value = ((temp_resistor * AD5112_RAB_RESISTANCE) / AD5112_WIPER_PRECISION) + AD5112_WIPER_RESISTANCE;
  
  return resistor_value;
}

/**
  * @brief  Set DPOT resistor value.
  * @param Resistor value
  * @retval None
  */
void BSP_DPOT_SetResistor( uint32_t Val )
{
  uint8_t resistor_value = 0;
  
  if( Val <= AD5112_WIPER_RESISTANCE )
  {
    resistor_value = 0xFF;
  }
  else if( Val > 10000 )
  {
    resistor_value = 0x80;
  }
  else
  {
    resistor_value = ((Val - AD5112_WIPER_RESISTANCE) * AD5112_WIPER_PRECISION) / AD5112_RAB_RESISTANCE;
    resistor_value = resistor_value << 1;
  }
  
  
  
  DpotDrv->SetResistor( ST25_DISCOVERY_DPOT_I2C_ADDR, resistor_value );
}

/**
  * @brief  Set DPOT in Reset state .
  * @retval None
  */
void BSP_DPOT_Reset( void )
{
   DpotDrv->Reset( ST25_DISCOVERY_DPOT_I2C_ADDR );
}

/**
  * @brief  Set DPOT in Reset state .
  * @retval None
  */
void BSP_DPOT_Shutdown( void )
{
   DpotDrv->Shutdown( ST25_DISCOVERY_DPOT_I2C_ADDR );
}

/**
  * @brief  Access to the extended features of the digital potentiometer.
  * @return Pointer on the Extended Component Structure for the nfctag.
  */
DPOT_ExtDrvTypeDef *BSP_DPOT_GetExtended_Drv( void )
{
  return (DPOT_ExtDrvTypeDef *)DpotDrv->pData;
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
