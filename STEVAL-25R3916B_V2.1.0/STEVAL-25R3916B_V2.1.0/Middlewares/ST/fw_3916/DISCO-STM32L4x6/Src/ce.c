/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2016 STMicroelectronics, all rights reserved
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

/*! \file
 *
 *  \author 
 *
 *  \brief 
 *
 */
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "ce.h"
#include "rfal_rf.h"
#include "rfal_isoDep.h"
#include "rfal_nfca.h"
#include "logger.h"

/*
******************************************************************************
* LOCAL MACROS
******************************************************************************
*/



/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
void ceNfcaCardemu4A(uint8_t mode, rfalIsoDepTxRxParam *isoDepTxRxParam);
void ceNfcfCardemu3(uint8_t mode, rfalTransceiveContext *ctx);


/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/


#define TX_BUF_LENGTH       (256+3)
static uint8_t txBuf[TX_BUF_LENGTH];

#define RX_BUF_LENGTH       (256+3)
static uint8_t rxBuf[RX_BUF_LENGTH];
static uint16_t rxRcvdLen = 0;
static bool  ceEnabled;


rfalLmConfPA                            configA;
rfalLmConfPB                            configB;
rfalLmConfPF                            configF;
rfalIsoDepAtsParam                      atsParam;
uint8_t                                 histChar[16];
uint32_t                                configMask = 0;
rfalTransceiveContext                   transceiveCtx;

uint8_t                                 emuMode = 0;

rfalIsoDepBufFormat                     *rxBufA;
rfalIsoDepBufFormat                     *txBufA;
bool                                    isRxChainingg;
bool                                    isActivatedA = false;
bool                                    isFirstF_Frame = false;
rfalIsoDepTxRxParam                     isoDepTxRxParam;

bool                                    rxReady = false;
bool                                    txReady = false;


/***************************************************************************/
/*                                                                         */
/***************************************************************************/
void ceInitalize( void )
{
    logUsart("ceInitalize..\n");
    
    transceiveCtx.txBuf = txBuf;
    transceiveCtx.txBufLen = 0;

    transceiveCtx.rxBuf = rxBuf;
    transceiveCtx.rxBufLen = RX_BUF_LENGTH;
    transceiveCtx.rxRcvdLen = &rxRcvdLen;
    
    transceiveCtx.flags = RFAL_TXRX_FLAGS_DEFAULT;
    transceiveCtx.fwt = RFAL_FWT_NONE;
    
    rxBufA = (rfalIsoDepBufFormat*)&rxBuf;
    txBufA = (rfalIsoDepBufFormat*)&txBuf;
    
    isoDepTxRxParam.rxBuf = rxBufA;
    isoDepTxRxParam.rxLen = &rxRcvdLen;
    isoDepTxRxParam.txBuf = txBufA;
    isoDepTxRxParam.ourFSx = RFAL_ISODEP_FSX_KEEP;
    isoDepTxRxParam.isRxChaining = &isRxChainingg;
    isoDepTxRxParam.isTxChaining = false;
    
    //
    configMask = 0;
    emuMode = 0;
    isActivatedA = false;
    isFirstF_Frame = false;
    rxReady = false;
    txReady = false;
    
    ceEnabled = false;
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/

void ceHandler( void )
{   
    bool dataFlag = false;
    ReturnCode retCode;
    
    /* Check whether CE is enabled */
    if( !ceEnabled )
    {
        return;
    }
    
    rfalLmState state = rfalListenGetState(&dataFlag, NULL);

    switch (state)
    {
        // ------------------------------------------------------------------
        // NFC A
        // ------------------------------------------------------------------
        //
        case RFAL_LM_STATE_ACTIVE_A:
        case RFAL_LM_STATE_ACTIVE_Ax:
        {
            if(dataFlag == true)
            {
                if (rfalIsoDepIsRats(rxBuf, rfalConvBitsToBytes(rxRcvdLen)))
                {
                    // enter next already state 
                    rfalListenSetState(RFAL_LM_STATE_CARDEMU_4A);

                    // prepare for RATS
                    rfalIsoDepListenActvParam rxParam;
                    rxParam.rxBuf = rxBufA;
                    rxParam.rxLen = &rxRcvdLen;
                    rxParam.isoDepDev = NULL;
                    rxParam.isRxChaining = &isRxChainingg;
                    //
                    isoDepTxRxParam.FSx = rfalIsoDepFSxI2FSx(atsParam.fsci);
                    rfalIsoDepInitialize(); /* Initialize all isodep params */
                    rfalIsoDepListenStartActivation( &atsParam, NULL, rxBuf, rxRcvdLen, rxParam );
                }
                else if (true == rfalNfcaListenerIsSleepReq( rxBuf, rfalConvBitsToBytes(rxRcvdLen) ) )
                {
                    rfalListenSleepStart( RFAL_LM_STATE_SLEEP_A , rxBuf, RX_BUF_LENGTH, &rxRcvdLen );
                }
            }
            break;        
        }
        //
        case RFAL_LM_STATE_CARDEMU_4A :
        {
            if(!isActivatedA)
            {   // finish card activation sequence 
                retCode = rfalIsoDepListenGetActivationStatus();
                if (retCode != RFAL_ERR_BUSY)
                {
                    //
                    switch(retCode)
                    {                    
                        case RFAL_ERR_LINK_LOSS:
                            rfalListenStop();
                            rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                            isActivatedA = false;
                            break;
                    
                        case RFAL_ERR_NONE:
                            isActivatedA = true;
                            break;
    
                        // all other error cases are simple ignored ..
                        default:
                            break;
                    }
                }
            }else
            {   // process command internal
                if(CARDEMULATION_PROCESS_INTERNAL == (emuMode & CARDEMULATION_PROCESS_INTERNAL))
                {   
                    switch (rfalIsoDepGetTransceiveStatus())
                    {
                        case RFAL_ERR_SLEEP_REQ:
                            retCode = rfalListenSleepStart( RFAL_LM_STATE_SLEEP_A , rxBuf, RX_BUF_LENGTH, &rxRcvdLen );
                            rxReady = txReady = isActivatedA = false;
                            if (RFAL_ERR_LINK_LOSS == retCode)
                            {
                                rfalListenStop();
                                rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                            }
                            break;
                        case RFAL_ERR_LINK_LOSS:
                            rfalListenStop();
                            rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                            rxReady = txReady = isActivatedA = false;                        
                            break;
                            
                        case RFAL_ERR_AGAIN:
                            // handle RX chaining..                        
                            // copy RX to my buffer                        
                            // send via USB to Host -> blocking
                            
                            //rfalIsoDepStartTransceive(isoDepTxRxParam);
                            break;
                            
                        case RFAL_ERR_NONE:
                            ceNfcaCardemu4A(emuMode, &isoDepTxRxParam);
                            break;
                            
                        // all other error cases are simple ignored ..
                        default:
                            break;
                    }
                }
            }
            break;
        }
          
        // ------------------------------------------------------------------
        // NFC F
        // ------------------------------------------------------------------
        //
        case RFAL_LM_STATE_READY_F:            
        {
            if(dataFlag == true)        
            {   // data is already received, we can go on ..
                rfalListenSetState(RFAL_LM_STATE_CARDEMU_3);
                isFirstF_Frame = true;
            }
            break;
        }
        //
        case RFAL_LM_STATE_CARDEMU_3  :
        {
            if(isFirstF_Frame){
              ceNfcfCardemu3(emuMode, &transceiveCtx);
              
              // NOTE: isFirstF_Frame is cleared after the first transmit is done.
              // For this mode this transmit is done within the rfalStartTransceive 
              // function in ceNfcfCardemu3(..)              
              
            }else{
            
              // IMPORTANT: We can not call rfalGetTransceiveStatus before we did the first
              // call rfalStartTransceive
              retCode = rfalGetTransceiveStatus();            
              if (retCode != RFAL_ERR_BUSY)
              {
                  switch(retCode)
                  {                    
                      case RFAL_ERR_LINK_LOSS:
                          rfalListenStop();
                          rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                          rxReady = txReady = false;
                          break;
                  
                      case RFAL_ERR_NONE:
                          ceNfcfCardemu3(emuMode, &transceiveCtx);
                          break;

                      // all other error cases are simple ignored ..
                      default:
                          break;
                  }
              }
            }
            break;
        }

//        case RFAL_LM_STATE_NOT_INIT   :
//        case RFAL_LM_STATE_POWER_OFF  :
//        case RFAL_LM_STATE_IDLE       :
//        case RFAL_LM_STATE_READY_A    :
//        case RFAL_LM_STATE_TARGET_A   :
//        case RFAL_LM_STATE_TARGET_F   :
//        case RFAL_LM_STATE_SLEEP_A    :
//        case RFAL_LM_STATE_SLEEP_B    :
//        case RFAL_LM_STATE_SLEEP_AF   :
//        case RFAL_LM_STATE_READY_Ax   :
        default:
            break;
    }   


    
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
ReturnCode ceStart(const uint8_t *rxData, const uint16_t rxSize)
{
    logUsart("ceInitalize..\n");

    //
    // rxData holds the emuMode and 4 TLV data sets (with TAG 4)
    //
    //   0         1              ..             ..             ..
    // | EmuMode | TLV Config A | TLV Config B | TLV Config F | ATS |
        
    //
    uint8_t *config[4];
    uint32_t maskValues[4] = {RFAL_LM_MASK_NFCA, RFAL_LM_MASK_NFCB, RFAL_LM_MASK_NFCF, 0};
    uint16_t offset = 0;
    configMask = 0;

    // get emu mode and increase offset to automatically 
    // remove the emuMode byte when not needed anymore
    emuMode = *rxData;
    offset++;
    
    // extract config data
    for (int i = 0; i < 4; i++)
    {
        if (i < rxSize)
        {
            if (rxData[(i*2) + 1 + offset] > 0)
            {
                //
                config[i] = (uint8_t *)&rxData[(i*2) + 2 + offset];
                //
                configMask |= maskValues[i];
                offset += rxData[(i*2) + 1  + offset];
            }else
            {
                config[i] = NULL;
            }
        }
    }
    
    // store config's for later usage
    (config[0] != NULL) ? RFAL_MEMCPY(&configA, config[0], sizeof(configA)) : RFAL_MEMSET(&configA, 0, sizeof(configA));
    (config[1] != NULL) ? RFAL_MEMCPY(&configB, config[1], sizeof(configB)) : RFAL_MEMSET(&configB, 0, sizeof(configB));
    (config[2] != NULL) ? RFAL_MEMCPY(&configF, config[2], sizeof(configF)) : RFAL_MEMSET(&configF, 0, sizeof(configF));
    
    // finally check if ATS is received as well..
    if(config[3] != NULL)
    {
        atsParam.fsci = config[3][0];
        atsParam.fwi = config[3][1];
        atsParam.sfgi = config[3][2];
        atsParam.ta = config[3][3];
        atsParam.hbLen = config[3][4];
        atsParam.didSupport = false;
        RFAL_MEMCPY(histChar, &config[3][5], atsParam.hbLen);
        atsParam.hb = histChar;
    }else
    {
        RFAL_MEMSET(&atsParam, 0, sizeof(atsParam));
    }
    
    // .. and go ..
    ceEnabled = true;
    rxRcvdLen = 0;
    return rfalListenStart (configMask, 
                    (rfalLmConfPA *)config[0], 
                    (rfalLmConfPB *)config[1], 
                    (rfalLmConfPF *)config[2], 
                    rxBuf, 
                    rfalConvBytesToBits(RX_BUF_LENGTH), 
                    &rxRcvdLen);    
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
ReturnCode ceStop( void )
{
    rfalListenStop();
    ceInitalize();
    return RFAL_ERR_NONE;
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/

ReturnCode ceGetRx(const uint8_t cmd, uint8_t *txData, uint16_t *txSize)
{
    ReturnCode err = RFAL_ERR_NOTFOUND;
    switch (cmd)
    {
        case CARDEMULATION_CMD_GET_RX_A:
            if(isActivatedA)
            {                
                err = rfalIsoDepGetTransceiveStatus();
               
                switch (err)
                {
                    case RFAL_ERR_SLEEP_REQ:
                        /* FIXME: RFAL_ERR_SLEEP_REQ is signaled after S(DSL)
                         * response was sent. Issuing SleepStart will
                         * re-enable automtatic responses to WUPA/ALL_REQ.
                         * ceGetRx() gets triggered by USB (every few ms).
                         * Calling SleepStart() here may be too late.*/
                        err = rfalListenSleepStart( RFAL_LM_STATE_SLEEP_A , rxBuf, RX_BUF_LENGTH, &rxRcvdLen );
                        rxReady = txReady = isActivatedA = false;
                        if (RFAL_ERR_LINK_LOSS == err)
                        {
                            rfalListenStop();
                            rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                        }
                        break;
                    case RFAL_ERR_LINK_LOSS:
                    default:
                        rfalListenStop();
                        rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                        rxReady = txReady = isActivatedA = false;                        
                        break;
                    
                    case RFAL_ERR_BUSY:           /* Transceive ongoing                              */
                    case RFAL_ERR_AGAIN:          /* Chaining packet received - handling to be added */
                        break;
                        
                    case RFAL_ERR_NONE:
                        RFAL_MEMCPY(txData, isoDepTxRxParam.rxBuf->inf, *isoDepTxRxParam.rxLen);
                        *txSize = *isoDepTxRxParam.rxLen;
                            
                        rxReady = false;
                        txReady = true;
                        break;
                }

            }
            break;
            
        case CARDEMULATION_CMD_GET_RX_B:
            break;
            
        case CARDEMULATION_CMD_GET_RX_F:
            if(isFirstF_Frame)
            {
              // IMPORTANT: We can not call rfalGetTransceiveStatus before we did the first
              // call rfalStartTransceive
              
              err = RFAL_ERR_NONE;
              RFAL_MEMCPY(txData, transceiveCtx.rxBuf, *transceiveCtx.rxRcvdLen);
              // this will transmit payload without CRC
              *txSize = txData[0];
              
              rxReady = false;
              txReady = true;
              
              // NOTE: isFirstF_Frame is cleared after the first transmit is done.
              // For this mode this transmit is done within the rfalStartTransceive 
              // function in ceSetTx(..)
              
            }else
            {
              err = rfalGetTransceiveStatus();
              switch( err )
              {             
                  
                  case RFAL_ERR_LINK_LOSS:
                  default:
                      rfalListenStop();
                      rfalListenStart(configMask, &configA, &configB, &configF, rxBuf, rfalConvBytesToBits(RX_BUF_LENGTH), &rxRcvdLen);
                      rxReady = txReady = false;
                      break;
                  
                  case RFAL_ERR_BUSY:           /* Transceive ongoing   */
                        break;
              
                  case RFAL_ERR_NONE:
                      RFAL_MEMCPY(txData, transceiveCtx.rxBuf, *transceiveCtx.rxRcvdLen);
                      // this will transmit payload without CRC
                      *txSize = txData[0];
      
                      rxReady = false;
                      txReady = true;
                      break;
              }
            }
            break;
            
        default:
            break;
    }
    
    return err;
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
ReturnCode ceSetTx(const uint8_t cmd, const uint8_t * rxData, const uint16_t rxSize)
{
    ReturnCode err = RFAL_ERR_NOTFOUND;
    switch (cmd)
    {
        case CARDEMULATION_CMD_SET_TX_A:
            if (txReady)
            {
              
                RFAL_MEMCPY(isoDepTxRxParam.txBuf->inf, rxData, rxSize);
                isoDepTxRxParam.txBufLen = rxSize;
                *isoDepTxRxParam.rxLen = 0;            
                err = rfalIsoDepStartTransceive(isoDepTxRxParam);
                
                if(err == RFAL_ERR_NONE)
                {
                   rxReady = txReady = false;
                }
            }
            break;
            
        case CARDEMULATION_CMD_SET_TX_B:
            break;
            
        case CARDEMULATION_CMD_SET_TX_F:
            if (txReady)
            {
                RFAL_MEMCPY(transceiveCtx.txBuf, rxData, rxSize);
                transceiveCtx.txBufLen = rfalConvBytesToBits(rxSize);
                *transceiveCtx.rxRcvdLen = 0;
                err = rfalStartTransceive(&transceiveCtx);
                
                if(err == RFAL_ERR_NONE)
                {
                    // clear flag, after the first transmit (any other transceive is also not the first)
                    isFirstF_Frame = false;
                    // clear rest of flags
                    rxReady = txReady = false;
                }
            }
            break;
        default:
            break;
    }    
    return err;
}

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
ReturnCode ceGetListenState(uint8_t * txData, uint16_t *txSize)
{
    txData[0] = rfalListenGetState(NULL, NULL);
    *txSize = 1;
    return RFAL_ERR_NONE;
}





void ceNfcaCardemu4A(uint8_t mode, rfalIsoDepTxRxParam *isoDepTxRxParam)
{
    switch(mode)
    {
        case CARDEMULATION_MODE_REFLECT: 
            // echo ..
            isoDepTxRxParam->txBuf = isoDepTxRxParam->rxBuf;
            isoDepTxRxParam->txBufLen = *(isoDepTxRxParam->rxLen);
            rfalIsoDepStartTransceive(*isoDepTxRxParam);
            break;
        
        case CARDEMULATION_MODE_NDEF:
            // Stub for internal NDEF processing

        default:
            break;
    }
}


void ceNfcfCardemu3(uint8_t mode, rfalTransceiveContext *ctx)
{
    switch(mode)
    {
        case CARDEMULATION_MODE_REFLECT: 
            // echo ..
            ctx->txBuf = ctx->rxBuf;
            ctx->txBufLen = *ctx->rxRcvdLen;
            rfalStartTransceive(ctx);
            // clear flag, after the first transmit (any other transceive is also not the first)
            isFirstF_Frame = false;
            break;
        
        case CARDEMULATION_MODE_NDEF:
            // Stub for internal NDEF processing

        default:
            break;
    }
}

