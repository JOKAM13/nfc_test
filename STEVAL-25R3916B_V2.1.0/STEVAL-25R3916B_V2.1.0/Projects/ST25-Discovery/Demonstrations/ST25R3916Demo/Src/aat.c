/**
  ******************************************************************************
  * @file    aat.c
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @ingroup ST25R3916_Discovery_Demo
  * @brief   Antenna Tuning demo
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

#include "st25r3916_com.h"
#include "st25r3916.h"
#include "st25r3916_aat.h"
#include "rfal_rf.h"
#include "stm32l4xx_hal.h"
#ifdef ST25R3916B
#include "3916b_aat_jpg.h"
#else
#include "Gauge_light_jpg.h"
#endif

#include "BarV_jpg.h"
#include "CursorV_jpg.h"
#include "CursorBlue_jpg.h"
#include "CadreCp_jpg.h"
#include "CadreCs_jpg.h"
#include "st25_discovery_lcd.h"

#include "Menu_config.h"
#include "Menu_cursor.h"
#include "tuneAntenna.h"
#include "math.h"
#include "stdio.h"
#include <inttypes.h>

void InitializeRFAL(void);

#define DEFAULT_ANT_TUNE_A 0x7F
#define DEFAULT_ANT_TUNE_B 0x7F

/** Capacitance cursor definition */
enum {
  CP = 0,   /** Parallel capacitance cursor Id */
  CS = 1,   /** Serial Capacitance cursor Id*/
  NB_CURSORS
} cursor_id_t;

/* Capacitance Cursor bars */
static Menu_Bar_t CpBar, CsBar;
/* Capacitance Cursors */
static Menu_Cursor_t CpCursor, CsCursor;
static Menu_Cursor_t *currentCursor = &CsCursor;

/* Operation control register value */
static unsigned char reg_op_ctrl;
/* IO configuration register value */
static unsigned char reg_io_cfg_02;

/* Length of the cursor bars (in pixels) */
static const uint32_t barLength = 125;
/* Position of the cursor bars (in pixels) */
static const uint32_t barPos = 30;
/* Antenna amplitude factor */
static const double realLengthFactor = 0.01302;

/* Method to draw a needle with a given amplitude & phase */
static void drawNeedle(double amp, double phase)
{
  phase = round(phase) * 3.141592 / 180;
  amp *= 30;
  for(int i = 0 ; i < amp ; i +=1)
  {
      BSP_LCD_FillRect(160 + i * cos(phase),143 - i * sin(phase),3,3);
  }
}

/* Helper function to know if Amplitude or Phase has changed, and so a refresh is needed */
static bool hasChanged(uint8_t Amp, uint8_t Phase, bool first)
{
  static uint8_t prevAmp = 0;
  static uint8_t prevPhase = 0;
  if(first || (prevAmp != Amp) || (prevPhase != Phase))
  {
    prevAmp = Amp;
    prevPhase = Phase;
    return true;
  }
  return false;
}

/* Update the text label is associated which each capacitance cursor, with current cursor value */
void updateCursorText(Menu_Cursor_t *cursor)
{
  const uint32_t textY = 190;
  char text[10] = "";
  sprintf(text,"%03"PRIu32,cursor->Value);
  BSP_LCD_DisplayStringAt(cursor->Bar->X - 24,textY,(uint8_t*)text,LEFT_MODE);

}

/* Special grade character, for Phase display, missing from regular Font */
static const uint8_t gradeC[] = {
	// @2816 '`' (16 pixels wide)
	0x00, 0x00, //
 	0x1E, 0x00, //       ####        
	0x33, 0x00, //      ##  ##      
	0x33, 0x00, //      ##  ##      
	0x33, 0x00, //      ##  ##      
	0x1E, 0x00, //       ####        
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x00, 0x00 //                 
};

/* Reset the Capacitance to their default value */
void setDefaultCapacitance(void)
{
  st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_A, DEFAULT_ANT_TUNE_A);
  st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_B, DEFAULT_ANT_TUNE_B);
  Menu_CursorSetValue(&CsCursor,DEFAULT_ANT_TUNE_A);
  Menu_CursorSetValue(&CpCursor,DEFAULT_ANT_TUNE_B);
}

#ifdef DEBUG_AAT
uint8_t DBG_phase[20];
#endif

/** @brief Run antenna tuning demo.
           Serial & Parallel capacitors can be set to different values, using cursors.
           They can also be automatically setup for maximum amplitude (in this case, previous setting is shown as a ghost needle).
           The antenna tuning is displayed as a needle showing antenna measurement amplitude & phase.
  */
void startAAT (void) 
{
  char text[21];
  struct st25r3916AatTuneResult tuningStatus;
  float Amp = 0, Phase =0;
  int tune = 0;
  int enable_auto_tune = 1;
  bool first =true;
  uint8_t ButtonState = 0;
  int err_status = RFAL_ERR_NONE;
  double ghostAmp, ghostPhase;
  bool drawGhost = false;
  uint8_t joypress;
  Menu_Direction_t JoyState;
  uint32_t timeelapsed = 0;

#if defined(USE_MB1414_B) || defined(USE_MB1749_A)
  static uint8_t displayAAT_warning = 1;
  if(displayAAT_warning)
  {
    uint32_t y = 70;
    
    Menu_MsgStatus("Antenna Tuning","",MSG_INFO);
    BSP_LCD_SetFont(&Font16);
    Menu_SetStyle(PLAIN);
    y += Menu_DisplayLongString(20, y, "The matching on STEVAL-D25R16B only allows limited tuning range.", 30) * 16 + 16;
    Menu_DisplayLongString(20, y, "Please consult AN5322 for maximum tuning range.", 30);
    BSP_LCD_SetFont(&Font22);
    Menu_SetStyle(CLEAR_PLAIN);
#if defined(USE_JOYSTICKONLY)
    Menu_DisplayCenterString(10, "PB Blue to continue");
#else
    Menu_DisplayCenterString(10, "Touch to continue");
#endif /* USE_JOYSTICKONLY */
    displayAAT_warning = 0;
    Menu_WaitForUser();
  }
#endif

  Menu_MsgStatus("Antenna Tuning","",MSG_INFO);
#if defined(USE_JOYSTICKONLY)
    Menu_DisplayCenterString(10, "PB Blue to exit");
#else
    Menu_DisplayCenterString(10, "Touch to exit");
#endif /* USE_JOYSTICKONLY */
  Menu_SetStyle(GREY);

  Menu_DisplayPicture(40,40,Gauge_light);

  Menu_BarInit(290,barPos,barLength,MENU_BAR_VERTICAL,GREY,&CsBar);
  Menu_BarInit(30,barPos,barLength,MENU_BAR_VERTICAL,GREY,&CpBar);

  Menu_CursorInit(&CsBar,0,0xFF,DEFAULT_ANT_TUNE_A,CursorBlue,20,&updateCursorText,&CsCursor);
  Menu_CursorInit(&CpBar,0,0xFF,DEFAULT_ANT_TUNE_B,CursorV,20,&updateCursorText,&CpCursor);

  Menu_DisplayPicture(4, 175,CadreCp);
  Menu_DisplayPicture(264, 175,CadreCs);

  InitializeRFAL();
  rfalFieldOnAndStartGT();

  st25r3916ReadRegister(ST25R3916_REG_IO_CONF2, &reg_io_cfg_02);
  st25r3916ReadRegister(ST25R3916_REG_OP_CONTROL, &reg_op_ctrl);

  st25r3916WriteRegister(ST25R3916_REG_IO_CONF2, reg_io_cfg_02 | ST25R3916_REG_IO_CONF2_aat_en);
  st25r3916WriteRegister(ST25R3916_REG_OP_CONTROL, reg_op_ctrl | ST25R3916_REG_OP_CONTROL_en);


  setDefaultCapacitance();

  while(true)
  {
    if(tune && enable_auto_tune)
    {
      uint32_t duration = HAL_GetTick();
      if(enable_auto_tune)
      {
#if !defined(USE_MB1414_B) && !defined(USE_MB1749_A)
        st25r3916AatTune(NULL,&tuningStatus);
#else // use MB1414-B
        struct st25r3916AatTuneParams tuningParams = {
          .aat_a_min        = 0,
          .aat_a_max        = 0xFF,
          .aat_a_start      = 0x7F,
          .aat_a_stepWidth  = 32,
          .aat_b_min        = 0,
          .aat_b_max        = 0xFF,
          .aat_b_start      = 0x7F,
          .aat_b_stepWidth  = 32,
          .phaTarget        = 0x80,
          .phaWeight        = 0,
          .ampTarget        = 0xB1,
          .ampWeight        = 1,
          .doDynamicSteps   = 1,
          .measureLimit     = 0x46
        };

        st25r3916AatTune(&tuningParams,&tuningStatus);
#endif
        st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_A, tuningStatus.aat_a);
        st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_B, tuningStatus.aat_b);
      }

      duration = HAL_GetTick() - duration;
    
      Menu_CursorSetValue(&CsCursor,tuningStatus.aat_a);
      Menu_CursorSetValue(&CpCursor,tuningStatus.aat_b);
      tune--;
      
    }
      // use measurement, removing possible oscillations when at limit values (100 ms)
      uint32_t sumAmp = 0;
      uint32_t sumPhase = 0;
      err_status = RFAL_ERR_NONE;

      for(int p =0; p < 20; p++)
      {
        HAL_Delay(5);
        err_status |= st25r3916MeasurePhase(&tuningStatus.pha);
#ifdef DEBUG_AAT
        DBG_phase[p] = tuningStatus.phaTuned;
#endif
        err_status |= st25r3916MeasureAmplitude(&tuningStatus.amp);
        sumAmp += tuningStatus.amp;
        sumPhase += tuningStatus.pha;
      }

      tuningStatus.amp = sumAmp / 20;
      tuningStatus.pha = sumPhase / 20;

    Phase = (float)(180 - (tuningStatus.pha * 0.577) - 16.36);
    Amp = tuningStatus.amp * realLengthFactor;

    if(hasChanged(tuningStatus.amp, tuningStatus.pha, first))
    {
        Menu_DisplayPicture(40,40,Gauge_light);
        if(enable_auto_tune && drawGhost)
        {
          BSP_LCD_SetTextColor(BSP_LCD_FadeColor(LCD_COLOR_BLUEST,LCD_COLOR_LIGHTGRAY, 100, 90));
          drawNeedle(ghostAmp,ghostPhase);
        }
        Menu_SetStyle(PLAIN);
        drawNeedle(Amp,Phase);

        sprintf(text,"Ampli %1d.%02d V",(int)Amp,((int)(Amp * 100))%100);
        BSP_LCD_DisplayStringAt(70,174,(uint8_t*)text,LEFT_MODE);
        if(Phase >= 100)
          sprintf(text,"Phase %3d.%01d",(int)Phase,((int)(Phase * 10))%10);
        else
          sprintf(text,"Phase %2d.%02d",(int)Phase,((int)(Phase * 100))%100);
        BSP_LCD_DisplayStringAt(70,196,(uint8_t*)text,LEFT_MODE);
        BSP_LCD_DrawChar(248,196,gradeC);
        Menu_SetStyle(GREY);
        first = false;

    }

    timeelapsed = HAL_GetTick();
    joypress = Menu_ReadDirection(&JoyState);
    if((JoyState.Sel) && ((HAL_GetTick() - timeelapsed) < 1000))
    {
      // back to default values
      drawGhost = false;
      setDefaultCapacitance();
      HAL_Delay(250);
    }
    else if(JoyState.Sel)
    {
      if (!enable_auto_tune)
      {
        // reset default values
        setDefaultCapacitance();
      } else {
        ghostAmp = Amp;
        ghostPhase = Phase;
        drawGhost = true;
        tune = 1;
      }
    }
    else if(JoyState.Dir == MENU_LEFT)
    {
      if(currentCursor != &CpCursor)
      {
        currentCursor = &CpCursor;
        CpCursor.Picture = CursorBlue;
        CsCursor.Picture = CursorV;
        Menu_CursorSetValue(&CpCursor, CpCursor.Value);
        Menu_CursorSetValue(&CsCursor, CsCursor.Value);
      }
    }
    else if(JoyState.Dir == MENU_RIGHT)
    {
      if(currentCursor != &CsCursor)
      {
        currentCursor = &CsCursor;
        CsCursor.Picture = CursorBlue;
        CpCursor.Picture = CursorV;
        Menu_CursorSetValue(&CpCursor, CpCursor.Value);
        Menu_CursorSetValue(&CsCursor, CsCursor.Value);
      }
    }

    uint8_t btnpress;
    Menu_Position_t Pos;
    Menu_ReadPosition(&Pos);
    btnpress = Menu_ReadSelection(&ButtonState);
    if((Pos.Sel) || (joypress != 0) || (btnpress != 0))
    {
      if(((Pos.Y > 210) && (Pos.Sel)) || ((btnpress != 0) && (ButtonState != 0)))
      {
        // exit
        rfalFieldOff();
        break;
      } else if(Pos.Sel)
      {
        if(Menu_CursorInput(&CsCursor,&Pos))
        {
          st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_A, CsCursor.Value);
          currentCursor = &CsCursor;
          CsCursor.Picture = CursorBlue;
          CpCursor.Picture = CursorV;
          Menu_CursorSetValue(&CpCursor, CpCursor.Value);
          Menu_CursorSetValue(&CsCursor, CsCursor.Value);
          HAL_Delay(250);
        } else if(Menu_CursorInput(&CpCursor,&Pos))
        {
          st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_B, CpCursor.Value);
          currentCursor = &CpCursor;
          CpCursor.Picture = CursorBlue;
          CsCursor.Picture = CursorV;
          Menu_CursorSetValue(&CpCursor, CpCursor.Value);
          Menu_CursorSetValue(&CsCursor, CsCursor.Value);          
          HAL_Delay(250);
        } else if((Pos.X > 40) && (Pos.Y > 40) && (Pos.X < 280) && (Pos.Y < 200)) {
          if (!enable_auto_tune)
          {
            // reset default values
            setDefaultCapacitance();
          } else {
            ghostAmp = Amp;
            ghostPhase = Phase;
            drawGhost = true;
            tune = 1;
          }
        }
      }
      else if(joypress != 0)
      {
        Menu_CursorJoyInput(currentCursor, &JoyState);
      }
    }
  }
}
