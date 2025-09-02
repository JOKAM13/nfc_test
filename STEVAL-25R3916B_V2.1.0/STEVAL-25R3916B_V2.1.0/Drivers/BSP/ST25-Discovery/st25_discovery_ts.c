/**
  ******************************************************************************
  * @file    st25_discovery_ts.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage Touch 
  *          screen available with STMPE811 IO Expander device mounted on 
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
#include "st25_discovery_ts.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup ST25_DISCOVERY_BSP
  * @{
  */ 
  
/** @defgroup ST25_DISCOVERY_TS
  * @{
  */ 

static TS_DrvTypeDef     *TsDrv = NULL;
static uint16_t          TsXBoundary, TsYBoundary;

/**
  * @brief  Initializes and configures the touch screen functionalities and 
  *         configures all necessary hardware resources (GPIOs, clocks..).
  * @param  XSize: The maximum X size of the TS area on LCD
  * @param  YSize: The maximum Y size of the TS area on LCD  
  * @retval TS_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_Init(uint16_t XSize, uint16_t YSize)
{
  uint8_t ret = TS_ERROR;

  /* Initialize x and y positions boundaries */
  TsXBoundary = XSize;
  TsYBoundary = YSize;

  /* Read ID and verify if the IO expander is ready */
  if( stmpe811_ts_drv.ReadID(ST25_DISCOVERY_TS_I2C_ADDRESS) == STMPE811_ID )
  {
    /* Initialize the TS driver structure */
    TsDrv = &stmpe811_ts_drv;

    ret = TS_OK;
  }

  if(ret == TS_OK)
  {
    /* Initialize the LL TS Driver */
    TsDrv->Init( ST25_DISCOVERY_TS_I2C_ADDRESS );
    TsDrv->Start( ST25_DISCOVERY_TS_I2C_ADDRESS );
  }

  return ret;
}

/**
  * @brief  Configures and enables the touch screen interrupts.
  * @retval TS_OK: if ITconfig is OK. Other value if error.
  */
uint8_t BSP_TS_ITConfig(void)
{
  uint8_t ret = TS_ERROR;
  
  if(TsDrv != NULL)
  {
    /* Enable the TS ITs */
    TsDrv->EnableIT(ST25_DISCOVERY_TS_I2C_ADDRESS);
    
    ret = TS_OK;
  }

  return ret;
}

/**
  * @brief  Gets the TS IT status.
  * @retval Interrupt status.
  */  
uint8_t BSP_TS_ITGetStatus(void)
{
   uint8_t status = 0;
  
  if(TsDrv != NULL)
  {
    /* Return the TS IT status */
    status = TsDrv->GetITStatus(ST25_DISCOVERY_TS_I2C_ADDRESS);
  }
  
  return status;
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TsState: Pointer to touch screen current state structure
  */
void BSP_TS_GetState(TS_StateTypeDef* TsState)
{
  static uint32_t _x = 0, _y = 0;
  uint16_t xDiff, yDiff , x , y, xr, yr;
  
  if(TsDrv == NULL)
  {
    TsState->TouchDetected = 0;
    TsState->X = 0;
    TsState->Y = 0;
  }
  else
  {
    TsState->TouchDetected = TsDrv->DetectTouch(ST25_DISCOVERY_TS_I2C_ADDRESS);
  }
  
  if(TsState->TouchDetected)
  {
    TsDrv->GetXY(ST25_DISCOVERY_TS_I2C_ADDRESS, &x, &y);
    
    /* Y value first correction */
    y -= 360;  
    
    /* Y value second correction */
    yr = y / 11;
    
    /* Return y position value */
    if(yr <= 0)
    {
      yr = 0;
    }
    else if (yr > TsYBoundary)
    {
      yr = TsYBoundary - 1;
    }
    else
    {}
    y = yr;
    
    /* X value first correction */
    if(x <= 3000)
    {
      x = 3870 - x;
    }
    else
    {
      x = 3800 - x;
    }
    
    /* X value second correction */  
    xr = x / 15;
    
    /* Return X position value */
    if(xr <= 0)
    {
      xr = 0;
    }
    else if (xr > TsXBoundary)
    {
      xr = TsXBoundary - 1;
    }
    else 
    {}
    
    x = xr;
    xDiff = x > _x? (x - _x): (_x - x);
    yDiff = y > _y? (y - _y): (_y - y); 
    
    if (xDiff + yDiff > 5)
    {
      _x = x;
      _y = TsYBoundary - y; 
    }
    
    /* Update the X position */
    TsState->X = _y;
    
    /* Update the Y position */  
    TsState->Y = _x;
  }
}

/**
  * @brief  Clears all touch screen interrupts.
  */  
void BSP_TS_ITClear(void)
{
  if(TsDrv != NULL)
  {
    /* Clear TS IT pending bits */
    TsDrv->ClearIT(ST25_DISCOVERY_TS_I2C_ADDRESS);
  }
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
