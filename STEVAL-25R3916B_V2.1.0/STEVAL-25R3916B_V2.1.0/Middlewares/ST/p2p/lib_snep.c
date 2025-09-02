/**
  ******************************************************************************
  * @file    lib_snep.c 
  * @author  MMY Application Team
  * @version V4.0.0
  * @date    15/07/2014
  * @brief   NFC SNEP librairy 
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2015 STMicroelectronics
  *
  * Licensed under Software License Agreement (ST95HF), (the “License”); 
  *
  * You may not use this file except in compliance with the License, including, 
  * but not limited to, the specific language governing permissions 
  * and limitations therein.
  *
  * THIS SOFTWARE IS PROVIDED “AS IS” WITH ALL FAULTS. LICENSOR MAKES NO, 
  * AND HEREBY DISCLAIMS ON ITS BEHALF AND ON BEHALF OT ITS AFFILIATES, ALL, 
  * REPRESENTATIONS, WARRANTIES OR CONDITIONS, EXPRESS, IMPLIED OR STATUTORY 
  * WITH RESPECT TO THE LICENSED SOFTWARE AND DOCUMENTATION, INCLUDING WITHOUT 
  * LIMITATION ANY AND ALL WARRANTIES OF MERCHANTABILITY, SUITABILITY OR FITNESS 
  * FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS, 
  * ALL WARRANTIES THAT THE LICENSED SOFWTARE IS ERROR-FREE 
  * OR THAT ITS USE WILL BE UNINTERRUPTED, AND ANY AND ALL WARRANTIES ARISING 
  * FROM ANY COURSE OF DEALING OR USAGE OF TRADE. 
  *
	******************************************************************************
	*/
#ifdef __rev
    #define __REV __rev
#else
    #define __REV(x) ( (((x) & 0xFF) << 24) | (((x) & 0xFF00) << 8) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF000000) >> 24))
#endif
#include "lib_snep.h"

/* to notify user when NDEF file has been received */
bool NdefFileReceive = false;
bool NdefFileSend = false;
bool NdefFileTransmissionOnGoing = false;
bool Continue = false;

bool SNEP_Connected2LLCP = false; 
uint8_t SNEP_Message2SendState = SNEP_NO_MESSAGE;

//#define SNEP_STATIC 1
#ifdef SNEP_STATIC
static uint8_t	SNEP_Receive_Buffer [SNEP_MAX_SIZE_BUFFER];
static uint8_t	SNEP_Send_Buffer [SNEP_MAX_SIZE_BUFFER];
#else
static uint8_t	*SNEP_Receive_Buffer;
static uint8_t	*SNEP_Send_Buffer;
#endif

uint8_t SNEP_NotIplemented[0x06]={0x10,0xE0,0x00,0x00,0x00,0x00};
uint8_t SNEP_SuccessPut[0x06]={0x10,0x81,0x00,0x00,0x00,0x00};
uint8_t SNEP_Continue[0x06]={0x10,0x80,0x00,0x00,0x00,0x00};


static SNEP_Server SNEPServer;
static uint8_t SNEPInBuffer[LLCP_MAX_LENGTH];
static uint8_t SNEPOutBuffer[LLCP_MAX_LENGTH];

static uint8_t SNEP_Connect2LLCPServer(LLCP_LLC* LLC);
static uint8_t SNEP_Disconnect2LLCPServer( LLCP_LLC* LLC );

/* Request command */
#if 0
static uint8_t SNEP_SendGetCMD( void  );
static uint8_t SNEP_SendRejectCMD( void  );
#endif
static uint8_t SNEP_SendPutCMD( uint8_t *pBuffer, uint16_t  size );
static uint8_t SNEP_SendContinue( uint8_t *pBuffer, uint16_t  size );

/* Response command */
static uint8_t SNEP_SendRES( uint8_t CMD );

static uint8_t SNEP_Connect2LLCPServer(LLCP_LLC* LLC)
{
	if( SNEP_Connected2LLCP == false)
	{
    SNEPServer.Version= SNEP_SERVER_VERSION;
		SNEPServer.LLC=LLC;
		SNEPServer.LLC->InMessage=SNEPInBuffer;
		SNEPServer.LLC->OutMessage=SNEPOutBuffer;
		SNEPServer.SNEPFlags=0;
		SNEPServer.MessLen=0x00000000;
		SNEPServer.Offset=0x00000000;
    	
		SNEP_Connected2LLCP = true;
		
		return SNEP_SUCCESS_CODE;
	}
	else
		return SNEP_ERROR_CODE;	
}


static uint8_t SNEP_Disconnect2LLCPServer( LLCP_LLC* LLC )
{
	if( SNEP_Connected2LLCP == true)
	{
		SNEP_Connected2LLCP = false;
		return SNEP_SUCCESS_CODE;
	}
		
	return SNEP_ERROR_CODE;	
}

uint8_t SNEP_Init( uint8_t SAP, char* uri )
{
	LLCP_Service NewService;
	
	NdefFileReceive = false;
  NdefFileSend = false;
	NdefFileTransmissionOnGoing = false;
	Continue = false;
	SNEP_Connected2LLCP = false;
	/* Insure message can be sent when needed */
	SNEP_Message2SendState = SNEP_NO_MESSAGE;

#ifdef SNEP_STATIC
  SNEP_Receive_Buffer = NULL;
  SNEP_Send_Buffer = NULL;
#endif

	/* Create a service SNEP that will take care of NDEF content */
	NewService.SAP=SAP;
	NewService.Class=0x02;
	NewService.uri=uri;
	NewService.Connect=&SNEP_Connect2LLCPServer;
	NewService.Disconnect=&SNEP_Disconnect2LLCPServer;
	if( LLCP_AddService(NewService) == LLCP_SUCCESS_CODE )
	{
		/* SNEP Connection with LLCP service */
		if( LLCP_ConnectService( SAP, &(SNEPServer.LLC)) == LLCP_SUCCESS_CODE)
			return SNEP_SUCCESS_CODE;
	}

	return SNEP_ERROR_CODE;
}

uint8_t SNEP_RemoveService( uint8_t SAP)
{
	/* Disconnect LLCP link */
	if(IS_CONNECTION_FLAG(SNEPServer.LLC->Flags) )
	{	
		if(LLCP_Disconnect( SNEPServer.LLC ) == LLCP_SUCCESS_CODE)
		{	
			if(LLCP_RemoveService(SAP) == LLCP_SUCCESS_CODE) 
				return SNEP_SUCCESS_CODE;
		}
	}
	else
		if(LLCP_RemoveService(SAP) == LLCP_SUCCESS_CODE) 
				return SNEP_SUCCESS_CODE;
	
	return SNEP_ERROR_CODE;
}


bool SNEP_IsTransmissionComplete(void)
{	
	if(NdefFileReceive == true || NdefFileSend == true )
		if( !IS_OUT_MESS_FLAG(SNEPServer.LLC->Flags) && !IS_IN_MESS_FLAG(SNEPServer.LLC->Flags))
			return true;
	
	return false;	
}

bool SNEP_IsConnectionTerminated(void)
{	
	if( !IS_CONNECTION_FLAG(SNEPServer.LLC->Flags))
		return true;
	else
	{
		/* send disconnect command */
		LLCP_Disconnect( SNEPServer.LLC);
		return false;	
	}
}	

#include "stdlib.h"
/*****************************************************************************/
/*                         		  CLIENT (Interface)  		    								 */
/*****************************************************************************/
uint8_t SNEP_SendNDEF( uint8_t *pNdefMessage, uint16_t size )
{	
	/* Can deal with only one message at a time */
	if(SNEP_Message2SendState>SNEP_NO_MESSAGE)
	{
		return SNEP_ERROR_CODE;	
	}

#ifndef SNEP_STATIC
  if(SNEP_Send_Buffer != NULL)
  {
    free(SNEP_Send_Buffer);
    SNEP_Send_Buffer = NULL;
  }
  SNEP_Send_Buffer = malloc(size+SNEP_OFFSET_PUT_NDEF + 10);

  if(SNEP_Send_Buffer == NULL)
    return SNEP_ERROR_CODE;
#endif

	/* Prepare Message */
	SNEP_Send_Buffer[0] = 0x10; /* SNEP Version */
	SNEP_Send_Buffer[1] = 0x02; /* SNEP Put command */
	SNEP_Send_Buffer[2] = 0x00; /* Length */
	SNEP_Send_Buffer[3] = 0x00; /* Length */
	SNEP_Send_Buffer[4] = ((size & 0xFF00)>>8); /* Length */
	SNEP_Send_Buffer[5] = (size & 0x00FF); /* Length */
	
	SNEPServer.MessLen = size+SNEP_OFFSET_PUT_NDEF;
	
	memcpy( &SNEP_Send_Buffer[SNEP_OFFSET_PUT_NDEF] , pNdefMessage, SNEPServer.MessLen);
	SNEP_Message2SendState = SNEP_MESSAGE2SEND;
	return SNEP_SUCCESS_CODE;

}

uint8_t SNEP_RetrieveNDEF( uint8_t **pNdefMessage)
{
	*pNdefMessage = SNEP_Receive_Buffer;
	return SNEP_SUCCESS_CODE;
}


/*****************************************************************************/
/*                  SERVER (manage IN/OUT NDEF message)									  	 */
/*****************************************************************************/

void Server_SNEP (void)
{	
	/* check if we have received a message */
  if(IS_IN_MESS_FLAG(SNEPServer.LLC->Flags))
  {
    CLEAR_IN_MESS_FLAG(SNEPServer.LLC->Flags); 
    
    if( NdefFileTransmissionOnGoing == true)
    {  
      memcpy(&SNEP_Receive_Buffer[SNEPServer.Offset], &SNEPServer.LLC->InMessage[0], SNEPServer.LLC->InLength);
			SNEPServer.Offset+=SNEPServer.LLC->InLength;
      if(SNEPServer.Offset>=(SNEPServer.MessLen - SNEP_OFFSET_PUT_NDEF))
      {
        SNEP_SendRES(SNEP_RES_SUCCESS);		
        NdefFileTransmissionOnGoing = false;
      }
    }
    else
    {  
  
      if(SNEPServer.LLC->InMessage[SNEP_OFFSET_VERSION]!=SNEP_SERVER_VERSION)
      {
        SNEP_SendRES(SNEP_RES_UNSUPPORTED_VERS);
      }
      else
      {
        switch(SNEPServer.LLC->InMessage[SNEP_OFFSET_CODE_CMD])
        {
          /******************** SNEP Request Message ***********************/					
          case SNEP_CMD_CONTINUE	:
            if(SNEPServer.Offset<SNEPServer.MessLen)
            {
              memcpy(&SNEP_Receive_Buffer[SNEPServer.Offset], &SNEPServer.LLC->InMessage[0], SNEPServer.LLC->InLength);
              SNEPServer.Offset+=SNEPServer.LLC->InLength;
              if(SNEPServer.Offset>=SNEPServer.MessLen)
                SNEP_SendRES(SNEP_RES_SUCCESS);		
            }
            break;
				
          case SNEP_CMD_GET	:
						SNEP_SendRES(SNEP_RES_NOT_IMPLEMENT);  
            break;
						
          case SNEP_CMD_PUT	:		
            SNEPServer.MessLen=__REV(*((uint32_t*) &SNEPServer.LLC->InMessage[SNEP_OFFSET_LENGTH]));

#ifndef SNEP_STATIC
            if(SNEP_Receive_Buffer != NULL)
            {
              free(SNEP_Receive_Buffer);
              SNEP_Receive_Buffer = NULL;
            }
            SNEP_Receive_Buffer = malloc(SNEPServer.MessLen + 10);

            if(SNEP_Receive_Buffer == NULL)
            {
                SNEP_SendRES(SNEP_RES_REJECT);
                //while(1); /* For debug purpose */
            }
#endif
            if(SNEPServer.MessLen < (SNEPServer.LLC->MIU))
            {
              SNEP_SendRES(SNEP_RES_SUCCESS);		
              memcpy(SNEP_Receive_Buffer, &SNEPServer.LLC->InMessage[SNEP_OFFSET_PUT_NDEF], SNEPServer.MessLen);
            }
            else
            {
#ifdef SNEP_STATIC
              if(SNEPServer.MessLen<=SNEP_MAX_SIZE_BUFFER)
#endif
              {	
                SNEP_SendRES(SNEP_RES_CONTINUE);               
                memcpy(SNEP_Receive_Buffer, &SNEPServer.LLC->InMessage[SNEP_OFFSET_PUT_NDEF], SNEPServer.LLC->InLength-SNEP_OFFSET_PUT_NDEF);
                SNEPServer.Offset=SNEPServer.LLC->InLength-SNEP_OFFSET_PUT_NDEF;	
                NdefFileTransmissionOnGoing = true;              
              }
#ifdef SNEP_STATIC
              else
              {
                SNEP_SendRES(SNEP_RES_REJECT);
                while(1); /* For debug purpose */
              }	
#endif
            }
            break;
						
          case SNEP_CMD_REJECT:
            break;
					
					/******************** end of SNEP Request Message ***********************/
					
					/******************** start of SNEP Request Message ***********************/
					
					case SNEP_RES_SUCCESS:
						SNEP_Message2SendState = SNEP_MESSAGE_RECEIVED;
						NdefFileSend = true;
            break;
					
					case SNEP_RES_CONTINUE:
						Continue = true;
            break;
        
          default:
            break;
        }       
      }	  
    }
  }	
	else if( SNEP_Message2SendState == SNEP_MESSAGE2SEND) /* check if we want to send a message */ 
	{	
		/* Are we already connected to the remote SNEP service ? */
		if( !IS_CONNECTION_FLAG(SNEPServer.LLC->Flags) )
		{	
			/* Send connect command SAP to LLCP server */
			LLCP_Connect( SERVER_SNEP, SNEPServer.LLC );		
		}
		else if( IS_CONNECTION_FLAG(SNEPServer.LLC->Flags) &&  SNEPServer.LLC->DSAP != SERVER_SNEP)
		{
			/* send disconnect command because we are not connected to remote snep service */
			LLCP_Disconnect( SNEPServer.LLC);
		}
		else /* We are connected, we can send the message */
		{
			if( SNEPServer.MessLen <= SNEPServer.LLC->MIU) // we need to divide
			{	
				SNEP_SendPutCMD( SNEP_Send_Buffer, SNEPServer.MessLen);
				SNEP_Message2SendState = SNEP_MESSAGE_SENT;
        free(SNEP_Send_Buffer);
        SNEP_Send_Buffer = NULL;
			}
			else
			{
				SNEP_SendPutCMD( SNEP_Send_Buffer, SNEPServer.LLC->MIU );
				SNEPServer.Offset = SNEPServer.LLC->MIU;
				SNEP_Message2SendState = SNEP_MESSAGE_SENDINGONGOING;
				Continue = false;
			}
		}		
	}
	else if( SNEP_Message2SendState == SNEP_MESSAGE_SENDINGONGOING) 
	{
		if( Continue )
		{	
			if( (SNEPServer.MessLen-SNEPServer.Offset) <= SNEPServer.LLC->MIU) // this is the last
			{	
				SNEP_SendContinue( &SNEP_Send_Buffer[SNEPServer.Offset], (SNEPServer.MessLen-SNEPServer.Offset));
				SNEP_Message2SendState = SNEP_MESSAGE_SENT;
				Continue = false;
        free(SNEP_Send_Buffer);
        SNEP_Send_Buffer = NULL;
			}
			else
			{
				SNEP_SendContinue( &SNEP_Send_Buffer[SNEPServer.Offset], SNEPServer.LLC->MIU );
				SNEP_Message2SendState = SNEP_MESSAGE_SENDINGONGOING;
				SNEPServer.Offset += SNEPServer.LLC->MIU;
				Continue = true;
			}
		}			
	}
	else if( SNEP_Message2SendState == SNEP_MESSAGE_RECEIVED)  /* No message to send or receive check we are no more connected */ 
	{
		if( IS_CONNECTION_FLAG(SNEPServer.LLC->Flags) )
		{	
			/* send disconnect command */
			LLCP_Disconnect( SNEPServer.LLC);
		}	
		SNEP_Message2SendState = SNEP_NO_MESSAGE;
	}
}

/* Request command */

#if 0
static uint8_t SNEP_SendGetCMD( void  )
{
	memcpy(SNEPServer.LLC->OutMessage,SNEP_NotIplemented,0x06);
	SNEPServer.LLC->OutLength=0x06;
	SET_OUT_MESS_FLAG(SNEPServer.LLC->Flags);
	
	return LLCP_SUCCESS_CODE;
}
#endif

static uint8_t SNEP_SendPutCMD( uint8_t *pBuffer, uint16_t  size)
{	
	SNEPServer.LLC->OutLength = size;
	memcpy(SNEPServer.LLC->OutMessage, pBuffer, SNEPServer.LLC->OutLength );
	
	SET_OUT_MESS_FLAG(SNEPServer.LLC->Flags);
	
	return LLCP_SUCCESS_CODE;
}

static uint8_t SNEP_SendContinue( uint8_t *pBuffer, uint16_t  size  )
{
	SNEPServer.LLC->OutLength = size;
	memcpy(SNEPServer.LLC->OutMessage, pBuffer, SNEPServer.LLC->OutLength );
	
	SET_OUT_MESS_FLAG(SNEPServer.LLC->Flags);
	
	return LLCP_SUCCESS_CODE;
}

#if 0
static uint8_t SNEP_SendRejectCMD( void  )
{
	memcpy(SNEPServer.LLC->OutMessage,SNEP_NotIplemented,0x06);
	SNEPServer.LLC->OutLength=0x06;
	SET_OUT_MESS_FLAG(SNEPServer.LLC->Flags);
	
	return LLCP_SUCCESS_CODE;
}
#endif

/* Response command */

static uint8_t SNEP_SendRES( uint8_t CMD )
{	
	uint8_t SNEP_Header[0x06]={0x00,0x00,0x00,0x00,0x00,0x00};
	SNEP_Header[SNEP_OFFSET_VERSION]=SNEPServer.Version;
	SNEP_Header[SNEP_OFFSET_CODE_CMD]=CMD;
	
  SNEPServer.LLC->OutLength=0x06;
	memcpy(SNEPServer.LLC->OutMessage,SNEP_Header,SNEPServer.LLC->OutLength);		
  SET_OUT_MESS_FLAG(SNEPServer.LLC->Flags);
	
	if( CMD == SNEP_RES_SUCCESS)
		NdefFileReceive = true;
	
	return LLCP_SUCCESS_CODE;
}

void SNEP_Free(void)
{
  if(SNEP_Receive_Buffer != NULL)
  {
    free(SNEP_Receive_Buffer);
    SNEP_Receive_Buffer = NULL;
  }
  if(SNEP_Send_Buffer != NULL)
  {
    free(SNEP_Send_Buffer);
    SNEP_Send_Buffer = NULL;
  }
}

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE***/
