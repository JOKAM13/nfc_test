/**
	*****************************************************************************
	* @file 		lib_llcp.c
	* @author  	MMY Application Team 
	* @version 	1.0.0
	* @date			15/07/2014
	* @brief		LLCP Library
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2015 STMicroelectronics
  *
  * Licensed under Software License Agreement (ST95HF), (the �License�); 
  *
  * You may not use this file except in compliance with the License, including, 
  * but not limited to, the specific language governing permissions 
  * and limitations therein.
  *
  * THIS SOFTWARE IS PROVIDED �AS IS� WITH ALL FAULTS. LICENSOR MAKES NO, 
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

#include "lib_llcp.h"
#include "rfal_utils.h"


/* Function to map LLCP and NFCDEP layers */
extern void (*NFCDepCallBack)(uint8_t*pData, uint16_t *nbByte);
extern void (*LLCPPaxCallBack)(uint8_t*pData, uint16_t*nbByte);

bool LLCPSymActivated = false;
extern bool TargetMode;

static LLCP_Service						AvailableService[LLCP_NB_SERVICE_MAX];
static uint8_t								NbAvailableService=0;
static LLCP_LLC								Connections[LLCP_NB_CONNECTION_MAX];
static uint8_t								NbConnections=0;

/* Header PDU */
uint8_t ptype,ssap,dsap;

/* To store LLCP characteristics */
static LLCP_ConfigStruct 			LLCP_LocalConfig;
static LLCP_ConfigStruct 			LLCP_RemoteConfig;


static void LLCP_DecodeHeader(uint16_t * plength, uint8_t * Buffer, uint8_t * dsap, uint8_t * ptype,uint8_t * ssap);
static void WritePDUHeader(uint8_t DSAP, uint8_t PTYPE, uint8_t SSAP, uint8_t * Buffer );
static int8_t LLCP_GetService( uint8_t SAP);
static int8_t LLCP_GetLLC( uint8_t DSAP, uint8_t SSAP);
#if 0
static int8_t LLCP_GetSAP( char * uri);
#endif

static void LLCP_DecodeHeader(uint16_t * plength, uint8_t * Buffer, uint8_t * dsap,uint8_t * ptype,uint8_t * ssap)
{
	*dsap= (Buffer[0] & LLCP_DSAP_MASK) >>2;
	*ssap= (Buffer[1] & LLCP_SSAP_MASK);
	*ptype=((Buffer[0] & (LLCP_DSAP_MASK^0xFF)) <<2 )+((Buffer[1] & (LLCP_SSAP_MASK^0xFF))>>6);	
}

static void WritePDUHeader(uint8_t DSAP,uint8_t PTYPE,uint8_t SSAP,uint8_t * Buffer )
{	
	Buffer[0]=(DSAP << 2 )+ ((PTYPE & 0x0C) >> 2	);
	Buffer[1]=SSAP + ((PTYPE & 0x03) << 6	);	
}


static int8_t LLCP_GetService( uint8_t SAP)
{
	uint8_t i;
	for(i=0;i<NbAvailableService;i++)
	{
		if(SAP==AvailableService[i].SAP)
		{
			return i;
		}
	}
	return -1;
}


static int8_t LLCP_GetLLC( uint8_t DSAP,uint8_t SSAP)
{
	uint8_t i;
	for(i=0;i<NbConnections;i++)
	{
		if(DSAP==Connections[i].DSAP && SSAP==Connections[i].SSAP)
		{
			return i;
		}
	}
	
	return -1;
}

#if 0
static int8_t LLCP_GetSAP(char * uri)
{
	uint8_t i;
	for(i=0;i<NbAvailableService;i++)
	{
		if(strcmp(uri, AvailableService[i].uri))
		{
			return i;
		}
	}
	return -1;
}
#endif

uint8_t LLCP_Init( void )
{	
	/* insure each time we call init function it will be as if it was the first time */
	NbConnections = 0;
	NbAvailableService=0; /* Service connected through LLCP */
	
	/* Initialize our LLCP parameter */
	LLCP_LocalConfig.Version=LLCP_VERSION;
	LLCP_LocalConfig.MIUX=0;  /* MIU = 128 + MIUX */
	LLCP_LocalConfig.WKS=LLCP_WKS;
	LLCP_LocalConfig.LTO=LLCP_LTO;
  LLCP_LocalConfig.RW=LLCP_RW;
	LLCP_LocalConfig.OPT=LLCP_CLASS;
	
	/* to store Client LLCP parameter */
	LLCP_RemoteConfig.Version=0;
	LLCP_RemoteConfig.MIUX=0; /* default is 128, MIU = 128 + MIUX */
	LLCP_RemoteConfig.WKS=0;
	LLCP_RemoteConfig.LTO=10;  /* default 100ms if not transmitted */
	LLCP_RemoteConfig.OPT=0;
	
	/* map LLCP function with NFCDEP layer */
//	LLCPPaxCallBack = LLCP_SendReceiveTLVPAX;
//	NFCDepCallBack = LLCP_Reply;

	/* First LLCP structure is LLC itself */
	Connections[NbConnections].SSAP=0x00;
	Connections[NbConnections].DSAP=0x00;		/* local LLC will communicate only with remote LLC */
	Connections[NbConnections].V_S = 0x00;
	Connections[NbConnections].V_SA = 0x00;
	Connections[NbConnections].V_R = 0x00;
	Connections[NbConnections].V_RA = 0x00;
	Connections[NbConnections].Flags=0;
	Connections[NbConnections].MIU=LLCP_DEFAULT_MIU+LLCP_LocalConfig.MIUX;
	Connections[NbConnections].RW=0;
	NbConnections++;
	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_AddService(LLCP_Service NewService)
{
	if(NbAvailableService<LLCP_NB_SERVICE_MAX)
	{
		memcpy(&AvailableService[NbAvailableService],
							&NewService,
							sizeof(LLCP_Service));
		NbAvailableService++;
		
		return LLCP_SUCCESS_CODE;
	}
	else
	{
		return LLCP_ERRORCODE_FULLSERVICE;
	}
}

uint8_t LLCP_RemoveService(uint8_t SAP)
{
  uint8_t i;
  for(i=0;i<LLCP_NB_SERVICE_MAX;i++)
  {
    if( AvailableService[i].SAP == SAP)
    {
      memcpy(&AvailableService[i],&AvailableService[NbAvailableService-1],sizeof(LLCP_Service));
      NbAvailableService--;
      return LLCP_SUCCESS_CODE;
    }
  }
		return LLCP_ERRORCODE_DEFAULT;
}

uint8_t LLCP_ConnectService( uint8_t SAP, LLCP_LLC **LLC )
{
	uint8_t i =0;
	
	if(NbConnections<LLCP_NB_CONNECTION_MAX)
	{		
		Connections[NbConnections].SSAP=SAP; /* Local */
		Connections[NbConnections].PTYPE = 0;
		Connections[NbConnections].DSAP = 0;  /* Remote */
		memset(Connections[NbConnections].LLCInternalBuff,0,512);
		Connections[NbConnections].LLCBuffLength = 0;	
		Connections[NbConnections].V_S = 0x00;
		Connections[NbConnections].V_SA = 0x00;
		Connections[NbConnections].V_R = 0x00;
		Connections[NbConnections].V_RA = 0x00;
		Connections[NbConnections].Flags=0;
		Connections[NbConnections].MIU=LLCP_DEFAULT_MIU+LLCP_LocalConfig.MIUX;
		Connections[NbConnections].RW=0;
		*LLC=&Connections[NbConnections];
		
		for(i=0; i<LLCP_NB_SERVICE_MAX; i++)
		{
			if(AvailableService[i].SAP==SAP)
			{	
				AvailableService[i].Connect(*LLC);
				NbConnections++;
				return LLCP_SUCCESS_CODE;
			}
		}		
		return LLCP_ERRORCODE_DEFAULT;
	}
	else
	{
		/* No free connection send DM message */
		return LLCP_ERRORCODE_NO_CONN_LEFT;
	}
}


uint8_t LLCP_RemoveConnection( LLCP_LLC *LLC )
{
	int8_t ServiceIndex;
	
	ServiceIndex = LLCP_GetService(LLC->SSAP);
	
	LLC->SSAP = 0;  /* Local */
	LLC->PTYPE = 0;
	LLC->DSAP = 0;  /* Remote */
	memset(LLC->LLCInternalBuff,0,512);
	LLC->LLCBuffLength = 0;
	LLC->Flags = 0;
	LLC->V_S = 0;  /* Send sequence number local LLCP */
	LLC->V_SA = 0; /* Send sequence numbered acknowledged by remote LLCP */
	LLC->V_R = 0;  /* Receive sequence number local LLCP */
	LLC->V_RA = 0; /* Receive sequence numbered acknowledged by remote LLCP */
	LLC->MIU = 0;
	LLC->RW = 0;
	/* Buffer for upper layer */
	LLC->InMessage = NULL;
	LLC->InLength = 0;
	LLC->OutMessage = NULL;
	LLC->OutLength = 0;
	
	AvailableService[ServiceIndex].Disconnect(LLC);
		
	memcpy(LLC,&Connections[NbConnections-1],sizeof(LLCP_LLC));
	NbConnections--;
	
	return LLCP_SUCCESS_CODE;
}


/*****************************************************************************/
/*														MAC LINK																			 */
/*****************************************************************************/


uint8_t LLCP_NFCDEPdisconnect(void)
{
	uint8_t i=0;
	
	for(i=0;i<NbConnections;i++)
	{
		if( Connections[i].SSAP != 0)
			AvailableService[LLCP_GetService( Connections[i].SSAP)].Disconnect(&Connections[i]);
	}
	
	NbConnections=0;
	LLCPSymActivated = false;
	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_Connect( uint8_t DSAP , LLCP_LLC *LLC)
{	
	LLC->DSAP = DSAP;
	
	if(!IS_ASK_CONN_FLAG(LLC->Flags))
	{
		SET_ASK_CONN_FLAG(LLC->Flags);
		LLC->ConnectDisconnect = LLCP_CONNECTION_CLAIM;
		SET_OUT_MESS_FLAG(LLC->Flags);
		return LLCP_SUCCESS_CODE;
  }		
	
	return LLCP_ERRORCODE_ALREADY_ASK;
}


uint8_t LLCP_Disconnect( LLCP_LLC *LLC)
{
	if(!IS_ASK_DISC_FLAG(LLC->Flags))
	{
		SET_ASK_DISC_FLAG(LLC->Flags);
		LLC->ConnectDisconnect = LLCP_DISCONNECTION_CLAIM;
		SET_OUT_MESS_FLAG(LLC->Flags);
    return LLCP_SUCCESS_CODE;
	}	
	return LLCP_ERRORCODE_ALREADY_ASK;
}

/*****************************************************************************/
/*														Initiator mode																 */
/*****************************************************************************/
#include "st_errno.h"
ReturnCode nfcDepTxRx( const uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxBuf, uint16_t rxBufSize, uint16_t *rxActLen );
#ifdef DEBUG_LLCP
uint8_t DBG_LLCP_TX[10][20];
uint32_t DBG_LLCP_TX_index = 0;
#endif
/* Function call periodicly only in Initiator mode*/
uint8_t LLCP_Send ( void )
{
	uint8_t i = 0;
	
  for(i=0;i<NbConnections;i++)
  {
		/* First do we have an internal LLCP data to send */
		if(IS_LLCP_MESS_FLAG( Connections[i].Flags))
		{					
			break;				
		}
		/* Second do we have a message from upper layer to send */
		else if(IS_OUT_MESS_FLAG( Connections[i].Flags))
    { 
			if(Connections[i].ConnectDisconnect == LLCP_CONNECTION_CLAIM)
			{	
				LLCP_SendCONNECT(&Connections[i]);
				Connections[i].ConnectDisconnect = LLCP_NO_ACTION;		
			}
			else if(Connections[i].ConnectDisconnect == LLCP_DISCONNECTION_CLAIM)
			{
				LLCP_SendDISC(&Connections[i]);
				Connections[i].ConnectDisconnect = LLCP_NO_ACTION;
			}
			else			
				LLCP_SendI(&Connections[i]);
			
			break;	
    }
	}

	if(i>=NbConnections)
	{
		/* No message from upper layer to send, or LLCP answer to send, SYMM frame must be send */
		LLCP_SendSYMM();
		i = LLCP_GetLLC(0x00,0x00);
	} 
#ifdef DEBUG_LLCP
  else if(DBG_LLCP_TX_index < 10) {
      memcpy(&DBG_LLCP_TX[DBG_LLCP_TX_index++],Connections[i].LLCInternalBuff,
                Connections[i].LLCBuffLength > 20 ? 20 : Connections[i].LLCBuffLength);
  }
#endif

//	if (PCDNFCDEP_Dep (0x00, Connections[i].LLCInternalBuff, &(Connections[i].LLCBuffLength) ) == PCDNFCDEP_SUCESSSCODE)
  if(nfcDepTxRx(Connections[i].LLCInternalBuff,Connections[i].LLCBuffLength,
                      Connections[i].LLCInternalBuff,512,
                      &Connections[i].LLCBuffLength) == ERR_NONE)
	{		
		CLEAR_LLCP_MESS_FLAG( Connections[i].Flags);
		LLCP_HandlePDU(&(Connections[i].LLCBuffLength),Connections[i].LLCInternalBuff);			
		return LLCP_SUCCESS_CODE;
	}
	else
		return LLCP_ERRORCODE_DEFAULT;	

} 

//#define DBG_PROBE_INIT 1

#ifdef DBG_PROBE_INIT
#define DBG_PROBE_INIT(what,nb,length,first) \
  uint8_t DBG_ ## what[nb][length]; \
  uint32_t DBG_ ## what ## _index = 0; \
  uint8_t DBG_ ## what ## _first = first; \
  uint8_t DBG_ ## what ## _length = length; \
  uint8_t DBG_ ## what ## _nb = nb;

#define DBG_PROBE(what, length) \
  do {  \
      if(((DBG_ ## what ## _first) && ((DBG_ ## what ## _index) < (DBG_ ## what ## _nb))) || (!(DBG_ ## what ## _first))) \
      { \
        memcpy(&DBG_ ## what[(DBG_ ## what ## _index)++][0],what, \
          (length > (DBG_ ## what ## _length))? (DBG_ ## what ## _length) : (length)); \
        if(!(DBG_ ## what ## _first)) \
          DBG_ ## what ## _index = (DBG_ ## what ## _index) >=  (DBG_ ## what ## _nb)? \
            0 :  (DBG_ ## what ## _index); \
      } \
  } while (0);
#else // disable probe
#define DBG_PROBE_INIT(what,nb,length,first)
#define DBG_PROBE(what, length)
#endif
/*****************************************************************************/
/*											   			Target mode					  											 */
/*****************************************************************************/
#ifdef DEBUG_LLCP
uint8_t DBG_LLCP_RX[10][20];
uint32_t DBG_LLCP_RX_index = 0;
#endif
DBG_PROBE_INIT(pSendReceive,10, 20, 1);

/* This function is mapped to the PICC NFCDepCallBack */
/* Function is called only in Target mode when data are received*/
void LLCP_Reply(uint8_t *pSendReceive, uint16_t *nbByte)
{
	uint8_t result;
  uint8_t i =0;
	
  if((*nbByte != 2) || pSendReceive[0] || pSendReceive[1])
  {
    DBG_PROBE(pSendReceive, *nbByte);
  }

	result = LLCP_HandlePDU(nbByte,pSendReceive);
	
	if( result == LLCP_ERRORCODE_REBOOTNEEDED)
	{
		/* Need to stop everything */
	}
	
	for(i=0;i<NbConnections;i++)
  {
		if(IS_LLCP_MESS_FLAG( Connections[i].Flags))
			break;
		else if(IS_OUT_MESS_FLAG( Connections[i].Flags))
		{
			if(Connections[i].ConnectDisconnect == LLCP_CONNECTION_CLAIM)
			{	
				LLCP_SendCONNECT(&Connections[i]);
				Connections[i].ConnectDisconnect = LLCP_NO_ACTION;
				break;
			}
			else if(Connections[i].ConnectDisconnect == LLCP_DISCONNECTION_CLAIM)
			{
				LLCP_SendDISC(&Connections[i]);
				Connections[i].ConnectDisconnect = LLCP_NO_ACTION;	
				break;
			}
			else
			{	
				/* Send I */
				LLCP_SendI(&Connections[i]);
				break;
			}
		}
	}
	
	/* No message to send, SYMM must be sent */
	if(i>= NbConnections)
	{	
		LLCP_SendSYMM();
		i = LLCP_GetLLC(0x00,0x00);
	}
	
	/* Fill NFCDEP answer buffer */
	memcpy(pSendReceive, Connections[i].LLCInternalBuff, Connections[i].LLCBuffLength);
	*nbByte = Connections[i].LLCBuffLength;
	CLEAR_LLCP_MESS_FLAG( Connections[i].Flags);
	
}


/*****************************************************************************/
/*														PDU Handeling																	 */
/*****************************************************************************/

uint8_t LLCP_HandlePDU(	uint16_t  *pLength, uint8_t * Buffer)
{
	int8_t i,index=0, result;
	
	/* DSAP is our local service, SSAP is the remote service */
	LLCP_DecodeHeader(pLength,Buffer,&dsap,&ptype,&ssap);
	
	/* Find LLC structure, exception on connect command */
	if( ptype != LLCP_PTYPE_CONNECT)
	{	
		index = LLCP_GetLLC(ssap,dsap);
	}
	else
	{
		for(i=0;i<NbConnections;i++)
		{
			if(dsap==Connections[i].SSAP)
			{
				index = i;				
				break;				
			}
		}
	}
	
	/* Service not available send a DM message, but need to create a LLC struct for the service not connected...*/
	//if( i==-1)
		//LLCP_SendDM(LLC,LLCP_DM_NOT_CONNECTED);
	
	memcpy(Connections[index].LLCInternalBuff, Buffer, *pLength);
	Connections[index].LLCBuffLength = *pLength;
#ifdef DEBUG_LLCP	
  if(ptype != LLCP_PTYPE_SYMM)
  {
    if(DBG_LLCP_RX_index < 10)
      memcpy(DBG_LLCP_RX[DBG_LLCP_RX_index++],Connections[index].LLCInternalBuff, *pLength > 20 ? 20 : *pLength);
  }
#endif
	switch(ptype)
  {		
		case LLCP_PTYPE_SYMM :
			/* Do Nothing */
			result = LLCP_ReplyToSYMM();
			break;
		case LLCP_PTYPE_PAX:
			result = LLCP_ReplyToPAX(&Connections[index]);
			break;
		case LLCP_PTYPE_AGF:
			result = LLCP_ReplyToAGF(&Connections[index]);
			break;
		case LLCP_PTYPE_UI:
			result = LLCP_ReplyToUI(&Connections[index]);
			break;
		case LLCP_PTYPE_CONNECT:
			result = LLCP_ReplyToCONNECT(&Connections[index],ssap);
			break;
		case LLCP_PTYPE_DISC	:
			result = LLCP_ReplyToDISC(&Connections[index]);
			break;
		case LLCP_PTYPE_CC	:
			result = LLCP_ReplyToCC(&Connections[index]);
			break;
		case LLCP_PTYPE_DM:
			result = LLCP_ReplyToDM(&Connections[index]);
			break;
		case LLCP_PTYPE_FRMR	:
			result = LLCP_ReplyToFRMR(&Connections[index]);
			break;
		case LLCP_PTYPE_SNL:
			result = LLCP_ReplyToSNL(&Connections[index]);
			break;
		case LLCP_PTYPE_I	:
			result = LLCP_ReplyToI(&Connections[index]);
			break;
		case LLCP_PTYPE_RR:
			result = LLCP_ReplyToRR(&Connections[index]);
			break;
		case LLCP_PTYPE_RNR:
			result = LLCP_ReplyToRNR(&Connections[index]);
			break;
		default:
			/* Connection issue must not occured */
			result = LLCP_ERRORCODE_REBOOTNEEDED;
			break;	
	}
	
	return result;
}


/*****************************************************************************/
/*														Reply Fct																			 */
/*****************************************************************************/

uint8_t LLCP_ReplyToSYMM ( )
{   
	LLCPSymActivated = true;
    	
	return LLCP_SUCCESS_CODE;
}

/* PAX : Parameter Exchange */
uint8_t LLCP_ReplyToPAX ( LLCP_LLC * LLC )
{
	
	LLCP_HandleTLVPAX(LLC->LLCInternalBuff,LLC->LLCBuffLength);
	
	if(LLCP_RemoteConfig.MIUX < LLCP_LocalConfig.MIUX)
		LLC->MIU = LLCP_DEFAULT_MIU+LLCP_RemoteConfig.MIUX;
	else
		LLC->MIU = LLCP_DEFAULT_MIU+LLCP_LocalConfig.MIUX;
	
	if(TargetMode)
		return(LLCP_SendSYMM());
	
	return LLCP_SUCCESS_CODE;
}

/* AGF Aggregated Frame */
uint8_t LLCP_ReplyToAGF ( LLCP_LLC * LLC )
{
	/* Not Implemented Yet */
	
	if(TargetMode)
		return(LLCP_SendSYMM());
		
	return LLCP_SUCCESS_CODE;
}

/* UI Unnumbered Information */
uint8_t LLCP_ReplyToUI ( LLCP_LLC * LLC )
{
	memcpy(LLC->InMessage,&(LLC->LLCInternalBuff[LLCP_OFFSET_UI_INFO]), (LLC->LLCBuffLength-LLCP_OFFSET_UI_INFO));
	LLC->InLength=LLC->LLCBuffLength-LLCP_OFFSET_UI_INFO;
	/* Inform upper layer */
  SET_IN_MESS_FLAG(LLC->Flags);
    
  if(TargetMode)
		return(LLCP_SendSYMM());
	
	return LLCP_SUCCESS_CODE;
}


uint8_t LLCP_ReplyToCONNECT ( LLCP_LLC * LLC, uint8_t SSAP)
{
	/* Currently only one	connection to a service */
	if(IS_CONNECTION_FLAG(LLC->Flags))
	{
		LLCP_SendDM(LLC, SSAP, LLCP_DM_CONNECTION_REJECTED);
	}
	else	
	{
		LLC->DSAP=SSAP;
			
		LLC->V_S = 0x00;
		LLC->V_SA = 0x00;
		LLC->V_R = 0x00;
		LLC->V_RA = 0x00;
		LLCP_HandleTLVCONNECT(LLC);
		SET_CONNECTION_FLAG(LLC->Flags);
		
		if(LLCP_RemoteConfig.MIUX < LLCP_LocalConfig.MIUX)
			LLC->MIU = LLCP_DEFAULT_MIU + LLCP_RemoteConfig.MIUX;
		else
			LLC->MIU = LLCP_DEFAULT_MIU + LLCP_LocalConfig.MIUX;
				
		LLCP_SendCC(LLC);
	}
	
	return LLCP_SUCCESS_CODE;
}


uint8_t LLCP_ReplyToDISC ( LLCP_LLC * LLC)
{
	LLCP_SendDM(LLC, LLC->DSAP ,LLCP_DM_DISC_PDU);
	
	return LLCP_SUCCESS_CODE;
}

/* CC Connection Complete */
uint8_t LLCP_ReplyToCC ( LLCP_LLC * LLC )
{
	SET_CONNECTION_FLAG(LLC->Flags);
	CLEAR_ASK_CONN_FLAG(LLC->Flags);
	/* Do we have TLV parameters to update ? */
	if( LLC->LLCBuffLength > 2)   
		LLCP_HandleTLVPAX( &(LLC->LLCInternalBuff[2]), LLC->LLCBuffLength-2);
	
	if(LLCP_RemoteConfig.MIUX < LLCP_LocalConfig.MIUX)
		LLC->MIU = LLCP_DEFAULT_MIU + LLCP_RemoteConfig.MIUX;
	else
		LLC->MIU = LLCP_DEFAULT_MIU + LLCP_LocalConfig.MIUX;	
	
	if(TargetMode)
		return(LLCP_SendSYMM());
	
	return LLCP_SUCCESS_CODE;
}

/* DM Disconnected Mode */
uint8_t LLCP_ReplyToDM ( LLCP_LLC * LLC )
{
	/* information field of DM not managed currently */
	CLEAR_CONNECTION_FLAG(LLC->Flags);
	CLEAR_ASK_DISC_FLAG(LLC->Flags);
	
	if(TargetMode)
		return(LLCP_SendSYMM());
	
	return LLCP_SUCCESS_CODE;
}

/* FRMR Frame Reject */
uint8_t LLCP_ReplyToFRMR ( LLCP_LLC * LLC)
{
	/* Not Implemented yet */
	/* Maybe we can decide to perform reboot */
	
	if(TargetMode)
		return(LLCP_SendSYMM());

	return LLCP_SUCCESS_CODE;
}

/* I Information */
uint8_t LLCP_ReplyToI ( LLCP_LLC * LLC )
{
	LLC->V_R = ((LLC->LLCInternalBuff[2] & 0xF0)>>4)+1;
	memcpy(LLC->InMessage,&(LLC->LLCInternalBuff[LLCP_OFFSET_I_INFO]),LLC->LLCBuffLength-LLCP_OFFSET_I_INFO);
	LLC->InLength=LLC->LLCBuffLength-LLCP_OFFSET_I_INFO;
	/* Inform upper layer */
  SET_IN_MESS_FLAG(LLC->Flags);
    
  /* Reply with RR */
	LLCP_SendRR(LLC);
	
	return LLCP_SUCCESS_CODE;
}

/* RR Receive Ready */
uint8_t LLCP_ReplyToRR ( LLCP_LLC * LLC )
{
	LLC->V_SA=(LLC->LLCInternalBuff[LLCP_OFFSET_SEQ]&LLCP_N_R_MASK);
    
  if(TargetMode)
		return(LLCP_SendSYMM());
	
	return LLCP_SUCCESS_CODE;
}

/* RNR Receive Not Ready */
uint8_t LLCP_ReplyToRNR ( LLCP_LLC * LLC )
{
	LLC->V_SA=(LLC->LLCInternalBuff[LLCP_OFFSET_SEQ]&LLCP_N_R_MASK);
	
	/* must send again the missing I PDU i guess */
	
	return LLCP_SUCCESS_CODE;
}

/* SNL Service Name Lookup */
uint8_t LLCP_ReplyToSNL ( LLCP_LLC * LLC  )
{
	 /* #warning "Not Implemented Yet" */

	return LLCP_SUCCESS_CODE;
}


/*****************************************************************************/
/*														Send Fct																			 */
/*****************************************************************************/

uint8_t LLCP_SendSYMM ( void )
{	
	int8_t index;
	
	index = LLCP_GetLLC(0x00,0x00);
	
	WritePDUHeader (0,LLCP_PTYPE_SYMM,0,Connections[index].LLCInternalBuff);
	Connections[index].LLCBuffLength=0x02;
	SET_LLCP_MESS_FLAG(Connections[index].Flags);
		
	return LLCP_SUCCESS_CODE;

}


uint8_t LLCP_SendUI ( LLCP_LLC * LLC)
{
	/* not implemented yet */
	return LLCP_SUCCESS_CODE;
}


uint8_t LLCP_SendCONNECT ( LLCP_LLC * LLC)
{		
	LLC->V_S = 0x00;
	LLC->V_SA = 0x00;
	LLC->V_R = 0x00;
	LLC->V_RA = 0x00;
	

	WritePDUHeader (LLC->DSAP,LLCP_PTYPE_CONNECT,LLC->SSAP,LLC->LLCInternalBuff);
	LLC->LLCBuffLength=0x02;
	SET_LLCP_MESS_FLAG(LLC->Flags);
	CLEAR_OUT_MESS_FLAG( LLC->Flags);
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_SendDISC ( LLCP_LLC * LLC)
{  
	WritePDUHeader (LLC->DSAP,LLCP_PTYPE_DISC,LLC->SSAP,LLC->LLCInternalBuff);
	LLC->LLCBuffLength=0x02;
	SET_LLCP_MESS_FLAG(LLC->Flags);
	CLEAR_OUT_MESS_FLAG( LLC->Flags);
	return LLCP_SUCCESS_CODE;
}


uint8_t LLCP_SendCC ( LLCP_LLC * LLC )
{
	WritePDUHeader (LLC->DSAP,LLCP_PTYPE_CC,LLC->SSAP,LLC->LLCInternalBuff);
	LLC->LLCBuffLength = 0x02;
	SET_LLCP_MESS_FLAG(LLC->Flags);	
	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_SendDM ( LLCP_LLC * LLC, uint8_t DSAP, uint8_t DM )
{
	WritePDUHeader (DSAP,LLCP_PTYPE_DM,LLC->SSAP,LLC->LLCInternalBuff);
	LLC->LLCInternalBuff[0x02]=DM;
	LLC->LLCBuffLength=0x03;
	SET_LLCP_MESS_FLAG(LLC->Flags);	
		
	/* State machine */
	if( DM == LLCP_DM_DISC_PDU)
		CLEAR_CONNECTION_FLAG(LLC->Flags);
	
	return LLCP_SUCCESS_CODE;
}


uint8_t LLCP_SendFRMR ( LLCP_LLC * LLC, uint8_t Flags, uint8_t Ptype, uint8_t Seq)
{
	/* Not implemented yet */	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_SendI ( LLCP_LLC * LLC)
{
	WritePDUHeader(LLC->DSAP,LLCP_PTYPE_I,LLC->SSAP,LLC->LLCInternalBuff);
	LLC->LLCInternalBuff[0x02]=(LLC->V_R & LLCP_V_R_MASK) + ((LLC->V_S<<4)& LLCP_V_S_MASK);
	LLC->LLCBuffLength=0x03;
	
	/* Can we send all data in one PDU? */
	if( LLC->OutLength <= (LLC->MIU))
	{	
		memcpy(&(LLC->LLCInternalBuff[0x03]),LLC->OutMessage,LLC->OutLength);
		LLC->LLCBuffLength += LLC->OutLength;
		LLC->OutLength = 0;
		/* all data of the message has been sent */
		CLEAR_OUT_MESS_FLAG( LLC->Flags);
	}
	else
	{
		memcpy(&(LLC->LLCInternalBuff[0x03]),LLC->OutMessage,LLC->MIU);
		LLC->LLCBuffLength += LLC->MIU;
		
		LLC->OutLength -= LLC->MIU;
		memcpy(LLC->OutMessage, &LLC->OutMessage[LLC->MIU], LLC->OutLength );
	}
	
	LLC->V_S++;
	SET_LLCP_MESS_FLAG(LLC->Flags);		
	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_SendRR ( LLCP_LLC * LLC )
{	  		
	WritePDUHeader (LLC->DSAP,LLCP_PTYPE_RR,LLC->SSAP, LLC->LLCInternalBuff );
	LLC->LLCInternalBuff[LLCP_OFFSET_SEQ]=LLC->V_R &LLCP_N_R_MASK;
	LLC->V_RA=LLC->V_R;
	LLC->LLCBuffLength=0x03;
	SET_LLCP_MESS_FLAG(LLC->Flags);			
	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_SendRNR 				( LLCP_LLC * LLC)
{
	/* Not implemented yet */	
	return LLCP_SUCCESS_CODE;
}

uint8_t LLCP_SendSNL ( uint8_t * URI)
{
	/* Not implemented yet */	
	return LLCP_SUCCESS_CODE;
}


/*****************************************************************************/
/*														Parameters																		 */
/*****************************************************************************/

uint8_t LLCP_WritePAXTLV( uint8_t * Buffer , uint8_t TLVFlags )
{
	
	uint8_t Length =0;
	
	/*******************************************  VERSION   *********************/
	
	if(IS_TLV_VERSION_FLAG(TLVFlags))
	{
		Buffer[Length++]=LLCP_TLV_VERSION_T;
		Buffer[Length++]=LLCP_TLV_VERSION_L;
		Buffer[Length++]=LLCP_LocalConfig.Version;
	}
	
	/*********************************************  MIUX   *********************/
	
	if(IS_TLV_MIUX_FLAG(TLVFlags))
	{
		Buffer[Length++]=LLCP_TLV_MIUX_T;
		Buffer[Length++]=LLCP_TLV_MIUX_L;
		Buffer[Length++]=(LLCP_LocalConfig.MIUX & 0x0F00) >> 8 ;
		Buffer[Length++]=LLCP_LocalConfig.MIUX & 0x00FF;
	}
	
	/*********************************************  WKS   *********************/
	
	if(IS_TLV_WKS_FLAG(TLVFlags))
	{
		Buffer[Length++]=LLCP_TLV_WKS_T;
		Buffer[Length++]=LLCP_TLV_WKS_L;
		Buffer[Length++]=(LLCP_LocalConfig.WKS & 0xFF00) >> 8 ;
		Buffer[Length++]=(LLCP_LocalConfig.WKS & 0x00FF);
	}
	
	/*********************************************  LTO   *********************/
	if(IS_TLV_LTO_FLAG(TLVFlags))
	{
		Buffer[Length++]=LLCP_TLV_LTO_T;
		Buffer[Length++]=LLCP_TLV_LTO_L;
		Buffer[Length++]=LLCP_LocalConfig.LTO;
	}
  
  /*********************************************  RW   **********************/
	if(IS_TLV_RW_FLAG(TLVFlags))
	{
		Buffer[Length++]=LLCP_TLV_RW_T;
		Buffer[Length++]=LLCP_TLV_RW_L;
		Buffer[Length++]=LLCP_LocalConfig.RW;
	}
	
	/*********************************************  OPT   *********************/
	if(IS_TLV_OPT_FLAG(TLVFlags))
	{
		Buffer[Length++]=LLCP_TLV_OPT_T;
		Buffer[Length++]=LLCP_TLV_OPT_L;
		Buffer[Length++]=LLCP_LocalConfig.OPT;
	}
	
	return Length;
}


uint8_t LLCP_HandleTLVCONNECT(LLCP_LLC * LLC)
{	
	uint16_t Offset = 0;
	
	while(Offset<=LLC->LLCBuffLength){
		switch(LLC->LLCInternalBuff[Offset]){
			case LLCP_TLV_MIUX_T:			/********************* MIUX *************/
				if(LLC->LLCInternalBuff[Offset+1]==LLCP_TLV_MIUX_L)
				{
					LLC->MIU=*(uint16_t *) &(LLC->LLCInternalBuff[Offset+2])+LLCP_DEFAULT_MIU;
					Offset+=LLCP_TLV_MIUX_L+2;
				}
				else
				{
					return LLCP_ERRORCODE_WRONG_TLV;
				}
				break;
			case LLCP_TLV_RW_T :				/********************* RW *************/
				if(LLC->LLCInternalBuff[Offset+1]==LLCP_TLV_RW_L)
				{
					LLC->RW= LLC->LLCInternalBuff[Offset+2];
					Offset+=LLCP_TLV_RW_L+2;
				}
				else
				{
					return LLCP_ERRORCODE_WRONG_TLV;
				}
				break;
			case LLCP_TLV_SN_T :
			  /*	#warning "Not Implemented Yet" */
			
			default:
				return LLCP_ERRORCODE_WRONG_TLV;
						
		}	
	}
		return LLCP_SUCCESS_CODE;
}


void LLCP_HandleTLVPAX( uc8 * Buffer,uc16 Length)
{
	uint16_t Offset = 0;
	
	while(Offset<Length)
	{
		switch(Buffer[Offset])
		{
			case LLCP_TLV_VERSION_T:		/********************* VERSION *************/
				if(Buffer[Offset+1] == 1) /* len check */
				{
					LLCP_RemoteConfig.Version = Buffer[Offset+2];
					/* check we are supporting the version */
					if(LLCP_LocalConfig.Version != LLCP_RemoteConfig.Version) /* check major and minor version */
						while(1); /* For debug purpose */
					Offset+=LLCP_PARAM_VERSION_L;
				}
				break;
				
			case LLCP_TLV_MIUX_T:					/********************* MIUX ******************/
				if(Buffer[Offset+1] == 2) /* len check */
				{
					LLCP_RemoteConfig.MIUX = ((uint16_t)(Buffer[Offset+2])<<8) + Buffer[Offset+3];
					Offset+=LLCP_PARAM_MIUX_L;
				}
				break;
				
			case LLCP_TLV_WKS_T:					/********************* WKS ********************/
				if(Buffer[Offset+1] == 2) /* len check */
				{
					LLCP_RemoteConfig.WKS = ((uint16_t)(Buffer[Offset+2])<<8) + Buffer[Offset+3];
					Offset+=LLCP_PARAM_WKS_L;
				}
				break;
		
			case LLCP_TLV_LTO_T:					/********************* LTO *******************/
				if(Buffer[Offset+1] == 1) /* len check */
				{
					LLCP_RemoteConfig.LTO = Buffer[Offset+2];	
					Offset+=LLCP_PARAM_LTO_L;
				}
				break;
        
      case LLCP_TLV_RW_T:         /********************* RW *******************/
        if(Buffer[Offset+1] == 1) /* len check */
				{
					LLCP_RemoteConfig.RW = Buffer[Offset+2];	
					Offset+=LLCP_PARAM_RW_L;
				}
				break;
        
			case LLCP_TLV_OPT_T :				/********************* OPT ( CLASS ) *************/
				if(Buffer[Offset+1] == 1) /* len check */
				{
					LLCP_RemoteConfig.OPT = Buffer[Offset+2];
					Offset+=LLCP_PARAM_OPT_L;
				}
				break;
			default:
				return;		
		}			
	}
		return;
}

void LLCP_SendReceiveTLVPAX( uint8_t * Buffer,uint16_t* Length)
{
	/* Retrieve Information */
	LLCP_HandleTLVPAX( Buffer, *Length);
	
	/* Send response */
	*Length = LLCP_WritePAXTLV(Buffer,LLCP_TLV_VERSION_F | LLCP_TLV_MIUX_F |
																		LLCP_TLV_WKS_F | LLCP_TLV_LTO_F | LLCP_TLV_OPT_F);
}

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
