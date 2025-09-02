/**
  ******************************************************************************
  * @file    tag_detection.c
  * @author  MMY Application Team
  * @version $Revision:$
  * @date    $Date:$
  * @brief   This file executes the tag detection with a 39xx reader.
  * @details The demos displays up to 3 tags on the LCD screen,
  *          showing their type and the 4 last bytes of their UID.
  *          The demo displays the current DPO value.
  * 
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
#include "string.h"
#include "stdlib.h"

/* Display management */
#include "st25_discovery_lcd.h"
#include "Menu_core.h"
#include "Menu_config.h"
#include "cartouche_jpg.h"

extern LCD_DrawPropTypeDef DrawProp;
void tagDetectionDemo (void)
{
  Menu_MsgStatus("Tag detection","",MSG_INFO);
  Menu_SetStyle(PLAIN);
  Menu_DisplayPicture(8,30,cartouche);
  Menu_DisplayPicture(8,80,cartouche);
  Menu_DisplayPicture(8,130,cartouche);
  BSP_LCD_DisplayStringAt(DrawProp.pFont->Width,2*DrawProp.pFont->Height, "T4A    00:00:00:00",LEFT_MODE);
  while(!Menu_GetUserInput())
  {
     Menu_Delay(10);
  }
}