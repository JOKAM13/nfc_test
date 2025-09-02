/******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2024 STMicroelectronics</center></h2>
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
******************************************************************************/


/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
// Generic includes
#include <stdio.h>

// reader includes
#include "rfal_platform.h"
#include "utils.h"
#include "rfal_dpo.h"

#include "st25_discovery_st25r.h"
#include "ndef_display.h"
#include "ndef_poller.h"
#include "ndef_message.h"
#include "ndef_types.h"
#include "ndef_type_uri.h"

/* Display management */
#include "Menu_core.h"
#include "Menu_config.h"
#include "Menu_tab.h"
#include "cartouche_jpg.h"
#include "send_icon_jpg.h"
#include "Error_jpg.h"
#include "Success_jpg.h"

#ifdef ENABLE_TRUST25_SIGNATURE_VERIFICATION
#include "TruST25.h"
#endif

//#define FREEZE_DISPLAY 1

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/
#define EXAMPLE_RFAL_POLLER_DEVICES      4    /* Number of devices supported */

#define RADIO_X 20 /* Radio button X position (to select tag detection method) */
#define RADIO_Y 50 /* Radio button Y position (to select tag detection method) */

#define UIDLINE     1 /* Line Id to display Tag UID */
#define HBRLINE     3 /* Line Id to display High Bitrate info */
#define RSSILINE    5 /* Line Id to display RSSI info */
#define RSSIBARLINE 6 /* Line Id to display RSSI info as a bar */
#define XMARGIN     15 /* Tag information margin */

#define DEMO_ST_NOTINIT               0  /*!< Demo State:  Not initialized */
#define DEMO_ST_START_DISCOVERY       1  /*!< Demo State:  Start Discovery */
#define DEMO_ST_DISCOVERY             2  /*!< Demo State:  Discovery       */
#define DEMO_ST_TAG_OPERATION         3  /*!< Demo State:  Discovery       */

/*
******************************************************************************
* GLOBAL TYPES
******************************************************************************
*/
/** Detection mode for the demo */
typedef enum {
  DETECT_MODE_POLL = 0,     /** Continuous polling for tags */
  DETECT_MODE_WAKEUP = 1,   /** Waiting for the ST25R3916 wakeup detection */
  DETECT_MODE_AWAKEN = 2    /** Awaken by the ST25R3916 wakeup feature */
} detectMode_t;

/*
 ******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************
 */
static rfalNfcDiscoverParam    discParam;
static uint8_t                 state = DEMO_ST_NOTINIT;
static uint8_t                 deviceToDisplay = 0xFF;
static ndefContext             ndefctx;
static uint8_t                 multiSel = 0;
static uint8_t                 gDevNb = 0;
static rfalNfcDevice           *gDeviceList;
static char                    *gTypeStrList[] = { [RFAL_NFC_LISTEN_TYPE_NFCA] = "NFC-A",
                                                   [RFAL_NFC_LISTEN_TYPE_NFCB] = "NFC-B",
                                                   [RFAL_NFC_LISTEN_TYPE_NFCF] = "NFC-F",
                                                   [RFAL_NFC_LISTEN_TYPE_NFCV] = "NFC-V",
                                                   [RFAL_NFC_LISTEN_TYPE_ST25TB] = "ST25TB"
                                                 };

struct structnfcSelectedDevice
{
  rfalNfcDevType type;
  uint8_t nfcid[10];
  uint8_t nfcidLen;
} nfcSelectedDevice;


static rfalBitRate rxbr, txbr;                                           /* Detected tag bitrate information */
static bool instructionsDisplayed = false;                              /* Keep track of demo instruction display */
static const uint16_t yBox = 24;                                        /* Detected tag box margin*/
static const uint16_t boxHeight = 45;                                   /* Detected tag box height */
static const uint16_t boxSpace = 4;                                     /* Detected tag box spacing */
static uint8_t curTagSelected = 0;                                      /* Current tag selected in menu */
static detectMode_t detectMode = DETECT_MODE_POLL;                      /* Current tag detection method */
static uint16_t lineY[] = {75, 100, 116, 141, 157, 182, 192};           /* Tag information line position */
static char type1Str[] =  "Type 1 Tag";                                 /* NFC Forum Tag Type1 string */
static char type2Str[] =  "Type 2 Tag";                                 /* NFC Forum Tag Type2 string */
static char type3Str[] =  "Type 3 Tag";                                 /* NFC Forum Tag Type3 string */
static char type4aStr[] = "Type 4A Tag";                                /* NFC Forum Tag Type4A string */
static char type4bStr[] = "Type 4B Tag";                                /* NFC Forum Tag Type4B string */
static char type5Str[] =  "Type 5 Tag";                                 /* NFC Forum Tag Type5string */
static char iso15693Str[] =  "Iso15693 Tag";                            /* iso15693 Tag string */
static char st25tbStr[] = "ST25TB Tag";                                 /* ST25TB tag string */
static char iso14443aStr[] = "Iso14443A Tag";                           /* Iso14443A tag string */
static char felicaStr[] = "Felica Tag";                                 /* Felica tag string */
static char nfcdep4aStr[] = "P2P Type 4A";                              /* NFC Forum NFC-DEP Type4A string */
#if !defined(ST25R3916B)
static const char *dpoProfile[] = {"Full power", "Med. power", "Low power "}; /* Dynamic Power Output profile strings*/
#endif
static uint16_t lastRssi;                                               /* RSSI history value */
static uint32_t timeRssi = 0;                                           /* Use to detect long latency in tag response */

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
static void exampleRfalPollerRun( void );
static void demoNotif( rfalNfcState st );
static Menu_Callback_Status_t tabFirstTagReadInfo(void);
static Menu_Callback_Status_t tabTagReadInfo(void);
static Menu_Callback_Status_t tabFirstTagReadNdef(void);
static Menu_Callback_Status_t tabTagReadNdef(void);

/* Tag Tab setup */
static Menu_Tab_Setup_t tabSetup[] = {
{"Tag Info", &tabFirstTagReadInfo, &tabTagReadInfo},  /* Tag information Tab */
{"NDEF", &tabFirstTagReadNdef, &tabTagReadNdef},      /* Tag content Tab */
};

/*
******************************************************************************
* LOCAL FUNCTION
******************************************************************************
*/
/* Compute detected tag Y position */
static uint16_t getTagBoxY(int index)
{
  return yBox + index * (boxHeight + boxSpace);
}

/* Display a detected tag on the LCD screen (max: 4 tags detected) */
static void displayTag(int index, char* type, uint8_t *uid)
{
  const uint16_t xBox = 8;
  const uint16_t boxWidth = 304;
  const uint16_t boxCornerRadius = 10;
  const uint16_t yText = 15;

  char str[30] = ""; 
  char uid_str[30] = "";

  if (index < 4)
  {
    if(uid != NULL)
    {
      strcat(str, type);
      for(int c = strlen(type); c < 7; c++)
      {
        strcat(str, " ");
      }
      sprintf(uid_str, "%02X:%02X:%02X:%02X", uid[0], uid[1], uid[2], uid[3]);
      strcat(str, uid_str);
    }
    else
      {
      strcpy(str, "                  ");
    }

#ifndef FREEZE_DISPLAY
    BSP_LCD_DrawRectangleWithRoundCorner(xBox, getTagBoxY(index), boxWidth, boxHeight, boxCornerRadius);
    Menu_DisplayStringAt(Menu_GetFontWidth(), (yText + Menu_GetFontHeight() + index * (boxHeight + boxSpace)), str);
#endif
  }
}

/* Control Radio Button display */
static void setRadio(void)
{
#ifndef FREEZE_DISPLAY
  if(detectMode == DETECT_MODE_POLL)
  {
    BSP_LCD_SetColors(LCD_COLOR_BLUE2, 0xFFFF);
    BSP_LCD_FillCircle(RADIO_X, RADIO_Y, 5);
    BSP_LCD_SetColors(0xFFFF, 0xFFFF);
    BSP_LCD_FillCircle(RADIO_X, RADIO_Y + 40, 5);
  } else {
    BSP_LCD_SetColors(LCD_COLOR_BLUE2, 0xFFFF);
    BSP_LCD_FillCircle(RADIO_X, RADIO_Y + 40, 5);
    BSP_LCD_SetColors(0xFFFF, 0xFFFF);
    BSP_LCD_FillCircle(RADIO_X, RADIO_Y, 5);
  }
#endif
}

/* When a tag is selected, clear tab before displaying information */
static void clearTab(void)
{
  Menu_SetStyle(CLEAR_PLAIN);
  Menu_FillRectangle(12, 64, 296, 139);
}

static void clearBody(void)
{
  Menu_SetStyle(CLEAR_PLAIN);
  Menu_FillRectangle(0, 22, 320, 196);
}

/* Helper method to define tag information position */
static uint16_t yLine(uint8_t l)
{
  return lineY[l];
}

/* Helper method to get the tag type as a string */
static char* getTypeStr(void)
{
  char* retPtr = NULL;
  uint8_t buffer[16];   /* Only for Felica check */
  
  switch(gDeviceList[deviceToDisplay].type)
  {
    case RFAL_NFC_LISTEN_TYPE_NFCA:
      switch (gDeviceList[deviceToDisplay].dev.nfca.type)
      {
        case RFAL_NFCA_T1T:
          retPtr = type1Str;
          break;
        
        case RFAL_NFCA_T2T:
          retPtr = type2Str;
          break;
        
        case RFAL_NFCA_T4T:
          if(BSP_NFCTAG_GetNdefSupport())
          {
            retPtr = type4aStr;
          }
          else
          {
            retPtr = iso14443aStr;
          }
          break;
        case RFAL_NFCA_NFCDEP:
        case RFAL_NFCA_T4T_NFCDEP:
          retPtr = nfcdep4aStr;
          break;
      }
      break;
      
    case RFAL_NFC_LISTEN_TYPE_NFCB:
      retPtr = type4bStr;
      break;
    
    case RFAL_NFC_LISTEN_TYPE_ST25TB:
      retPtr = st25tbStr;
      break;
    
    case RFAL_NFC_LISTEN_TYPE_NFCF:
      if(BSP_NFCTAG_ReadData(buffer,0,1) == ERR_NONE)
      {
        retPtr = type3Str;
      }
      else
      {
        retPtr = felicaStr;
      }
      break;
      
    case RFAL_NFC_LISTEN_TYPE_NFCV:
      if((!BSP_NFCTAG_GetNdefSupport()) || (BSP_NFCTAG_CheckVicinity()))
      {
        retPtr = iso15693Str;
      }
      else
      {
        retPtr = type5Str;
      }
      break;
      
    default:
      break;
  }
  return retPtr;
}

/* Control display when no tag is detected (instruction + radio button) */
static void tagDetectionNoTag(void)
{
#ifndef FREEZE_DISPLAY
  uint32_t line = 4;

  BSP_LCD_SetColors(0x0000, 0xFFFF);

  BSP_LCD_FillCircle(RADIO_X, RADIO_Y, 10);
  BSP_LCD_SetColors(0xFFFF, 0x0000);
  BSP_LCD_FillCircle(RADIO_X, RADIO_Y, 7);


  BSP_LCD_SetColors(0x0000, 0xFFFF);
  BSP_LCD_FillCircle(RADIO_X, (RADIO_Y + 40), 10);
  BSP_LCD_SetColors(0xFFFF, 0x0000);
  BSP_LCD_FillCircle(RADIO_X, (RADIO_Y + 40), 7);

  setRadio();

  Menu_SetStyle(PLAIN);
  BSP_LCD_DisplayStringAt((RADIO_X + 20), (RADIO_Y - 10), (uint8_t*)"Continuous Poll", LEFT_MODE);
  BSP_LCD_DisplayStringAt((RADIO_X + 20), (RADIO_Y + 30), (uint8_t*)"Wake-up", LEFT_MODE);
  
  line = 6;
  Menu_DisplayCenterString(line, "Place tags above");
  Menu_DisplayCenterString((line + 1), "the antenna...");
  instructionsDisplayed = true;
#endif
}

/* Helper method to get the current tag bitrate as numerical b/s */
static uint16_t getSpeedVal(rfalBitRate br)
{
  if(br <= RFAL_BR_848)
  {
    return 106 * (int)pow(2,br);
  }
  else if (br == RFAL_BR_52p97 )
  {
    return 52;
  }
  else if (br == RFAL_BR_26p48)
  {
    return 26;
  }
  else
  {
    return 00;
  }
}

/* Helper method to retrieve UID from a tag structure */
static bool getDevUid(rfalNfcDevice *devselected, uint8_t *uid, uint8_t *uidLen)
{
    switch (devselected->type)
    {
      case RFAL_NFC_LISTEN_TYPE_NFCA:
        memcpy(uid, devselected->dev.nfca.nfcId1, devselected->dev.nfca.nfcId1Len);
        *uidLen = devselected->dev.nfca.nfcId1Len;
        break;
      
      case RFAL_NFC_LISTEN_TYPE_NFCB:
        memcpy(uid, devselected->dev.nfcb.sensbRes.nfcid0, RFAL_NFCB_NFCID0_LEN);
        *uidLen = RFAL_NFCB_NFCID0_LEN;
        break;
      
      case RFAL_NFC_LISTEN_TYPE_ST25TB:
        memcpy(uid, devselected->dev.st25tb.UID, RFAL_ST25TB_UID_LEN);
        *uidLen = RFAL_ST25TB_UID_LEN;
        break;
      
      case RFAL_NFC_LISTEN_TYPE_NFCF:
        memcpy(uid, devselected->dev.nfcf.sensfRes.NFCID2, RFAL_NFCF_NFCID2_LEN);
        *uidLen = RFAL_NFCF_NFCID2_LEN;
        break;
      
      case RFAL_NFC_LISTEN_TYPE_NFCV:
        memcpy(uid, devselected->dev.nfcv.InvRes.UID, RFAL_NFCV_UID_LEN);
        *uidLen = RFAL_NFCV_UID_LEN;
        break;
      
      default:
        return false;
    }
  return true;
}

/* Helper method to get the current tag bitrate as a string */
static void getSpeedStr(char *retPtr)
{
  rfalGetBitRate(&txbr, &rxbr);

  sprintf(retPtr, "%d-%d kb/s", getSpeedVal(rxbr), getSpeedVal(txbr));
}

/* When a tag is slected, dispays the current DPO profile */
static void displayDpo(const rfalDpoEntry *currentDpo)
{
  const char *txt;
  uint16_t rfoX = 170;
  BSP_LCD_SetFont(&Font16);
  uint16_t rfoWidth = (10 * BSP_LCD_GetFont()->Width) + 20;
  uint16_t rfoTxtX;

  if(currentDpo != NULL)
  {
#ifdef ST25R3916B
    BSP_LCD_SetColors(BSP_LCD_FadeColor(LCD_COLOR_BLUEST, LCD_COLOR_LIGHTBLUE, 14, currentDpo->rfoRes), LCD_COLOR_WHITE);
    txt = (currentDpo->rfoRes == 0) ? "Full power" : "Low power ";
#else
    BSP_LCD_SetColors(BSP_LCD_FadeColor(LCD_COLOR_BLUEST, LCD_COLOR_LIGHTBLUE, 4, currentDpo->rfoRes), LCD_COLOR_WHITE);
    txt = dpoProfile[currentDpo->rfoRes];
#endif
    rfoTxtX = rfoX + ((rfoWidth - (strlen(txt) * BSP_LCD_GetFont()->Width)) / 2);

    BSP_LCD_DrawRectangleWithRoundCorner(rfoX,
                                         yLine(0) - 4 - 5,
                                         rfoWidth,
                                         BSP_LCD_GetFont()->Height + 14,
                                         10);
    BSP_LCD_DisplayStringAt(rfoTxtX, yLine(0), (uint8_t*)txt, LEFT_MODE);

    BSP_LCD_SetFont(&Font22);
    Menu_SetStyle(PLAIN);
  }
}

/* Callback function used to detect long latenncy in tag response, displays "Wait..." and avoid display freeze */
static uint8_t waitRssi(void)
{
  static uint8_t checkTouch = 0;
  Menu_Position_t touch;

  uint16_t rssiX = XMARGIN + (5 * BSP_LCD_GetFont()->Width);
  uint32_t delay = ((int32_t) HAL_GetTick() - (int32_t)timeRssi);
  if(delay > 1000)
  {
    BSP_LCD_DisplayStringAt(rssiX, yLine(RSSILINE), (uint8_t*)"Wait...  ", LEFT_MODE);
    lastRssi = 0;
  }
  if(checkTouch++ > 50)
  {
    checkTouch = 0;
    Menu_ReadPosition(&touch);
    if((touch.Sel) && (touch.Y > 60) )
    {
      // exit
      return 0;
    }
  }

  return 1;
}

/* Tag information Tab initial callback function */
static Menu_Callback_Status_t tabFirstTagReadInfo(void)
{
  char line[40] = "";
  char nb[4] = "";

  clearTab();
  uint8_t nbUidDigit = 8;

  char speedStr[20];
  // re-init lastRssi at dummy value != 0
  lastRssi = 1;
  BSP_NFCTAG_WaitingCb = &waitRssi;

  for(int d = 0; d < gDeviceList[deviceToDisplay].nfcidLen; d++)
  {
    if(gDeviceList[deviceToDisplay].type != RFAL_NFC_LISTEN_TYPE_NFCV)
    {
      sprintf(nb, "%02X:", gDeviceList[deviceToDisplay].nfcid[d]);
      strcat(line, nb);
    }
    else
    {
      // for type V invert UID
      sprintf(nb, "%02X:", gDeviceList[deviceToDisplay].nfcid[nbUidDigit - d - 1]);
      strcat(line, nb);
    }
  }
  // remove last colon 
  line[strlen(line)-1] = '\0';

  Menu_SetStyle(PLAIN);
  BSP_LCD_SetFont(&Font16);

  BSP_LCD_DisplayStringAt(XMARGIN,yLine(0), (uint8_t *)getTypeStr(), LEFT_MODE);

  BSP_LCD_DisplayStringAt(XMARGIN, yLine(UIDLINE), (uint8_t *)"UID:", LEFT_MODE);
  BSP_LCD_DisplayStringAt((XMARGIN + 1 * Menu_GetFontWidth()), yLine(UIDLINE + 1), (uint8_t *)line, LEFT_MODE);


  BSP_LCD_DisplayStringAt(XMARGIN, yLine(HBRLINE), (uint8_t *)"Speed:", LEFT_MODE);
  getSpeedStr(speedStr);
  BSP_LCD_DisplayStringAt((XMARGIN + 1 * Menu_GetFontWidth()), yLine(HBRLINE + 1), (uint8_t *)speedStr, LEFT_MODE);


  BSP_LCD_DisplayStringAt(XMARGIN, yLine(RSSILINE), (uint8_t *)"RSSI:", LEFT_MODE);

#ifdef ENABLE_TRUST25_SIGNATURE_VERIFICATION
  uint16_t trust25Line = 3;
  TruST25_Status_t TruST25_valid = TruST25_SignatureVerification(gDeviceList[deviceToDisplay].type, gDeviceList[deviceToDisplay].dev.nfca.type, gDeviceList[deviceToDisplay].nfcid, nbUidDigit);
  if(TruST25_valid == TRUST25_VALID)
  {
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
    BSP_LCD_DisplayPicture(170, yLine(trust25Line), Success);
    BSP_LCD_DisplayStringAt(205, yLine(trust25Line), (uint8_t *)"TruST25", LEFT_MODE);   
    BSP_LCD_DisplayStringAt(205, (yLine(trust25Line) + 16), (uint8_t *)"Validated", LEFT_MODE);   
    Menu_SetStyle(PLAIN);
  }
  else if (TruST25_valid == TRUST25_INVALID)
  {
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayPicture((170 + 10), yLine(trust25Line), Error);
    BSP_LCD_DisplayStringAt((205 + 10), yLine(trust25Line), (uint8_t *)"TruST25", LEFT_MODE);   
    BSP_LCD_DisplayStringAt((205 + 10), (yLine(trust25Line) + 16), (uint8_t *)"Error!", LEFT_MODE);   
    Menu_SetStyle(PLAIN);
  }
#endif

  BSP_LCD_SetFont(&Font22);

  return MENU_CALLBACK_CONTINUE;
}

/* Tag Information Tab callback: displays the RSSI & DPO profile */
static Menu_Callback_Status_t tabTagReadInfo(void)
{
  uint8_t buffer[4];
  uint16_t rssi = 0;
  uint16_t rssiLen = 0;
  uint16_t front,back;
  static uint16_t execCount = 0;
  uint8_t status;

  if(execCount++ > 100)
  {
    execCount = 0;
    return MENU_CALLBACK_CONTINUE;
  }
  BSP_LCD_SetFont(&Font16);
  uint16_t rssiX = XMARGIN + BSP_LCD_GetFont()->Width;
  uint16_t rssi_max = 280 - (2 * BSP_LCD_GetFont()->Width);


  timeRssi = HAL_GetTick();
  status = BSP_NFCTAG_ReadData(buffer, 0, 1);
  rfalGetTransceiveRSSI( &rssi );
  // with short frames, at High Bit Rate, rssi computation may not be relevant
  if((status == NFCTAG_OK) && (rssi == 0))
  {
    status = BSP_NFCTAG_ReadData(buffer, 0, 8);
    rfalGetTransceiveRSSI( &rssi );
  }
  if((status != NFCTAG_OK) && (status != NFCTAG_RESPONSE_ERROR))
  {
    // try to re-activate
    rfalFieldOff();
    HAL_Delay(5);
    timeRssi = HAL_GetTick();
    BSP_NFCTAG_Activate(&gDeviceList[deviceToDisplay]);
    rfalGetTransceiveRSSI( &rssi );
  }

  rssiLen = ceil ( (float)(rssi_max * rssi) / (float)0x550);
  rssiLen = rssiLen > rssi_max ? rssi_max : rssiLen;

  // don't update rssi display if rssi has not been measured
  if ((status == NFCTAG_OK) && (rssi == 0))
  {
    rssiLen = lastRssi;
  }


  Menu_SetStyle(PLAIN);
  if((rssiLen == 0) && (lastRssi != 0))
  {
    BSP_LCD_GetColors(&front, &back);
    BSP_LCD_SetColors(back, front);
    BSP_LCD_FillRect((rssiX + 6), (yLine(RSSIBARLINE) + 6), rssi_max, 2);
    BSP_LCD_SetColors(front, back);
    BSP_LCD_DisplayStringAt((rssiX + 4 * BSP_LCD_GetFont()->Width), yLine(RSSILINE), (uint8_t *)"Link lost", LEFT_MODE);
  }
  else if (rssiLen != 0)
  {
    if(lastRssi == 0)
    {
      BSP_LCD_DisplayStringAt((rssiX + 4 * BSP_LCD_GetFont()->Width), yLine(RSSILINE), (uint8_t *)"         ", LEFT_MODE);
    }
    char rssiTxt[50];
    sprintf(rssiTxt, "       ");
    BSP_LCD_DisplayStringAt((rssiX + 4 * BSP_LCD_GetFont()->Width), yLine(RSSILINE), (uint8_t *)rssiTxt, LEFT_MODE);

    BSP_LCD_GetColors(&front, &back);
    BSP_LCD_SetColors(back, front);
    BSP_LCD_FillRect((BSP_LCD_GetXSize() / 2 - rssi_max / 2), (yLine(RSSIBARLINE) + 6), (rssi_max / 2 - lastRssi / 2), 2);
    BSP_LCD_FillRect((BSP_LCD_GetXSize() / 2 + lastRssi / 2 + 6), (yLine(RSSIBARLINE) + 6), (rssi_max / 2 - lastRssi / 2), 2);
    BSP_LCD_SetColors(front, back);
    BSP_LCD_FillRect((BSP_LCD_GetXSize() / 2 - rssiLen / 2), (yLine(RSSIBARLINE) + 6), rssiLen, 2);
  }

  lastRssi = rssiLen;

  /* DPO */
  displayDpo(rfalDpoGetCurrentTableEntry());
 
  BSP_LCD_SetFont(&Font22);

  return MENU_CALLBACK_CONTINUE;
}

/* Tag content Tab initial callback: display NDEF tag content*/
static Menu_Callback_Status_t tabFirstTagReadNdef(void)
{
  char txt[50] = {0};
  uint8_t status;
  uint8_t* ndefBuffer = NULL;
  uint16_t ndefLen = 0;
  uint32_t rawMessageLen;
  ndefConstBuffer  bufRawMessage;
  ndefMessage ndefmessage;
  ndefInfo info;
  
  clearTab();

  Menu_SetStyle(PLAIN);
  BSP_LCD_SetFont(&Font16);

  // disable NFCTAG loop callback
  BSP_NFCTAG_WaitingCb = NULL;

  status = ndefPollerContextInitialization(&ndefctx, &gDeviceList[deviceToDisplay]);
  
  if(status == RFAL_ERR_NONE)
  {
    status = ndefPollerNdefDetect(&ndefctx, &info);
    if(status == RFAL_ERR_NONE)
    {
      ndefLen = info.areaLen;
    }
  }
  
  // reset start of NDEF buffer
  if(ndefLen > sizeof(NDEF_Buffer))
  {
    ndefBuffer = (uint8_t *)malloc(ndefLen + 2);
    if(ndefBuffer == NULL)
    {
      sprintf(txt, "NDEF is too big!");
      BSP_LCD_DisplayStringAt(0, 102, (uint8_t *)txt, CENTER_MODE);
      status = RFAL_ERR_NOMEM;
    }
  }
  else
  {
    ndefBuffer = NDEF_Buffer;
  }
  
  if(status == RFAL_ERR_NONE)
  {
    memset(ndefBuffer, 0, 100);
    
    status = ndefPollerReadRawMessage(&ndefctx, ndefBuffer, ndefLen, &rawMessageLen, true);
    
    bufRawMessage.buffer = ndefBuffer;
    bufRawMessage.length = rawMessageLen;
    
    status = ndefMessageDecode(&bufRawMessage, &ndefmessage);
  }
  
  if((status == RFAL_ERR_NONE) && ndefLen)
  {
    if(displayNdef(&ndefmessage) != RFAL_ERR_NONE)
    {
      char data_str[30] = {0};
      
      sprintf(txt, "Cannot identify NDEF record");
      Menu_SetStyle(PLAIN);
      BSP_LCD_SetFont(&Font16);
      BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)txt, CENTER_MODE);
      
      status = BSP_NFCTAG_ReadData(ndefBuffer, 0, 16);
      if(status == NFCTAG_OK)
      {
        for(int data_index = 0; data_index < 4 ; data_index++)
        {
          sprintf(data_str,"0x%02X, 0x%02X, 0x%02X, 0x%02X", ndefBuffer[data_index * 4],
                                                             ndefBuffer[data_index * 4 + 1],
                                                             ndefBuffer[data_index * 4 + 2],
                                                             ndefBuffer[data_index * 4 + 3]);
          BSP_LCD_DisplayStringAt(0,86 + (data_index * 16), (uint8_t *)data_str, CENTER_MODE);
        }
      }
      else
      {
        sprintf(txt, "Cannot read data");
        BSP_LCD_DisplayStringAt(0, 102, (uint8_t *)txt, CENTER_MODE);
      }
    }

  }
  else
  {
    char data_str[30] = {0};
    
    if(status == RFAL_ERR_NOMEM)
    {
      sprintf(txt, "NDEF is too big");      
    }
    else
    {
      sprintf(txt, "Cannot read NDEF");
    }
 
    status = BSP_NFCTAG_ReadData(NDEF_Buffer, 0, 16);

    Menu_SetStyle(PLAIN);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)txt, CENTER_MODE);

    if(status == NFCTAG_OK)
    {
      for(int data_index = 0; data_index < 4 ; data_index++)
      {
        sprintf(data_str,"0x%02X, 0x%02X, 0x%02X, 0x%02X", NDEF_Buffer[data_index * 4],
                                                           NDEF_Buffer[data_index * 4 + 1],
                                                           NDEF_Buffer[data_index * 4 + 2],
                                                           NDEF_Buffer[data_index * 4 + 3]);
        BSP_LCD_DisplayStringAt(0,86 + (data_index * 16), (uint8_t *)data_str, CENTER_MODE);
      }
    }
    else
    {
      sprintf(txt, "Cannot read data");
      BSP_LCD_DisplayStringAt(0, 102, (uint8_t *)txt, CENTER_MODE);
    }

    BSP_LCD_SetFont(&Font22);
  }

  // write ndef icon
  if(BSP_NFCTAG_GetNdefSupport())
  {
    BSP_LCD_DisplayPicture(260, 160, send_icon);
  }

  if((ndefBuffer != NDEF_Buffer) && (ndefBuffer != NULL))
  {
    free(ndefBuffer);
    ndefBuffer = NULL;
  }
  
  return MENU_CALLBACK_CONTINUE;
}

/* Tag content Tab callback: manage NDEF message write */
static Menu_Callback_Status_t tabTagReadNdef(void)
{
  ReturnCode err;
  uint8_t touchpress = 0;
  uint8_t joypress = 0;
  Menu_Position_t touch;
  Menu_Direction_t JoyState;
  ndefMessage message;
  ndefBuffer bufRawMessage;
  ndefConstBuffer bufUri;
  char ndefURI[] = "st.com/st25r3916-demo";
  ndefType uri;
  ndefRecord record;
  
  
  if(!BSP_NFCTAG_GetNdefSupport())
    return MENU_CALLBACK_CONTINUE;

  touchpress = Menu_ReadPosition(&touch);
  
  if (BSP_PB_GetState(JOYSTICK_DOWN)) 
  { /* Check only DOWN button to avoid conflict with Menu Tab LEFT RIGHT joystick detection */
    JoyState.Dir = MENU_DOWN;
    joypress = 1;
    while(BSP_PB_GetState(JOYSTICK_DOWN));
  }

  if(touchpress || joypress)
  {
    if(((touch.X > 250) && (touch.Y > 150) && (touchpress != 0)) || ((JoyState.Dir == MENU_DOWN) && (joypress != 0)))
    {
      err  = ndefMessageInit(&message);  /* Initialize message structure */
      bufUri.buffer = (uint8_t *)ndefURI;
      bufUri.length = strlen(ndefURI);
      err |= ndefRtdUriInit(&uri, NDEF_URI_PREFIX_HTTP_WWW, &bufUri); /* Initialize URI type structure */
      err |= ndefRtdUriToRecord(&uri, &record); /* Encode URI Record */

      err |= ndefMessageAppend(&message, &record); /* Append URI to message */

      bufRawMessage.buffer = NDEF_Buffer;
      bufRawMessage.length = sizeof(NDEF_Buffer);
      err |= ndefMessageEncode(&message, &bufRawMessage); /* Encode the message to the raw buffer */
      err |= ndefPollerWriteRawMessage(&ndefctx, bufRawMessage.buffer, bufRawMessage.length);
      
      if(err == RFAL_ERR_NONE)
      {
        clearTab();
        Menu_SetStyle(PLAIN);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"NDEF write success!", CENTER_MODE);
        HAL_Delay(500);
        tabFirstTagReadNdef();
      }
      else
      {
        clearTab();
        Menu_SetStyle(PLAIN);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"NDEF write error!", CENTER_MODE);
        HAL_Delay(500);        
      }
      BSP_LCD_SetFont(&Font22);
    }
  }
  return MENU_CALLBACK_CONTINUE;
}

/* Helper function to manage user touchscreen interface */
static void manageTagDisplay(uint8_t devId)
{
  int status;
  
  status = BSP_NFCTAG_Activate(&gDeviceList[devId]);
  
  if (status == NFCTAG_OK)
  {
    Menu_MsgStatus("Tag read", "", MSG_INFO);
#if defined(USE_JOYSTICKONLY)
    Menu_DisplayCenterString(10, "PB Blue to exit");
#else
    Menu_DisplayCenterString(10, "Touch to exit");
#endif /* USE_JOYSTICKONLY */
    Menu_TabLoop(tabSetup, (sizeof(tabSetup) / sizeof(Menu_Tab_Setup_t)));

    curTagSelected = 0;
    gDevNb = 0;

    BSP_LCD_SetFont(&Font22);
    clearBody();
    Menu_Delay(200);
  }
}

/* Helper function to manage user touchscreen interface */
static uint8_t manageInput(void)
{
  Menu_Position_t touch;
  Menu_Direction_t JoyState;
  uint8_t ButtonState = 0;

  if(Menu_ReadPosition(&touch))
  {
    if(touch.Y > 215)
    {
      // make sure the Wakeup mode is deactivated
      rfalWakeUpModeStop();
      // switch the field off before leaving the demo
      rfalFieldOff();
      return 1;
    }
    else if ((instructionsDisplayed) && (gDevNb == 0))
    {
      if(touch.Y < 60)
      {
        if(detectMode != DETECT_MODE_POLL)
        {
          detectMode = DETECT_MODE_POLL;
          discParam.wakeupEnabled = false;    /* enable/disable wakeup */
          state = DEMO_ST_START_DISCOVERY;                       /* restart loop          */
          platformLog("Toggling Wake Up mode %s\r\n", discParam.wakeupEnabled ? "ON": "OFF");

          /* Re-start discovery with new Wake-up configuration */
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          rfalNfcDiscover( &discParam );
          setRadio();
        }
      }
      else
      {
        if(detectMode != DETECT_MODE_WAKEUP)
        {
          detectMode = DETECT_MODE_WAKEUP;
          discParam.wakeupEnabled = true;    /* enable/disable wakeup */
          state = DEMO_ST_START_DISCOVERY;                       /* restart loop          */
          platformLog("Toggling Wake Up mode %s\r\n", discParam.wakeupEnabled ? "ON": "OFF");

          /* Re-start discovery with new Wake-up configuration */
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          rfalNfcDiscover( &discParam );
          setRadio();
        }
      }
    }
    else
    {
      uint8_t devId = 0xFF;
      // tags are in the field
      if((touch.Y < getTagBoxY(1)) && (gDevNb >= 1))
      {
        devId = 0;
      }
      else if ((touch.Y < getTagBoxY(2)) && (gDevNb >= 2))
      {
        devId = 1;
      }
      else if ((touch.Y < getTagBoxY(3)) && (gDevNb >= 3))
      {
        devId = 2;
      }
      else if (gDevNb >= 4)
      {
        devId = 3;
      }
      
      if(devId != 0xFF)
      {
        deviceToDisplay = devId;
        nfcSelectedDevice.type = gDeviceList[devId].type;
        getDevUid(&gDeviceList[devId], nfcSelectedDevice.nfcid, &nfcSelectedDevice.nfcidLen);
      }
    }
  }
  
  if(Menu_ReadDirection(&JoyState))
  {
    if ((instructionsDisplayed) && (gDevNb == 0))
    {
      if((JoyState.Dir == MENU_UP) || (JoyState.Dir == MENU_DOWN))
      {
        if(detectMode != DETECT_MODE_POLL)
        {
          detectMode = DETECT_MODE_POLL;
          discParam.wakeupEnabled = false;    /* enable/disable wakeup */
          state = DEMO_ST_START_DISCOVERY;                       /* restart loop          */
          platformLog("Toggling Wake Up mode %s\r\n", discParam.wakeupEnabled ? "ON": "OFF");

          /* Re-start discovery with new Wake-up configuration */
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          rfalNfcDiscover( &discParam );
          setRadio();
        }
        else
        {
          detectMode = DETECT_MODE_WAKEUP;
          discParam.wakeupEnabled = true;    /* enable/disable wakeup */
          state = DEMO_ST_START_DISCOVERY;                       /* restart loop          */
          platformLog("Toggling Wake Up mode %s\r\n", discParam.wakeupEnabled ? "ON": "OFF");

          /* Re-start discovery with new Wake-up configuration */
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          rfalNfcDiscover( &discParam );
          setRadio();
        }
      }
    }
    else
    {
      if(JoyState.Dir == MENU_UP)
      {
        if(curTagSelected > 1)
        {
          curTagSelected--;
        }
      }
      if(JoyState.Dir == MENU_DOWN)
      {
        if((curTagSelected < gDevNb) && (gDevNb != 0))
        {
          curTagSelected++;
        }
      }
      if(JoyState.Sel != 0)
      {
        uint8_t devId = 0xFF;
        // tags are in the field
        if((curTagSelected == 1) && (gDevNb >= 1))
        {
          devId = 0;
        }
        else if ((curTagSelected == 2) && (gDevNb >= 2))
        {
          devId = 1;
        }
        else if ((curTagSelected == 3) && (gDevNb >= 3))
        {
          devId = 2;
        }
        else if (gDevNb >= 4)
        {
          devId = 3;
        }
        
        if(devId != 0xFF)
        {
          deviceToDisplay = devId;
          nfcSelectedDevice.type = gDeviceList[devId].type;
          getDevUid(&gDeviceList[devId], nfcSelectedDevice.nfcid, &nfcSelectedDevice.nfcidLen);
        }
      }
    }
  }
  
  if(Menu_ReadSelection(&ButtonState))
  {
    if(ButtonState != 0)
    {
      // make sure the Wakeup mode is deactivated
      rfalWakeUpModeStop();
      // switch the field off before leaving the demo
      rfalFieldOff();
      
      return 1;
    }
    else if ((instructionsDisplayed) && (gDevNb == 0))
    {
      if(touch.Y < 60)
      {
        if(detectMode != DETECT_MODE_POLL)
        {
          detectMode = DETECT_MODE_POLL;
          discParam.wakeupEnabled = false;    /* enable/disable wakeup */
          state = DEMO_ST_START_DISCOVERY;                       /* restart loop          */
          platformLog("Toggling Wake Up mode %s\r\n", discParam.wakeupEnabled ? "ON": "OFF");

          /* Re-start discovery with new Wake-up configuration */
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          rfalNfcDiscover( &discParam );
          setRadio();
        }
      }
      else
      {
        if(detectMode != DETECT_MODE_WAKEUP)
        {
          detectMode = DETECT_MODE_WAKEUP;
          discParam.wakeupEnabled = true;    /* enable/disable wakeup */
          state = DEMO_ST_START_DISCOVERY;                       /* restart loop          */
          platformLog("Toggling Wake Up mode %s\r\n", discParam.wakeupEnabled ? "ON": "OFF");

          /* Re-start discovery with new Wake-up configuration */
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          rfalNfcDiscover( &discParam );
          setRadio();
        }
      }
    }
  }
  return 0;
}

static void UpdateSelector(void)
{
  Point arrowpoints[3];
  for(uint8_t index = 0; index < 4; index++)
  {
    arrowpoints[0].X = 2;
    arrowpoints[0].Y = 21 + getTagBoxY(index);
    arrowpoints[1].X = 2;
    arrowpoints[1].Y = 29 + getTagBoxY(index);
    arrowpoints[2].X = 5;
    arrowpoints[2].Y = 25 + getTagBoxY(index);

    if(curTagSelected == index + 1)
    {
      if(gDeviceList[index].type == RFAL_NFC_LISTEN_TYPE_NFCA)
      {
        Menu_SetStyle(NFCA);
      }
      else if(gDeviceList[index].type == RFAL_NFC_LISTEN_TYPE_NFCB)
      {
        Menu_SetStyle(NFCB);
      }
      else if(gDeviceList[index].type == RFAL_NFC_LISTEN_TYPE_NFCF)
      {
        Menu_SetStyle(NFCF);
      }
      else if(gDeviceList[index].type == RFAL_NFC_LISTEN_TYPE_ST25TB)
      {
        Menu_SetStyle(NFCB);
      }
      else
      {
        Menu_SetStyle(NFCV);
      }
      BSP_LCD_FillPolygon(arrowpoints, 3);
    }
    else
    {
      Menu_SetStyle(CLEAR_TAB);
      BSP_LCD_FillPolygon(arrowpoints, 3);
    }
  }
}

static void UpdateDisplay(uint8_t *PrevDevNb, uint8_t *PrevCurTagSelected)
{ 
  /* Update display only when a Tag appear/disappear */
  if(*PrevDevNb != gDevNb)
  {
    /* Clear screen before update */
    clearBody();
    
    /* if 1 or more tags were removed, change the cursor position if necessary */
    if(curTagSelected > gDevNb)
    {
      curTagSelected = gDevNb;
    }
    
    *PrevDevNb = gDevNb;
    
    /* Detect which tag type to display */
    for(int i = 0; i < gDevNb; i++)
    {
      if(gDeviceList[i].type == RFAL_NFC_LISTEN_TYPE_NFCA)
      {
        Menu_SetStyle(NFCA);
        displayTag(i, gTypeStrList[gDeviceList[i].type], gDeviceList[i].dev.nfca.nfcId1);
      }
      else if(gDeviceList[i].type == RFAL_NFC_LISTEN_TYPE_NFCB)
      {
        Menu_SetStyle(NFCB);
        displayTag(i, gTypeStrList[gDeviceList[i].type], gDeviceList[i].dev.nfcb.sensbRes.nfcid0);
      }
      else if(gDeviceList[i].type == RFAL_NFC_LISTEN_TYPE_NFCF)
      {
        Menu_SetStyle(NFCF);
        displayTag(i, gTypeStrList[gDeviceList[i].type], gDeviceList[i].dev.nfcf.sensfRes.NFCID2);
      }
      else if(gDeviceList[i].type == RFAL_NFC_LISTEN_TYPE_NFCV)
      {
        uint8_t devUID[RFAL_NFCV_UID_LEN];
              
        ST_MEMCPY( devUID, gDeviceList[i].dev.nfcv.InvRes.UID, RFAL_NFCV_UID_LEN);   /* Copy the UID into local var */
        REVERSE_BYTES( devUID, RFAL_NFCV_UID_LEN );                 /* Reverse the UID for display purposes */
        
        Menu_SetStyle(NFCV);
        displayTag(i, gTypeStrList[gDeviceList[i].type], devUID);
      }
      else if(gDeviceList[i].type == RFAL_NFC_LISTEN_TYPE_ST25TB)
      {
        Menu_SetStyle(NFCB);
        displayTag(i, gTypeStrList[gDeviceList[i].type], gDeviceList[i].dev.st25tb.UID);
      }
      if(detectMode == DETECT_MODE_WAKEUP)
      {
        if( discParam.wakeupEnabled == true )
        {
          discParam.wakeupEnabled = false;
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          state = DEMO_ST_START_DISCOVERY;
        }
      }
    }
    
    /* All tags removed, reset display. Otherwise update selector position */
    if(gDevNb == 0)
    {
      curTagSelected = 0;
      tagDetectionNoTag();
      if(detectMode == DETECT_MODE_WAKEUP)
      {
        if( discParam.wakeupEnabled == false )
        {
          discParam.wakeupEnabled = true;
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
          state = DEMO_ST_START_DISCOVERY;
        }
      }
    }
    else
    {
      UpdateSelector();
    }
  }
  
  /* Selector position has changed, update display */
  if(curTagSelected != *PrevCurTagSelected)
  {
    *PrevCurTagSelected = curTagSelected;
    UpdateSelector();
  }
}

/*!
 *****************************************************************************
 * \brief Demo Notification
 *
 *  This function receives the event notifications from RFAL
 *  in multitag detection.
 *****************************************************************************
 */
static void demoNotif( rfalNfcState st )
{
    if((st != 2) && (st != 10))
    {
      platformLog("state = %d\r\n", st );
    }
    if( st == RFAL_NFC_STATE_WAKEUP_MODE )
    {
      platformLog("Wake Up mode started \r\n");
    }
    else if( st == RFAL_NFC_STATE_POLL_TECHDETECT )
    {
      if( discParam.wakeupEnabled )
      {
        platformLog("Wake Up mode terminated. Polling for devices \r\n");
      }
    }
    else if( st == RFAL_NFC_STATE_POLL_SELECT )
    {
      if(deviceToDisplay == 0xFF)
      { /* In multitag type detection, no need to select tag */
        if(curTagSelected == 0)
        {
          curTagSelected++;
        }
      
        rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_DISCOVERY );
      }
      else
      { /* In simple type detection, user selected the device to activate and multiple tags detected */
        rfalNfcGetDevicesFound( &gDeviceList, &gDevNb );
        /* Check if in case of multiple devices, selection is already attempted */
        if( (multiSel < gDevNb) )
        {
          uint8_t uid[10];
          uint8_t uidlen = 0;
          
          /* Multiple devices were found, check which one to activate, selected by user */
          while(multiSel < gDevNb)
          {
            getDevUid(&gDeviceList[multiSel], uid, &uidlen);
            if(memcmp(nfcSelectedDevice.nfcid, uid, uidlen) == 0)
            {
              rfalNfcSelect( multiSel );
              deviceToDisplay = multiSel;
            }
            multiSel++;
          }
        }
        else
        {
          rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_DISCOVERY );
        }
      }
    }
    else if( st == RFAL_NFC_STATE_START_DISCOVERY )
    {
      multiSel = 0;
      state = DEMO_ST_START_DISCOVERY;
    }
    else if( st == RFAL_NFC_STATE_ACTIVATED )
    {
      if(curTagSelected == 0)
      { /* Device is activated, position the cursor if it has not already be done */
        curTagSelected++;
      }
      
      if((multiSel == 0) && (deviceToDisplay != 0xFF))
      { /* Device activated, only 1 tag of this type detected,
         * no RFAL_NFC_STATE_POLL_SELECT state triggered, select this tag to display info */
        deviceToDisplay = 0;
      }
    }
}

/*
******************************************************************************
* LOCAL FUNCTION
******************************************************************************
*/
void exampleRfalPollerIni(void)
{
  rfalNfcDefaultDiscParams( &discParam );
        
  discParam.devLimit      = EXAMPLE_RFAL_POLLER_DEVICES;
  
  discParam.notifyCb      = demoNotif;
  discParam.totalDuration = 500U;

  discParam.wakeupConfigDefault = true;
  
  discParam.techs2Find = RFAL_NFC_TECH_NONE;
  discParam.techs2Find |= RFAL_NFC_POLL_TECH_A;
  discParam.techs2Find |= RFAL_NFC_POLL_TECH_B;
  discParam.techs2Find |= RFAL_NFC_POLL_TECH_F;
  discParam.techs2Find |= RFAL_NFC_POLL_TECH_V;
  discParam.techs2Find |= RFAL_NFC_POLL_TECH_ST25TB;

  rfalNfcDiscover( &discParam );

  state = DEMO_ST_START_DISCOVERY;
}

/** @brief Tag detection demo.
  *        Detect and display tags in the field.
  *        The user may select a detected tag to display tag details & content.
  */
void tagDetectionDemo(void)
{
  exampleRfalPollerIni();
  
  Menu_MsgStatus("Tag detection", "", MSG_INFO);
  Menu_SetStyle(CLEAR_PLAIN);
#if defined(USE_JOYSTICKONLY)
  Menu_DisplayCenterString(10, "PB Blue to exit");
#else
  Menu_DisplayCenterString(10, "Touch to exit");
#endif /* USE_JOYSTICKONLY */
  instructionsDisplayed = false;

  detectMode = DETECT_MODE_POLL;
  
  tagDetectionNoTag();

  exampleRfalPollerRun();
}

/*!
 ******************************************************************************
 * \brief Passive Poller Run
 * 
 * This method implements the main state machine going thought all the 
 * different activities that a Reader/Poller device (PCD) needs to perform.
 * 
 * 
 ******************************************************************************
 */
void exampleRfalPollerRun( void )
{
  bool exitloop = false;
  uint8_t prevdevnb = 0;
  uint8_t prevcurtagselected = 0;
  
  gDevNb = 0;
  curTagSelected = 0;
  
  while(exitloop == false)
  {
    rfalNfcWorker();                                    /* Run RFAL worker periodically */

    rfalNfcGetDevicesFound( &gDeviceList, &gDevNb );
    
    switch( state )
    {
        /*******************************************************************************/
        case DEMO_ST_START_DISCOVERY:
        
          multiSel = 0;
          if(deviceToDisplay == 0xFF)
          { /* Starting multitag type detection */
            discParam.techs2Find = RFAL_NFC_POLL_TECH_A | RFAL_NFC_POLL_TECH_B | RFAL_NFC_POLL_TECH_F | RFAL_NFC_POLL_TECH_V | RFAL_NFC_POLL_TECH_ST25TB;
            discParam.totalDuration = 500U;

            rfalNfcDiscover( &discParam );
            state = DEMO_ST_DISCOVERY;
          }
          else
          { /* Otherwise, start a simple tag type detection */
            switch(gDeviceList[deviceToDisplay].type)
            {
              case RFAL_NFC_LISTEN_TYPE_NFCA:
                discParam.techs2Find = RFAL_NFC_POLL_TECH_A;
                break;
              
              case RFAL_NFC_LISTEN_TYPE_NFCB:
                discParam.techs2Find = RFAL_NFC_POLL_TECH_B;
                break;
              
              case RFAL_NFC_LISTEN_TYPE_NFCF:
                discParam.techs2Find = RFAL_NFC_POLL_TECH_F;
                break;
              
              case RFAL_NFC_LISTEN_TYPE_NFCV:
                discParam.techs2Find = RFAL_NFC_POLL_TECH_V;
                break;
              
              case RFAL_NFC_LISTEN_TYPE_ST25TB:
                discParam.techs2Find = RFAL_NFC_POLL_TECH_ST25TB;
                break;
              
              default:
                break;
            }
            discParam.totalDuration = 300U;
            discParam.wakeupEnabled = false;
            rfalNfcDiscover( &discParam );
            state = DEMO_ST_TAG_OPERATION;
          }
          break;

        /*******************************************************************************/
        case DEMO_ST_DISCOVERY:
            if(deviceToDisplay == 0xFF)
            { /* Displays all tags detected */
              if( rfalNfcIsDevActivated( rfalNfcGetState() ) )
              {
                rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_DISCOVERY );
                state = DEMO_ST_START_DISCOVERY;
              }

              UpdateDisplay(&prevdevnb, &prevcurtagselected);
            }
            else
            { /* Device selected, restart discovery with simple type detection */
              rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
              state = DEMO_ST_START_DISCOVERY;
            }
            break;
            
        /*******************************************************************************/    
        case DEMO_ST_TAG_OPERATION:
            if( rfalNfcIsDevActivated( rfalNfcGetState() ) )
            { /* Display tag's information when device is activated */
              manageTagDisplay(deviceToDisplay);
              
              deviceToDisplay = 0xFF;
              rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
              state = DEMO_ST_START_DISCOVERY;
            }
            break;

        /*******************************************************************************/
        case DEMO_ST_NOTINIT:
        default:
            break;
    }
    if(manageInput())
    {
      rfalNfcDeactivate( RFAL_NFC_DEACTIVATE_IDLE );
      exitloop = true;
    }
  }
}
