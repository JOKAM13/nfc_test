/**
  ******************************************************************************
  * @file    Menu_definition.c
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @ingroup ST25DV_Menu_Definition
  * @brief   This file defines the content of the menu for the ST25DV demo.
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

#include "main.h"
#include "Menu_core.h"
#include "Menu_config.h"
#include "Menu_definition.h"
#include "stdbool.h"

void ReturnFunc(void);

// Dummy definition of demo functions
void IdleFunc(void){};

/* icon main menu */
#include "aat_84x84_jpg.h"
#include "tag_detection_84x84_jpg.h"
#include "emulate_84x84_jpg.h"
#include "P2P_84x84_jpg.h"

/* icon file for menu */
#include "url_64x64_jpg.h"
#include "vcard_64x64_jpg.h"
#include "nmark_64x64_jpg.h"
#include "email_64x64_jpg.h"
#include "Contact_jpg.h"
#include "mailbox_jpg.h"

/** @defgroup ST25R3916_Menu_Definition ST25R3916 Menu Definition
  * @ingroup  ST25_Discovery_Demo
  * @brief    This module defines the structure and content of the ST25R3916 demo menu.
  * @details  Menu structure is statically defined in the module, and complies with the expected structure of the menu_demo middleware.
  *           Call Menu_Start() to start the menu main loop.
  * @{
  */
  
/** @brief   List NDEF icon address, null if nothing to display
  * @details The order in the list defines the position of the icons in the NDEF menu.
  */


static const char * ceMenuIcons[] = {cUrl
,Contact
,mailbox
}; 

static const char * p2pMenuIcons[] = {cUrl
,Contact
,mailbox
};

/** @brief   List main menu icon address, null if nothing to display
  * @details The order in the list defines the position of the icons in the Main menu.
  */
static const char* MainMenuIcons[] = { tag_detection_84x84,  emulate_84x84, P2P_84x84 ,aat_84x84 };

/* Demo functions */
void tagDetectionDemo(void);
void ceUriDemo(void);
void ceVcardDemo(void);
Menu_Callback_Status_t CEDemoRun(void);
void P2P_StartDemo(void);
void doNothingURI(void);
void doNothingVcard(void);
Menu_Callback_Status_t P2P_RunDemo(void);
void startAAT (void);

/* Card emulation menu items */
struct sMenuItem ceMenuItems[] = {
  {"Card Emul: URI", &ceUriDemo, IdleFunc, NULL},
  {"Contact", &ceVcardDemo,IdleFunc,NULL},
  {"Received data", NULL, IdleFunc, NULL}
};

/** Card emulation menu definition. */
#if defined(USE_JOYSTICKONLY)
    static struct sMenu ceMenu = {"CE menu", ceMenuItems, countof(ceMenuIcons), ceMenuIcons, "PB Blue to exit", &CEDemoRun};
#else
    static struct sMenu ceMenu = {"CE menu", ceMenuItems, countof(ceMenuIcons), ceMenuIcons, "Touch to exit", &CEDemoRun};
#endif /* USE_JOYSTICKONLY */
    
/** Peer 2 peer menu items. */
struct sMenuItem p2pMenuItems[] = {
  {"Peer 2 peer: URI", &doNothingURI, IdleFunc, NULL},
  {"Peer 2 peer: vCard", &doNothingVcard, IdleFunc, NULL},
  {"Received data", NULL, IdleFunc, NULL}
};

/** Peer 2 peer menu definition. */
#if defined(USE_JOYSTICKONLY)
    static struct sMenu p2pMenu = {"P2P menu", p2pMenuItems, countof(p2pMenuIcons), p2pMenuIcons, "PB Blue to exit", &P2P_RunDemo};
#else
    static struct sMenu p2pMenu = {"P2P menu", p2pMenuItems, countof(p2pMenuIcons), p2pMenuIcons, "Touch to exit", &P2P_RunDemo};
#endif /* USE_JOYSTICKONLY */
    
/** Defines ST25R3916 demo main menu items. */
static struct sMenuItem MainMenuItems[] = {
  {"Reader / Writer", &tagDetectionDemo, IdleFunc, NULL},
  {"Card emulation", &ceUriDemo, IdleFunc, &ceMenu},
  {"Peer to peer", &P2P_StartDemo, IdleFunc, &p2pMenu},
  {"Antenna Tuning", &startAAT, IdleFunc, NULL}
};

/** Defines ST25R3916 demo main menu. */
#ifdef ST25R3916B
static struct sMenu MainMenu = {"Select a demo", MainMenuItems, countof(MainMenuItems), MainMenuIcons, "ST25R3916B  v" MCU_FW_VERSION};
#else
static struct sMenu MainMenu = {"Select a demo", MainMenuItems, countof(MainMenuItems), MainMenuIcons, "ST25R3916   v" MCU_FW_VERSION};
#endif


/** @brief Starts the main loop for the demo menu. */
void Menu_Start(void)
{
  Menu_MainLoop(&MainMenu);
}

/**
  * @}
  */ 
/**
  * @}
  */ 
/************************ (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
