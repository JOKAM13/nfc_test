/**
  ******************************************************************************
  * @file    p2p.c
  * @author  MMY Application Team
  * @version $Revision$
  * @date    $Date$
  * @ingroup ST25R3916_Discovery_Demo
  * @brief   Peer to peer demo
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
/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "st_errno.h"
#include "utils.h"
#include "rfal_platform.h"
#include "rfal_nfca.h"
#include "rfal_nfcb.h"
#include "rfal_nfcf.h"
#include "rfal_nfcv.h"
#include "rfal_isoDep.h"
#include "rfal_nfcDep.h"
#include "rfal_analogConfig.h"
#include "Menu_core.h"
#include "ndef_display.h"
#include "ndef_poller.h"
#include "ndef_message.h"
#include "ndef_types.h"
#include "ndef_type_uri.h"

#include "rfal_nfcDep.h"
#include "nfc.h"

#include <stdlib.h>
#include "Menu_config.h"
#include "Menu_core.h"

/* peer to peer lib*/
#include "lib_llcp.h"
#include "lib_snep.h"

/* No-op method from menu definition */
void IdleFunc(void);

/* Reuse shared pictures */
extern const char cUrl[];
extern const char cVcard[];
extern const char cEmail[];
extern const char mailbox[];
extern uint8_t SNEP_Message2SendState;
/* SNEP status: NDEF file available */
extern uint8_t NdefFileReceive;


/** Global: Target mode enabled, shared with LLCP library */
bool TargetMode;
/* Sends data, to be called only when a peer device is connected */
static void enableDataTransfer(void);
/* Adds iterations between peer conneciton and message sending */
uint8_t waitBeforeAutoSend = 10;

/* define P2P target timeout */
#define P2P_TARGET_TIMEOUT 1000
uint32_t P2P_TargetTimeout = 0;
/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/
#define EXAMPLE_RFAL_POLLER_DEVICES      10    /* Number of devices supported */
#define EXAMPLE_RFAL_POLLER_RF_BUF_LEN   255   /* RF buffer length            */

#define EXAMPLE_RFAL_POLLER_FOUND_NONE   0x00  /* No device found Flag        */
#define EXAMPLE_RFAL_POLLER_FOUND_A      0x01  /* NFC-A device found Flag     */
#define EXAMPLE_RFAL_POLLER_FOUND_B      0x02  /* NFC-B device found Flag     */
#define EXAMPLE_RFAL_POLLER_FOUND_F      0x04  /* NFC-F device found Flag     */
#define EXAMPLE_RFAL_POLLER_FOUND_V      0x08  /* NFC-V device Flag           */


/*
******************************************************************************
* GLOBAL TYPES
******************************************************************************
*/

/*! Main state                                                                            */
typedef enum{
    EXAMPLE_RFAL_POLLER_STATE_INIT                =  0,  /* Initialize state            */
    EXAMPLE_RFAL_POLLER_STATE_TECHDETECT          =  1,  /* Technology Detection state  */
    EXAMPLE_RFAL_POLLER_STATE_COLAVOIDANCE        =  2,  /* Collision Avoidance state   */
    EXAMPLE_RFAL_POLLER_STATE_ACTIVATION          =  3,  /* Activation state            */
    EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_START  =  4,  /* Data Exchange Start state   */
    EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_CHECK  =  5,  /* Data Exchange Check state   */
    EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION        =  9   /* Deactivation state          */
}exampleRfalPollerState;


/*! Device type                                                                           */
typedef enum{
    EXAMPLE_RFAL_POLLER_TYPE_NFCA  =  0,                 /* NFC-A device type           */
    EXAMPLE_RFAL_POLLER_TYPE_NFCB  =  1,                 /* NFC-B device type           */
    EXAMPLE_RFAL_POLLER_TYPE_NFCF  =  2,                 /* NFC-F device type           */
    EXAMPLE_RFAL_POLLER_TYPE_NFCV  =  3,                  /* NFC-V device type           */
    EXAMPLE_RFAL_POLLER_TYPE_ACTIVE_P2P  =  4,                 /* P2P active device type           */
}exampleRfalPollerDevType;


/*! Device interface                                                                      */
typedef enum{
    EXAMPLE_RFAL_POLLER_INTERFACE_RF     = 0,            /* RF Frame interface          */
    EXAMPLE_RFAL_POLLER_INTERFACE_ISODEP = 1,            /* ISO-DEP interface           */
    EXAMPLE_RFAL_POLLER_INTERFACE_NFCDEP = 2             /* NFC-DEP interface           */
}exampleRfalPollerRfInterface;


/*! Device struct containing all its details                                              */
typedef struct{
    exampleRfalPollerDevType type;                      /* Device's type                */
    union{
        rfalNfcaListenDevice nfca;                      /* NFC-A Listen Device instance */
        rfalNfcbListenDevice nfcb;                      /* NFC-B Listen Device instance */
        rfalNfcfListenDevice nfcf;                      /* NFC-F Listen Device instance */
        rfalNfcvListenDevice nfcv;                      /* NFC-V Listen Device instance */
    }dev;                                               /* Device's instance            */
    
    exampleRfalPollerRfInterface rfInterface;           /* Device's interface           */
    union{
        rfalIsoDepDevice isoDep;                        /* ISO-DEP instance             */
        rfalNfcDepDevice nfcDep;                        /* NFC-DEP instance             */
    }proto;                                             /* Device's protocol            */
    
}exampleRfalPollerDevice;

/** Defines the Peer to peer demo states */
typedef enum {
  P2P_DEMO_TARGET_PASSIVE     = 0x00,
  P2P_DEMO_TARGET_ACTIVE      = 0x01,
  P2P_DEMO_INITIATOR_PASSIVE  = 0x02,
  P2P_DEMO_INITIATOR_ACTIVE   = 0x03,
  P2P_DEMO_MODE_END
} p2p_mode_t;

/*
 ******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************
 */
/* NFC Id when connected following Felica P2P */
static uint8_t                 gNfcid3[]       = {0x01, 0xFE, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A };                                  /* NFCID3 used for ATR_REQ */
/* Generic Bytes for p2p */
static uint8_t                 gGenBytes[]     =  {0x46, 0x66, 0x6d, 0x01, 0x01, 0x11, 0x02, 0x02, 0x07, 0x80, 0x03, 0x02, 0x00, 0x03, 0x04, 0x01, 0x32, 0x07, 0x01, 0x03 }; /* P2P General Bytes: LCCP Connect */

/*******************************************************************************/

static uint8_t                 gDevCnt;                                 /* Number of devices found                         */
static exampleRfalPollerDevice gDevList[EXAMPLE_RFAL_POLLER_DEVICES];   /* Device List                                     */
static exampleRfalPollerState  gState;                                  /* Main state                                      */
static uint8_t                 gTechsFound;                             /* Technologies found bitmask                      */
exampleRfalPollerDevice        *gP2pDev;                             /* Active device pointer                           */

static rfalNfcDevice           *nfcDevice;
static ndefContext             ndefctx;


/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
static bool exampleRfalPollerTechDetetection( void );
static bool exampleRfalPollerCollResolution( void );
static bool exampleRfalPollerActivation( uint8_t devIt );
static bool exampleRfalPollerNfcDepActivate( exampleRfalPollerDevice *device );
static bool exampleRfalPollerDeactivate( void );
static void P2P_SendNdef(void);
static void p2p_displayNDEF(void);

/* Length of the p2p message to send */
static uint32_t p2pCmdLength = 0;
/* Pointer to the p2p message to send */
static uint8_t *p2pNdefFile = NULL;

/* LLCP lib control */
extern bool LLCPSymActivated;
/* The demo takes control of p2p menu to enable the read Icon when a NDEF message is received */
extern struct sMenuItem p2pMenuItems[];
/* reuse vCard from Card Emulation demo */
extern const char contact_base64[];

/*
******************************************************************************
* LOCAL VAR
******************************************************************************
*/
/* Demo message: No connection */
static const char* WaitConnection =        " Wait connection... ";
/* Demo message: Connected as a Passive Target */
static const char* ConnectedTarget =       "Role: Target(P)";
/* Demo message: Connected as an active Target */
static const char* ConnectedTargetActive = "Role: Target(A)";
/* Demo message: Connected as an Initiator */
static const char* ConnectedInitiator =    "Role: Initiator";
/* Reminds if target is connected as activ or passive */
static bool isActive = 0; 

/** Defines P2P machine states */
typedef enum {
  P2P_DEMO_CHANGE_MODE = 0,         /** Select a new connection mode */
  P2P_DEMO_TARGET_WAIT_CONNECTION,  /** Wait for a peer connection */
  P2P_DEMO_TARGET_CONNECTED,        /** Connected as a Target */
  P2P_DEMO_INITIATOR_RUN            /** Connected as an Initiator */
} P2P_Demo_State_t;

/* Current p2p machine state */
static P2P_Demo_State_t p2p_demo_state; 
/* Function handler to prepare and send an NDEF message */
static void  (*fnNdef)(void);

/* Function to prepare and send a NDEF URL message */
static void p2pUriDemo(void)
{
  ReturnCode err;
  ndefMessage message;
  ndefBuffer bufRawMessage;
  ndefConstBuffer bufUri;
  char ndefURI[] = "st.com/st25r3916-demo";
  ndefType uri;
  ndefRecord record;

 // save selected state
  fnNdef = &p2pUriDemo;

	platformLog("\n\rExample RFAL Poller started \r\n");

  err = ndefPollerContextInitialization(&ndefctx, nfcDevice);
  
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
  
  p2pCmdLength = bufRawMessage.length;
  p2pNdefFile = NDEF_Buffer;

  P2P_SendNdef();
}



/* Function to prepare and send a NDEF vCard message */
void p2pBigVcardDemo(void)
{
 // save selected state
  fnNdef = &p2pBigVcardDemo;

  p2pCmdLength = 0x1FCA;
  p2pNdefFile = (uint8_t*)&contact_base64[2];

  P2P_SendNdef();
}

/* Method to display a received NDEF */
static void p2p_displayNDEF(void)
{
  char txt[50] = {0};
  ndefConstBuffer  bufRawMessage;
  ndefMessage ndefmessage;

  if((SNEP_Message2SendState == SNEP_MESSAGE2SEND) || 
      (SNEP_Message2SendState == SNEP_MESSAGE_SENDINGONGOING))
    return;

  Menu_MsgStatus("Received data","",MSG_INFO);
#if defined(USE_JOYSTICKONLY)
  Menu_DisplayCenterString(10, "PB Blue to exit");
#else
  Menu_DisplayCenterString(10, "Touch to exit");
#endif /* USE_JOYSTICKONLY */
  SNEP_RetrieveNDEF((uint8_t **)&bufRawMessage.buffer);

  bufRawMessage.length = bufRawMessage.buffer[0];
  bufRawMessage.length = (bufRawMessage.length << 8) | bufRawMessage.buffer[1];
    
  ndefMessageDecode(&bufRawMessage, &ndefmessage);
  
  if(displayNdef(&ndefmessage) != RFAL_ERR_NONE)
  {
      sprintf(txt, "Cannot identify NDEF record");
      Menu_SetStyle(PLAIN);
      BSP_LCD_SetFont(&Font16);
      BSP_LCD_DisplayStringAt(0,70,(uint8_t*)txt,CENTER_MODE);
  }

  BSP_LCD_SetFont(&Font22);

  HAL_Delay(200);
  Menu_WaitForUser();


  // disable data received icon
  p2pMenuItems[2].pfMenuFunc = NULL;
  Menu_DisplayIconsMenu();
  Menu_SetStyle(PLAIN);
    if(p2p_demo_state == P2P_DEMO_TARGET_CONNECTED)
    {
      if(isActive)
        Menu_DisplayCenterString(2,ConnectedTargetActive);
      else
        Menu_DisplayCenterString(2,ConnectedTarget);
    } else {
      Menu_DisplayCenterString(2,ConnectedInitiator);
    }
}


/*!
 * \brief Initiator Poller Run
 */
Menu_Callback_Status_t p2pRfalPollerRun( void )
{

	    rfalWorker();                                                                 /* Execute RFAL process */   
 
	    switch( gState )
	    {
	        /*******************************************************************************/
	        case EXAMPLE_RFAL_POLLER_STATE_INIT:                                     
	            
	            gTechsFound = EXAMPLE_RFAL_POLLER_FOUND_NONE; 
	            gP2pDev  = NULL;
	            gDevCnt     = 0;
	            
              if(isActive)
                gState = EXAMPLE_RFAL_POLLER_STATE_ACTIVATION;
              else
                gState = EXAMPLE_RFAL_POLLER_STATE_TECHDETECT;
              break;
	            
	            
            /*******************************************************************************/
	        case EXAMPLE_RFAL_POLLER_STATE_TECHDETECT:
	            
	            if( !exampleRfalPollerTechDetetection() )                             /* Poll for nearby devices in different technologies */
	            {
	                //gState = EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION;                  /* If no device was found, restart loop */
                  // if there is no passive device try active NfcDep detection
	                gState = EXAMPLE_RFAL_POLLER_STATE_ACTIVATION;
	            break;
	            }
	            
	            gState = EXAMPLE_RFAL_POLLER_STATE_COLAVOIDANCE;                      /* One or more devices found, go to Collision Avoidance */
	            break;
	            
	            
            /*******************************************************************************/
	        case EXAMPLE_RFAL_POLLER_STATE_COLAVOIDANCE:
	            
	            if( !exampleRfalPollerCollResolution() )                              /* Resolve any eventual collision */
                {
                    gState = EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION;                  /* If Collision Resolution was unable to retrieve any device, restart loop */
                    break;
                }
	            
	            platformLog("Device(s) found: %d \r\n", gDevCnt);                   
	            gState = EXAMPLE_RFAL_POLLER_STATE_ACTIVATION;                        /* Device(s) have been identified, go to Activation */
                break;
	        
                
            /*******************************************************************************/
	        case EXAMPLE_RFAL_POLLER_STATE_ACTIVATION:
                if(gDevCnt  == 0)
                {
                    rfalNfcGetActiveDevice( &nfcDevice );
                    gDevCnt++;
                    gDevList[0].type = EXAMPLE_RFAL_POLLER_TYPE_ACTIVE_P2P;
                    if (!exampleRfalPollerActivation( 0 ))
                    {
                        gState = EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION;                  /* If Activation failed, restart loop */
                        return MENU_CALLBACK_CONTINUE;                 
                    }
                }
                else if( !exampleRfalPollerActivation( 0 ) )                               /* Any device previous identified can be Activated, on this example will select the first on the list */
	            {
                    gState = EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION;                  /* If Activation failed, restart loop */
                    break;
                }
              
              /* full p2p */
              LLCPSymActivated = false;
              LLCP_Init();
              Menu_DisplayCenterString(2,ConnectedInitiator);
              Menu_DisplayCenterString(8,"Transfer successful");

              /* SNEP server initialization */
              SNEP_Init( SERVER_SNEP, SERVER_SNEP_URI );	

              /************/

	            gState = EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_START;                /* Device has been properly activated, go to Data Exchange */
	            break;
	            
	            
            /*******************************************************************************/
	        case EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_START:                       
	        case EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_CHECK:

                if(waitBeforeAutoSend > 0)
                {
                  waitBeforeAutoSend--;
                  if(waitBeforeAutoSend == 0)
                  enableDataTransfer();  
                }
                Server_SNEP();
                if(LLCP_Send() != LLCP_SUCCESS_CODE)
                {
                  p2p_demo_state = P2P_DEMO_CHANGE_MODE;
                  gState = EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION;              /* Restart loop */
                }

                break;

          /*******************************************************************************/
	        case EXAMPLE_RFAL_POLLER_STATE_DEACTIVATION:
	            
	            exampleRfalPollerDeactivate();                                        /* If a card has been activated, properly deactivate the device */
	            
	            rfalFieldOff();                                                       /* Turn the Field Off powering down any device nearby */
              for(int i = 0; i < 10 ; i++)
              {
                uint8_t ButtonState = 0;
                Menu_Position_t touch;
                HAL_Delay(1);
                if((Menu_ReadPosition(&touch)) || (Menu_ReadSelection(&ButtonState)))
                {
                  HAL_Delay(100);
                  if((touch.Sel && (touch.Y > 200)) || (ButtonState != 0))
                  {
                    HAL_Delay(200);
                    SNEP_Free();
                    return MENU_CALLBACK_LAST;
                  }
                }
              }

	            gState = EXAMPLE_RFAL_POLLER_STATE_INIT;                              /* Restart the loop */
	            break;
	        
	        
            /*******************************************************************************/
	        default:
              SNEP_Free();
	            return MENU_CALLBACK_LAST;
	    }
  return MENU_CALLBACK_CONTINUE;
}
	

/*!
 ******************************************************************************
 * \brief Poller Technology Detection
 * 
 * This method implements the Technology Detection / Poll for different 
 * device technologies.
 * 
 * \return true         : One or more devices have been detected
 * \return false         : No device have been detected
 * 
 ******************************************************************************
 */
static bool exampleRfalPollerTechDetetection( void )
{
    ReturnCode           err;
    rfalNfcaSensRes      sensRes;
    
    gTechsFound = EXAMPLE_RFAL_POLLER_FOUND_NONE;
    
    /*******************************************************************************/
    /* NFC-A Technology Detection                                                  */
    /*******************************************************************************/
    
    rfalNfcaPollerInitialize();                                                       /* Initialize RFAL for NFC-A */
    rfalFieldOnAndStartGT();                                                          /* Turns the Field On and starts GT timer */
    
    err = rfalNfcaPollerTechnologyDetection( RFAL_COMPLIANCE_MODE_NFC, &sensRes ); /* Poll for NFC-A devices */
    if( err == ERR_NONE )
    {
        gTechsFound |= EXAMPLE_RFAL_POLLER_FOUND_A;
    }
    
    
    /*******************************************************************************/
    /* NFC-F Technology Detection                                                  */
    /*******************************************************************************/
    
    rfalNfcfPollerInitialize( RFAL_BR_212 );                                          /* Initialize RFAL for NFC-F */
    rfalFieldOnAndStartGT();                                                          /* As field is already On only starts GT timer */
    
    err = rfalNfcfPollerCheckPresence();                                              /* Poll for NFC-F devices */
    if( err == ERR_NONE )
    {
        gTechsFound |= EXAMPLE_RFAL_POLLER_FOUND_F;
    }

    return (gTechsFound != EXAMPLE_RFAL_POLLER_FOUND_NONE);
}

/*!
 ******************************************************************************
 * \brief Poller Collision Resolution
 * 
 * This method implements the Collision Resolution on all technologies that
 * have been detected before.
 * 
 * \return true         : One or more devices identified 
 * \return false        : No device have been identified
 * 
 ******************************************************************************
 */
static bool exampleRfalPollerCollResolution( void )
{
    uint8_t    i;
    uint8_t    devCnt;
    ReturnCode err;
    
    
    /*******************************************************************************/
    /* NFC-A Collision Resolution                                                  */
    /*******************************************************************************/
    if( gTechsFound & EXAMPLE_RFAL_POLLER_FOUND_A )                                   /* If a NFC-A device was found/detected, perform Collision Resolution */
    {
        rfalNfcaListenDevice nfcaDevList[EXAMPLE_RFAL_POLLER_DEVICES];
        
        rfalNfcaPollerInitialize();        
        rfalFieldOnAndStartGT();                                                      /* Ensure GT again as other technologies have also been polled */
        err = rfalNfcaPollerFullCollisionResolution( RFAL_COMPLIANCE_MODE_NFC /* RFAL_COMPLIANCE_MODE_ISO*/, (EXAMPLE_RFAL_POLLER_DEVICES - gDevCnt), nfcaDevList, &devCnt );
        if( (err == ERR_NONE) && (devCnt != 0) )
        {
            for( i=0; i<devCnt; i++ )                                                 /* Copy devices found form local Nfca list into global device list */
            {
                gDevList[gDevCnt].type     = EXAMPLE_RFAL_POLLER_TYPE_NFCA;
                gDevList[gDevCnt].dev.nfca = nfcaDevList[i];
                gDevCnt++;
            }
        }
    }
    
    /*******************************************************************************/
    /* NFC-F Collision Resolution                                                  */
    /*******************************************************************************/
    if( gTechsFound & EXAMPLE_RFAL_POLLER_FOUND_F )                                   /* If a NFC-F device was found/detected, perform Collision Resolution */
    {
        rfalNfcfListenDevice nfcfDevList[EXAMPLE_RFAL_POLLER_DEVICES];
        
        rfalNfcfPollerInitialize( RFAL_BR_212 );
        rfalFieldOnAndStartGT();                                                      /* Ensure GT again as other technologies have also been polled */
        err = rfalNfcfPollerCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, (EXAMPLE_RFAL_POLLER_DEVICES - gDevCnt), nfcfDevList, &devCnt );
        if( (err == ERR_NONE) && (devCnt != 0) )
        {
            for( i=0; i<devCnt; i++ )                                                 /* Copy devices found form local Nfcf list into global device list */
            {
                gDevList[gDevCnt].type     = EXAMPLE_RFAL_POLLER_TYPE_NFCF;
                gDevList[gDevCnt].dev.nfcf = nfcfDevList[i];
                gDevCnt++;
            }
        }
    }
        
    return (gDevCnt > 0);
}

/*!
 ******************************************************************************
 * \brief Poller Activation
 * 
 * This method Activates a given device according to it's type and 
 * protocols supported
 *  
 * \param[in]  devIt : device's position on the list to be activated 
 * 
 * \return true         : Activation successful 
 * \return false        : Activation failed
 * 
 ******************************************************************************
 */
static bool exampleRfalPollerActivation( uint8_t devIt )
{
    ReturnCode           err;
    rfalNfcaSensRes      sensRes;
    rfalNfcaSelRes       selRes;
    
    if( devIt >= gDevCnt )
    {
        // do NfcDep active activation
        return false;
    }
    
    if( (gDevList[devIt].type == EXAMPLE_RFAL_POLLER_TYPE_NFCA) && ((gDevList[devIt].dev.nfca.type != RFAL_NFCA_T4T_NFCDEP) &&
        (gDevList[devIt].dev.nfca.type != RFAL_NFCA_NFCDEP)))
        return false;

    switch( gDevList[devIt].type )
    {
        /*******************************************************************************/
        /* NFC-A Activation                                                            */
        /*******************************************************************************/
        case EXAMPLE_RFAL_POLLER_TYPE_NFCA:
            rfalNfcaPollerInitialize();

            if( gDevList[devIt].dev.nfca.isSleep )                                    /* Check if desired device is in Sleep      */
            {
                err = rfalNfcaPollerCheckPresence( RFAL_14443A_SHORTFRAME_CMD_WUPA, &sensRes ); /* Wake up all cards  */
                if( err != ERR_NONE )
                {
                    return false;
                }
                err = rfalNfcaPollerSelect( gDevList[devIt].dev.nfca.nfcId1, gDevList[devIt].dev.nfca.nfcId1Len, &selRes ); /* Select specific device  */
                if( err != ERR_NONE )
                {
                    return false;
                }
            }
            
             /*******************************************************************************/
              /* Perform NFC-DEP (P2P) activation: ATR and PSL if supported */
              if( !exampleRfalPollerNfcDepActivate( &gDevList[devIt] ) )
              {
                return false;
              }
              
              platformLog("NFC-A P2P (NFC-DEP) device activated \r\n");         /* NFC-A P2P device activated */
              gDevList[devIt].rfInterface = EXAMPLE_RFAL_POLLER_INTERFACE_NFCDEP;
            
            break;
        
            
        /*******************************************************************************/
        /* NFC-F Activation                                                            */
        /*******************************************************************************/
        case EXAMPLE_RFAL_POLLER_TYPE_NFCF:
            
            rfalNfcfPollerInitialize( RFAL_BR_212 );

            if( rfalNfcfIsNfcDepSupported( &gDevList[devIt].dev.nfcf ))
            {
                /* Perform NFC-DEP (P2P) activation: ATR and PSL if supported */
                if( !exampleRfalPollerNfcDepActivate( &gDevList[devIt] ) )
                {
                    return false;
                }
                
                platformLog("NFC-F P2P (NFC-DEP) device activated \r\n");             /* NFC-A P2P device activated */
                
                gDevList[devIt].rfInterface = EXAMPLE_RFAL_POLLER_INTERFACE_NFCDEP;
                break;
            }
            
            platformLog("NFC-F T3T device activated \r\n");                           /* NFC-F T3T device activated */
            gDevList[devIt].rfInterface = EXAMPLE_RFAL_POLLER_INTERFACE_RF;
            break;
        case EXAMPLE_RFAL_POLLER_TYPE_ACTIVE_P2P: 
            err = rfalSetMode(RFAL_MODE_POLL_ACTIVE_P2P, RFAL_BR_106, RFAL_BR_106);
            rfalSetGT(rfalConvMsTo1fc( 30 ));
            rfalFieldOnAndStartGT();
            if( !exampleRfalPollerNfcDepActivate( &gDevList[devIt] ) )
            {
              return false;
            }
                    
            gDevList[devIt].rfInterface = EXAMPLE_RFAL_POLLER_INTERFACE_NFCDEP;
          break;
        
        /*******************************************************************************/
        default:
            return false;
    }
    
    gP2pDev = &gDevList[devIt];                                                    /* Assign active device to be used further on */
    return true;
}


/*!
 ******************************************************************************
 * \brief Poller NFC DEP Activate
 * 
 * This method performs NFC-DEP Activation 
 *  
 * \param[in]  devIt : device to be activated 
 * 
 * \return true         : Activation successful 
 * \return false        : Activation failed
 * 
 ******************************************************************************
 */
static bool exampleRfalPollerNfcDepActivate( exampleRfalPollerDevice *device )
{
    rfalNfcDepAtrParam   param;
    rfalNfcDepInitialize();

    /*******************************************************************************/
    /* If Passive F use the NFCID2 retrieved from SENSF                            */
    if( device->type == EXAMPLE_RFAL_POLLER_TYPE_NFCF )
    {
        param.nfcid    = device->dev.nfcf.sensfRes.NFCID2;
        param.nfcidLen = RFAL_NFCF_NFCID2_LEN;
    }
    else
    {
        param.nfcid    = gNfcid3;
        param.nfcidLen = RFAL_NFCDEP_NFCID3_LEN;
    }    
    
    param.BS    = RFAL_NFCDEP_Bx_NO_HIGH_BR;
    param.BR    = RFAL_NFCDEP_Bx_NO_HIGH_BR;
    param.DID   = RFAL_NFCDEP_DID_NO;
    param.NAD   = RFAL_NFCDEP_NAD_NO;
    param.LR    = RFAL_NFCDEP_LR_254;
    param.GB    = gGenBytes;
    param.GBLen = sizeof(gGenBytes);
    if(device->type == EXAMPLE_RFAL_POLLER_TYPE_ACTIVE_P2P)
    {
      param.commMode  = RFAL_NFCDEP_COMM_ACTIVE;
    } else {
      param.commMode  = RFAL_NFCDEP_COMM_PASSIVE;
    }

    param.operParam = (RFAL_NFCDEP_OPER_FULL_MI_EN | RFAL_NFCDEP_OPER_EMPTY_DEP_DIS | RFAL_NFCDEP_OPER_ATN_EN | RFAL_NFCDEP_OPER_RTOX_REQ_EN);
    
    /* Perform NFC-DEP (P2P) activation: ATR and PSL if supported */
    return (rfalNfcDepInitiatorHandleActivation( &param, RFAL_BR_424, &device->proto.nfcDep ) == ERR_NONE);
}



/*!
 ******************************************************************************
 * \brief Poller NFC DEP Deactivate
 * 
 * This method Deactivates the device if a deactivation procedure exists 
 * 
 * \return true         : Deactivation successful 
 * \return false        : Deactivation failed
 * 
 ******************************************************************************
 */
static bool exampleRfalPollerDeactivate( void )
{
    if( gP2pDev != NULL )                                                          /* Check if a device has been activated */
    {
        switch( gP2pDev->rfInterface )
        {
            /*******************************************************************************/
            case EXAMPLE_RFAL_POLLER_INTERFACE_NFCDEP:
                rfalNfcDepRLS();                                                      /* Send a Release to device */
                break;
                
            default:
                return false;
        }
        platformLog("Device deactivated \r\n");
    }
    
    return true;
}


/* Dummy callback function, says always yes, used for rfal deactivating callback */
static bool p2pAlwaysTrue(void)
{
  return true;
}

/**
  * @brief  helper function to send data via the rfalNfcDep module
	* @param	txBuf     Buffer with comnad to be transmitted
	* @param	txBufSize Length in bytes of the Tx command
	* @param	rxBuf     Buffer to store the response
	* @param	rxBufSize Length in bytes of the rx buffer
	* @param	rxActLen  Actual repsonse length
  * @retval Status 0 is OK, other values are errors
  */
ReturnCode nfcDepTxRx( const uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxBuf, uint16_t rxBufSize, uint16_t *rxActLen )
{
  ReturnCode             err;
  bool                   isChaining;
  rfalNfcDepTxRxParam    rfalNfcDepTxRx;
  rfalNfcDepBufFormat    nfcDepRxBuf;
  rfalNfcDepBufFormat    nfcDepTxBuf;
  uint32_t timeout = HAL_GetTick() + 500;
  Menu_Position_t touch;
  uint32_t checkTouch = 0;
  uint8_t ButtonState = 0;


  /* Initialize the NFC-DEP protocol transceive context */
  rfalNfcDepTxRx.txBuf        = &nfcDepTxBuf;
  rfalNfcDepTxRx.txBufLen     = txBufSize;
  rfalNfcDepTxRx.rxBuf        = &nfcDepRxBuf;
  rfalNfcDepTxRx.rxLen     = rxActLen;
  rfalNfcDepTxRx.DID          = RFAL_NFCDEP_DID_NO;
  rfalNfcDepTxRx.FSx          = RFAL_NFCDEP_FRAME_SIZE_MAX_LEN;
  rfalNfcDepTxRx.FWT          = rfalNfcDepCalculateRWT( RFAL_NFCDEP_WT_INI_DEFAULT );
  rfalNfcDepTxRx.dFWT         = rfalNfcDepCalculateRWT( RFAL_NFCDEP_WT_DELTA );
  rfalNfcDepTxRx.isRxChaining = &isChaining;
  rfalNfcDepTxRx.isTxChaining = false;
  
  /* Copy data to send */
  ST_MEMCPY( nfcDepTxBuf.inf, txBuf, MIN( txBufSize, RFAL_NFCDEP_FRAME_SIZE_MAX_LEN ) );

  /* Set Deactivate Callback */
  rfalNfcDepSetDeactivatingCallback( (rfalNfcDepDeactCallback) p2pAlwaysTrue );

  /* Perform the NFC-DEP Transceive in a blocking way */
  rfalNfcDepStartTransceive( &rfalNfcDepTxRx );
  do {
    rfalWorker();
    err = rfalNfcDepGetTransceiveStatus();
    if(HAL_GetTick() > timeout)
    {
      Menu_DisplayCenterString(8,"Wait response");
    }
    if(checkTouch++ > 20)
    {
      checkTouch = 0;
      Menu_ReadPosition(&touch);
      if((touch.Sel && (touch.Y > 200)) || (Menu_ReadSelection(&ButtonState)))
      {
        Menu_DisplayCenterString(8,"Interrupted ");
        HAL_Delay(100);
        return err;
      }
    }
  } while(err == ERR_BUSY);

  /* get received data */
  if( err == ERR_NONE ) {
    /* Copy received data */
    ST_MEMCPY( rxBuf, nfcDepRxBuf.inf, MIN(*rxActLen, rxBufSize) );
   if(HAL_GetTick() > timeout)
      Menu_DisplayCenterString(8,"Transfer successful");
    return ERR_NONE;
  }

  Menu_DisplayCenterString(8,"Connection lost");

  return err;
}

/* UID when connecting as a target */
uint8_t p2p_uid[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};
/* P2P generic bytes when connecting as a target */
static uint8_t p2p_gen[] = {0x46, 0x66, 0x6d, 0x01, 0x01, 0x10, 0x03, 0x02, 0x00, 0x01, 0x04, 0x01, 0x96};

#ifdef ENABLE_P2P_DEBUG
uint8_t DBG_count = 0;
uint8_t DBG_data[5][10];
#endif



/** @brief Start peer to peer demo.
  * Send an URI NDEF message after connection.
  */
void P2P_StartDemo(void)
{
  srand(*(uint32_t*)UID_BASE ^ HAL_GetTick());
  p2p_demo_state = P2P_DEMO_CHANGE_MODE;
  fnNdef = &p2pUriDemo;
}

/* Initialization for target mode */
static Menu_Callback_Status_t P2P_InitTarget(void)
{

  nfcInitialize(isActive, false, RFAL_BR_424, 0x0D);
  nfcDepTargetSetParams(p2p_uid, RFAL_NFCDEP_LR_254, sizeof(p2p_gen), p2p_gen);
  p2p_demo_state = P2P_DEMO_TARGET_WAIT_CONNECTION;

  return MENU_CALLBACK_CONTINUE;
}

/* Manage connection as a target */
static Menu_Callback_Status_t P2P_ConnectTarget(void)
{
  uint8_t bitrate;
  rfalNfcDepActivation atrReq;
  uint8_t atrReqLen;

  uint8_t err = NfcDepTargetHandleActivation(&bitrate, &isActive,&atrReqLen, (uint8_t*)&atrReq);
  rfalWorker();

  if (err == ERR_BUSY)
  {
    p2p_demo_state = P2P_DEMO_TARGET_WAIT_CONNECTION;
  } else if(err == ERR_NONE) {
    LLCPSymActivated = false;
    LLCP_Init();
    SNEP_Init( SERVER_SNEP, SERVER_SNEP_URI );
    if(isActive)
      Menu_DisplayCenterString(2,ConnectedTargetActive);
    else
      Menu_DisplayCenterString(2,ConnectedTarget);
    P2P_TargetTimeout = HAL_GetTick();
    p2p_demo_state = P2P_DEMO_TARGET_CONNECTED;

  } else {
    Menu_DisplayCenterString(2,WaitConnection);
    Menu_DisplayCenterString(8,"Connection lost");
    p2p_demo_state = P2P_DEMO_CHANGE_MODE;
  }
  return MENU_CALLBACK_CONTINUE;
}

/* Manage the data transfer after being connected as a target */
static Menu_Callback_Status_t P2P_TargetConnected(void)
{
  uint8_t err;
  uint8_t data[1024];
  uint16_t actlength;

  if((uint32_t)((int) HAL_GetTick() -(int) P2P_TargetTimeout) > P2P_TARGET_TIMEOUT)
  {
    // timeout in target mode
    Menu_DisplayCenterString(2,WaitConnection);
    Menu_DisplayCenterString(8,"Connection lost");
    p2p_demo_state = P2P_DEMO_TARGET_WAIT_CONNECTION;
  }

  if(waitBeforeAutoSend > 0)
  {
    waitBeforeAutoSend--;
    if(waitBeforeAutoSend == 0)
    enableDataTransfer();  
  }


  rfalWorker();

  err = nfcDepRx( data, sizeof(data), &actlength );
  if(err == ERR_NONE)
  {
    uint8_t inner_err;
    if(!actlength)
      while(1);
    Server_SNEP();
    LLCP_Reply(data,&actlength);

#ifdef ENABLE_P2P_DEBUG
    if(DBG_count<5)
    memcpy(&DBG_data[DBG_count++][0],data,actlength>10?10:actlength);
#endif

    inner_err = nfcDepTx(data, actlength);
    if(inner_err != ERR_NONE)
      while(1);

    P2P_TargetTimeout = HAL_GetTick();
    p2p_demo_state = P2P_DEMO_TARGET_CONNECTED;

  } else if((err != ERR_BUSY) && (err != ERR_RELEASE_REQ)) {
    // error in listen mode
    Menu_DisplayCenterString(2,WaitConnection);
    Menu_DisplayCenterString(8,"Connection lost");
    p2p_demo_state = P2P_DEMO_TARGET_WAIT_CONNECTION;
  }
  return MENU_CALLBACK_CONTINUE;

}

/* Sends the selected NDEF message */
static void P2P_SendNdef(void)
{
    if(((p2p_demo_state == P2P_DEMO_INITIATOR_RUN) && ((gState == EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_START) || ((gState == EXAMPLE_RFAL_POLLER_STATE_DATAEXCHANGE_CHECK)))) ||
      (p2p_demo_state == P2P_DEMO_TARGET_CONNECTED))
      {
        if(SNEP_SendNDEF(p2pNdefFile, p2pCmdLength) == SNEP_SUCCESS_CODE)
        {
          Menu_DisplayCenterString(8,"Transfer on-going");
        }  
      }
}

/** @brief Select the URI demo function to be executed after connection */
void doNothingURI(void){ fnNdef = p2pUriDemo; }
/** @brief Select the vCard demo function to be executed after connection */
void doNothingVcard(void){ fnNdef = p2pBigVcardDemo; }

/* Enables icon to transmit data after a peer device is connected */
static void enableDataTransfer(void)
{

  p2pMenuItems[0].pfMenuFunc = &p2pUriDemo;
  p2pMenuItems[1].pfMenuFunc = &p2pBigVcardDemo;

  fnNdef();
}

/* Disables icon to transmit data after a peer device is disconnected */
void disableDataTransfer(void)
{

    p2pMenuItems[0].pfMenuFunc = &doNothingURI;
    p2pMenuItems[1].pfMenuFunc = &doNothingVcard;
    waitBeforeAutoSend = 10;

}

/**
  * @brief Runs the Peer to peer demo.
  */
Menu_Callback_Status_t P2P_RunDemo(void)
{
  uint8_t ButtonState = 0;
  p2p_mode_t current_mode = (p2p_mode_t)(rand() % P2P_DEMO_MODE_END);
  static uint8_t mode = 0;
  Menu_Position_t touch;
  static uint32_t timeout = 0xFFFFFFFF;

  // exit this demo in case of user exit touch
  if((Menu_ReadPosition(&touch) && touch.Y > 200) || (Menu_ReadSelection(&ButtonState)))
  {
    HAL_Delay(200);
    SNEP_Free();

    if(mode)
      nfcDeinitialize();

    return MENU_CALLBACK_LAST;
  }
  // change mode after ~200 ms
  if(HAL_GetTick() > timeout)
  {
    // if initiator wait to reach init state before switching mode
    // if target change mode only when not connected
    if(((p2p_demo_state == P2P_DEMO_INITIATOR_RUN) && (gState == EXAMPLE_RFAL_POLLER_STATE_INIT)) ||
      (p2p_demo_state == P2P_DEMO_TARGET_WAIT_CONNECTION))
    {
      p2p_demo_state = P2P_DEMO_CHANGE_MODE;
    }
  }

  if(NdefFileReceive)
  {
    NdefFileReceive = 0;
    p2pMenuItems[2].pfMenuFunc = p2p_displayNDEF;
    Menu_DisplayIcons();
  }

  if(SNEP_Message2SendState == SNEP_MESSAGE_SENT)
  {
    Menu_DisplayCenterString(8,"Transfer successful");
  }

  switch(p2p_demo_state)
  {
    case P2P_DEMO_CHANGE_MODE:
      disableDataTransfer();
      current_mode++;
      current_mode %= P2P_DEMO_MODE_END;
      mode = (current_mode & 0x2) == 0x02;
      isActive = (current_mode & 0x01) == 0x01;

      timeout = HAL_GetTick() + (rand()%500);
      Menu_DisplayCenterString(2,WaitConnection);
      Menu_Callback_Status_t status;
      if(mode)
      {
        status = P2P_InitTarget();
      } else {
        // de-init target
        nfcDeinitialize();
        p2p_demo_state = P2P_DEMO_INITIATOR_RUN;
        status = MENU_CALLBACK_CONTINUE;
      }
      return status;
    /*NOTREACHED*/
    break;
    case P2P_DEMO_TARGET_WAIT_CONNECTION:
        return P2P_ConnectTarget();
    /*NOTREACHED*/
    break;
    case P2P_DEMO_TARGET_CONNECTED:
        return P2P_TargetConnected();
    /*NOTREACHED*/
    break;
    case P2P_DEMO_INITIATOR_RUN:
      return p2pRfalPollerRun();
    /*NOTREACHED*/
    break;
  }
  // should not end here
  SNEP_Free();
  return MENU_CALLBACK_LAST;
}

