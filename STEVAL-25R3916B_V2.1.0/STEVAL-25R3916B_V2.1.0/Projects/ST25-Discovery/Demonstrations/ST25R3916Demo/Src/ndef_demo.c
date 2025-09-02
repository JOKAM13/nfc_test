/**
  ******************************************************************************
  * @file    ndef_demo.c
  * @author  MMY Application Team
  * @ingroup ST25R3916_Discovery_Demo
  * @brief   NDEF display helper methods
  ******************************************************************************
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
  ******************************************************************************
  */
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "common.h"
#include "Menu_config.h"
#include "st25_discovery_st25r.h"

#include "ndef_message.h"
#include "ndef_record.h"
#include "ndef_types.h"
#include "ndef_type_uri.h"
#include "ndef_type_vcard.h"

#include <inttypes.h>

/* Reuse pictures */
extern const char nmark_64x64[];
extern const char cUrl[];
extern const char cVcard[];
extern const char cEmail[];

uint8_t NDEF_Buffer[NFC_DEVICE_MAX_NDEFMEMORY];

/* Method to display a URI NDEF message */
static void displayNdefUri(ndefType *uri, uint32_t maxChar)
{
  char txt[500] = {0};
  ndefConstBuffer bufprot;
  ndefConstBuffer bufuristring;
  
  ndefGetRtdUri(uri, &bufprot, &bufuristring);
  
  strncat(txt, (char *)bufprot.buffer, bufprot.length);
  Menu_DisplayPicture(126, 70, cUrl);
  if(bufuristring.length < 60)
  {
    strncat(txt, (char *)bufuristring.buffer, bufuristring.length);
  } else {
    strncat(txt, (char *)bufuristring.buffer, 60);
    strcat(txt, "...");
  }
  if(strlen(txt) <= maxChar)
  {
    BSP_LCD_DisplayStringAt(0, 140, (uint8_t*)txt, CENTER_MODE);
  }
  else
  {
    Menu_DisplayLongString(20, 140, txt, maxChar);
  }
}

/* Method to display a vCard NDEF message */
void displayNdefVCard(ndefType *vcardtype, uint32_t maxChar)
{
  uint8_t type[20] = "";
  uint16_t y_label = 86;
  ndefStatus status;
  ndefTypeVCard vcard;
  ndefConstBuffer typestring;
  ndefConstBuffer bufPayload;
  ndefConstBuffer bufPayloadtype;
  ndefConstBuffer bufPayloadsubtype;
  ndefConstBuffer bufPayloadvalue;
  
  typestring.buffer = type;
  ndefGetVCard(vcardtype, &vcard);
  
  strcpy((char *)typestring.buffer, "FN");
  typestring.length = strlen((char *)typestring.buffer);
  status = ndefVCardGetProperty(&vcard, &typestring, &bufPayload);
  ndefVCardParseProperty(&bufPayload, &bufPayloadtype, &bufPayloadsubtype, &bufPayloadvalue);
  
  if((bufPayloadvalue.length) && (status == ERR_NONE))
  {
    uint8_t text[bufPayloadvalue.length + 1];
    
    memcpy(text, bufPayloadvalue.buffer, bufPayloadvalue.length);
    text[bufPayloadvalue.length] = '\0';
    BSP_LCD_DisplayStringAt(100, y_label, text, LEFT_MODE);
    y_label += 16;
  }
  else
  {
    strcpy((char *)typestring.buffer, "NAME");
    typestring.length = strlen((char *)typestring.buffer);
    status = ndefVCardGetProperty(&vcard, &typestring, &bufPayload);
    ndefVCardParseProperty(&bufPayload, &bufPayloadtype, &bufPayloadsubtype, &bufPayloadvalue);
    if((bufPayloadvalue.length) && (status == ERR_NONE))
    {
      uint8_t text[bufPayloadvalue.length + 1];
    
      memcpy(text, bufPayloadvalue.buffer, bufPayloadvalue.length);
      text[bufPayloadvalue.length] = '\0';
      BSP_LCD_DisplayStringAt(100, y_label, text, LEFT_MODE);
      y_label += 16;
    }
  }
    
  strcpy((char *)typestring.buffer, "ORG");
  typestring.length = strlen((char *)typestring.buffer);
  status = ndefVCardGetProperty(&vcard, &typestring, &bufPayload);
  ndefVCardParseProperty(&bufPayload, &bufPayloadtype, &bufPayloadsubtype, &bufPayloadvalue);
  
  if((bufPayloadvalue.length) && (status == ERR_NONE))
  {
    uint8_t text[bufPayloadvalue.length + 1];
    
    memcpy(text, bufPayloadvalue.buffer, bufPayloadvalue.length);
    text[bufPayloadvalue.length] = '\0';
    BSP_LCD_DisplayStringAt(100, y_label, text, LEFT_MODE);
    y_label += 16;
  }
  
  strcpy((char *)typestring.buffer, "TEL");
  typestring.length = strlen((char *)typestring.buffer);
  status = ndefVCardGetProperty(&vcard, &typestring, &bufPayload);
  ndefVCardParseProperty(&bufPayload, &bufPayloadtype, &bufPayloadsubtype, &bufPayloadvalue);
  
  if((bufPayloadvalue.length) && (status == ERR_NONE))
  {
    uint8_t text[bufPayloadvalue.length + 1];
    
    memcpy(text, bufPayloadvalue.buffer, bufPayloadvalue.length);
    text[bufPayloadvalue.length] = '\0';
    BSP_LCD_DisplayStringAt(100, y_label, text, LEFT_MODE);
    y_label += 16;
  }

  y_label = 140;
  
  strcpy((char *)typestring.buffer, "EMAIL");
  typestring.length = strlen((char *)typestring.buffer);
  status = ndefVCardGetProperty(&vcard, &typestring, &bufPayload);
  ndefVCardParseProperty(&bufPayload, &bufPayloadtype, &bufPayloadsubtype, &bufPayloadvalue);
  
  if((bufPayloadvalue.length) && (status == ERR_NONE))
  {
    uint8_t text[bufPayloadvalue.length + 1];
    
    memcpy(text, bufPayloadvalue.buffer, bufPayloadvalue.length);
    text[bufPayloadvalue.length] = '\0';
    BSP_LCD_DisplayStringAt(100, y_label, text, LEFT_MODE);
    y_label += 16;
  }
  
  strcpy((char *)typestring.buffer, "EMAIL");
  typestring.length = strlen((char *)typestring.buffer);
  status = ndefVCardGetProperty(&vcard, &typestring, &bufPayload);
  ndefVCardParseProperty(&bufPayload, &bufPayloadtype, &bufPayloadsubtype, &bufPayloadvalue);
  
  if((bufPayloadvalue.length) && (status == ERR_NONE))
  {
    uint8_t text[bufPayloadvalue.length + 1];
    
    memcpy(text, bufPayloadvalue.buffer, bufPayloadvalue.length);
    text[bufPayloadvalue.length] = '\0';
    y_label += Menu_DisplayLongString(20, y_label, (char *)text, maxChar) * 16;
    BSP_LCD_DisplayPicture(20, 70, cVcard);
  }
}

/* Method to display an email NDEF message */
void displayNdefEmail(ndefType *emailtype)
{
  
  uint16_t y_label = 70;
  uint16_t x_label = 90;
  ndefConstBuffer emailprot;
  ndefConstBuffer emailstring;
  
  Menu_SetStyle(GREY);
  
  ndefGetRtdUri(emailtype, &emailprot, &emailstring);
  
  if(!strcmp((const char *)emailprot.buffer, "mailto:") )
  {
    BSP_LCD_DisplayStringAt(x_label, y_label, (uint8_t *)"to:", LEFT_MODE);
    
    Menu_SetStyle(PLAIN);
    x_label = 90 + strlen("to:") * BSP_LCD_GetFont()->Width;
    y_label += Menu_DisplayLongString(x_label, y_label, (char *)emailstring.buffer, (300 - x_label) / BSP_LCD_GetFont()->Width) * 16;
    
    Menu_SetStyle(GREY);
    x_label = 90;
    
    BSP_LCD_DisplayStringAt(x_label, y_label, (uint8_t *)"sub:", LEFT_MODE);
    
    Menu_SetStyle(PLAIN);
    x_label = 90 + strlen("sub:") * BSP_LCD_GetFont()->Width;
    y_label += Menu_DisplayLongString(x_label, y_label, (char *)emailstring.buffer, (300 - x_label) / BSP_LCD_GetFont()->Width) * 16;
    y_label = (y_label > 140) ? y_label : 140;
    y_label += Menu_DisplayLongString(20, y_label, (char *)emailstring.buffer, 300 / BSP_LCD_GetFont()->Width) * 16;
    BSP_LCD_DisplayPicture(20, 70, cEmail);
  }
  else
  {
    char msg[80];
    strcat (msg, "!URI:\n");
    strncat(msg,(const char *)emailprot.buffer, emailprot.length);
    strncat(msg,(const char *)emailstring.buffer, emailstring.length);
    Menu_DisplayLongString(20, y_label, msg, 300 / BSP_LCD_GetFont()->Width);
  }
}

/* Method to display any other than previous NDEF message */
void displayNdefGeneric(ndefRecord* record)
{
  // Other record
  char type[41] = {0};
  char txt[60] = {0};
  uint8_t tnf;
  ndefConstBuffer8 buftype;
  
  ndefRecordGetType(record, &tnf, &buftype);
  memcpy(type, buftype.buffer, ((buftype.length > 40) ? 40 : buftype.length));
  Menu_DisplayPicture(240, 70, nmark_64x64);

  sprintf(txt, "Length: %"PRIu32" Bytes", record->bufPayload.length);
  BSP_LCD_DisplayStringAt(20, 86, (uint8_t *)txt, LEFT_MODE);
  sprintf(txt, "Flag:   0x%02X", record->header);
  BSP_LCD_DisplayStringAt(20, 102, (uint8_t *)txt, LEFT_MODE);
  uint32_t smallTypeLen = ((300 - 64) / BSP_LCD_GetFont()->Width ) - 8;
  if(record->typeLength < smallTypeLen)
  {
    sprintf(txt, "Type:   %s", type);
    BSP_LCD_DisplayStringAt(20, 118, (uint8_t *)txt, LEFT_MODE);
  } 
  else
  {
    BSP_LCD_DisplayStringAt(20, 118, (uint8_t *)"Type: ", LEFT_MODE);
    Menu_DisplayLongString(20, 134, type, 300 / BSP_LCD_GetFont()->Width);
  }

}

/**
  * @brief Helper method to display a NDEF message on demo screen.
  * @param ndefBuffer The buffer containing an ndef message.
  * @returns status 0 is success, other values are errors.
  */
uint8_t displayNdef(ndefMessage *ndefmessage)
{
  uint8_t status = RFAL_ERR_NONE;
  ndefRecord *record = NULL;
  ndefType type;
  
  Menu_SetStyle(PLAIN);
  BSP_LCD_SetFont(&Font16);

  record = ndefMessageGetFirstRecord(ndefmessage);
  
  status = ndefRecordToType(record, &type);
  if(status == ERR_NONE)
  {
    uint32_t maxChar = 270 / Menu_GetFontWidth();

    if (type.id == NDEF_TYPE_ID_RTD_URI)
    {
      if((type.data.uri.protocol > 0) && (type.data.uri.protocol < 5))
      {
        displayNdefUri(&type, maxChar);
      }
      else if(type.data.uri.protocol == 6)
      {
        displayNdefEmail(&type);
      }
      else
      {
        displayNdefGeneric(record);
      }
    }
    else if(type.id == NDEF_TYPE_ID_MEDIA_VCARD)
    {
      displayNdefVCard(&type, maxChar);
    }
    else
    {
      displayNdefGeneric(record);
    }
  }
  BSP_LCD_SetFont(&Font22);
  return status;
}
