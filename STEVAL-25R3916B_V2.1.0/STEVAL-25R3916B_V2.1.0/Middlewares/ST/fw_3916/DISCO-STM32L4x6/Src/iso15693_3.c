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
/*
 *      PROJECT:   ST25R3911 firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author Ulrich Herrmann
 *
 *  \brief Implementation of ISO-15693-3
 *
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "rfal_iso15693_2.h"
#include "iso15693_3.h"
#include "utils.h"
#include "logger.h"
#include "rfal_rf.h"

/*
******************************************************************************
* LOCAL MACROS
******************************************************************************
*/

/*
******************************************************************************
* LOCAL DEFINES
******************************************************************************
*/
#define ISO15693_NUM_UID_BITS 64 /*!< number of UID bits */

#define ISO15693_BUFFER_SIZE (64 + 8) /*!< length of iso15693 general purpose buffer */

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static uint8_t iso15693Buffer[ISO15693_BUFFER_SIZE+2]; /*!< general
                                        purpose buffer. 2 extra bytes are needed for crc*/
static uint8_t iso15693DefaultSendFlags; /*!< default flags used for iso15693SendRequest */

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
static ReturnCode iso15693SendRequest(uint8_t cmd,
                uint8_t flags,
                const iso15693ProximityCard_t* card,
                uint8_t* rcvbuffer,
                uint16_t rcvbuffer_len,
                uint16_t* actlength,
                uint8_t* addSendData,
                uint8_t addSendDataLength,
                uint32_t no_response_time_64fcs);
/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
ReturnCode iso15693Initialize( bool useSlowTX, bool useFastRX )
{
    ReturnCode err;

    err = rfalSetMode( RFAL_MODE_POLL_NFCV, useSlowTX?(RFAL_BR_1p66):(RFAL_BR_26p48), useFastRX?(RFAL_BR_52p97):(RFAL_BR_26p48) );
    rfalSetErrorHandling( RFAL_ERRORHANDLING_EMD );
    

    rfalSetGT( RFAL_GT_NFCV );
    rfalSetFDTListen( rfalConv64fcTo1fc(ISO15693_MASK_FDT_LISTEN) );
    rfalSetFDTPoll( RFAL_FDT_POLL_NFCV_POLLER );
  
    rfalFieldOnAndStartGT();

    return err;
}

ReturnCode iso15693Deinitialize(uint8_t keep_on)
{
    if (!keep_on)
    {
        return rfalFieldOff();
    }
    return RFAL_ERR_NONE;
}

ReturnCode iso15693SendStayQuiet(const iso15693ProximityCard_t* card)
{
    uint16_t actlength;
    uint8_t data;


    /* just send the command - no reply sent by the PICC */
    return iso15693SendRequest(ISO15693_CMD_STAY_QUIET,
            iso15693DefaultSendFlags,
            card,
            &data,
            1,
            &actlength,
            NULL,
            0,
            ISO15693_FWT);
}

ReturnCode iso15693SelectPicc(const iso15693ProximityCard_t* card)
{
    uint16_t actlength;
    ReturnCode err;

    err = iso15693SendRequest(ISO15693_CMD_SELECT,
            iso15693DefaultSendFlags,
            card,
            iso15693Buffer,
            4,
            &actlength,
            NULL,
            0,
            ISO15693_FWT);
    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    if (iso15693Buffer[0] != 0)
    {
        err = RFAL_ERR_NOMSG;
    }

out:
    return err;
}

ReturnCode iso15693GetPiccSystemInformation(const iso15693ProximityCard_t* card,
                                iso15693PiccSystemInformation_t* sysInfo, uint16_t *sysInfoLen )
{
    ReturnCode err;
    uint8_t offset = 0;

    if( sysInfoLen == NULL )
    {
        return RFAL_ERR_PARAM;
    }
    
    err = iso15693SendRequest(ISO15693_CMD_GET_SYSTEM_INFORMATION,
            iso15693DefaultSendFlags,
            card,
            iso15693Buffer,
            17,
            sysInfoLen,
            NULL,
            0,
            ISO15693_FWT);
    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    sysInfo->dsfid = 0;
    sysInfo->afi = 0;
    sysInfo->memNumBlocks = 0;
    sysInfo->memBlockSize = 0;
    sysInfo->icReference = 0;
    /* process data */
    if (*sysInfoLen > 4)
    {
        /* copy first 10 bytes which are fixed */
        RFAL_MEMCPY((uint8_t*)sysInfo, iso15693Buffer, 10);
        /* evaluate infoFlags field */
        if (sysInfo->infoFlags & 0x1)
        {
            /* dsfid field present */
            sysInfo->dsfid = iso15693Buffer[10];
            offset++;
        }
        if (sysInfo->infoFlags & 0x2)
        {
            /* afi field present */
            sysInfo->afi = iso15693Buffer[10+offset];
            offset++;
        }
        if (sysInfo->infoFlags & 0x4)
        {
            /* memory size field present */
            sysInfo->memNumBlocks = iso15693Buffer[10+offset];
            sysInfo->memBlockSize = iso15693Buffer[11+offset];
            offset += 2;
        }
        if (sysInfo->infoFlags & 0x8)
        {
            /* ic reference field present */
            sysInfo->icReference = iso15693Buffer[10+offset];
        }
    }
    else
    {
        /* error field set */
        err = RFAL_ERR_NOTSUPP;
    }

out:
    return err;
}

ReturnCode iso15693ReadSingleBlock(const iso15693ProximityCard_t* card,
                    iso15693PiccMemoryBlock_t* memBlock)
{
    ReturnCode err;
    uint16_t actlength;

    err = iso15693SendRequest(ISO15693_CMD_READ_SINGLE_BLOCK,
            iso15693DefaultSendFlags,
            card,
            iso15693Buffer,
            ISO15693_BUFFER_SIZE,
            &actlength,
            &memBlock->blocknr,
            1,
            ISO15693_FWT);
    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    if (actlength >= 2)
    {
        memBlock->flags = iso15693Buffer[0];
        if (memBlock->flags & ISO15693_RESP_FLAG_ERROR)
        {
            memBlock->actualSize = actlength - 2;
            memBlock->errorCode = iso15693Buffer[1];
        }
        else
        {
            memBlock->actualSize = actlength - 1;
            memBlock->errorCode = 0;
            RFAL_MEMCPY(memBlock->data, &iso15693Buffer[1], memBlock->actualSize);
        }
    }

out:
    return err;
}



ReturnCode iso15693ReadMultipleBlocks(const iso15693ProximityCard_t* card, uint8_t startblock, uint8_t numBlocks,
                                        uint8_t* res_flags, uint8_t* data, uint16_t dataLen, uint16_t *actLen )
{
    ReturnCode err;
    uint16_t actlength;
    uint8_t addData[2];

    addData[0] = startblock;
    addData[1] = numBlocks - 1;

    err = iso15693SendRequest(ISO15693_CMD_READ_MULTIPLE_BLOCKS,
            iso15693DefaultSendFlags,
            card,
            iso15693Buffer,
            ISO15693_BUFFER_SIZE,
            &actlength,
            addData,
            2,
            ISO15693_FWT);
    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    if (actlength >= 2)
    {
        *res_flags = iso15693Buffer[0];
        if (*res_flags & ISO15693_RESP_FLAG_ERROR)
        {
            *actLen = 0;
            err = iso15693Buffer[1];
        }
        else
        {
            *actLen = ((( actlength > dataLen ) ? dataLen : actlength) - 1);
            RFAL_MEMCPY( data, &iso15693Buffer[1], *actLen );
        }
    }

out:
    return err;

}

ReturnCode iso15693WriteSingleBlock(const iso15693ProximityCard_t* card,
                                uint8_t flags,
                                iso15693PiccMemoryBlock_t* memBlock)
{
    ReturnCode err;
    uint16_t actlength;
    uint8_t* buf;

    memBlock->errorCode = 0;

    /* just send the request and wait separately for the answer */
    err = iso15693SendRequest(ISO15693_CMD_WRITE_SINGLE_BLOCK,
            flags,
            NULL,
            iso15693Buffer,
            2+2,
            &actlength,
            (uint8_t*)&memBlock->blocknr,
            memBlock->actualSize+1,
            (flags & ISO15693_REQ_FLAG_OPTION)?ISO15693_FWT:4238); /* ~ 20ms */

    if (flags & ISO15693_REQ_FLAG_OPTION)
    {
        /* Ignore any potential errors, tag should not be answering */

        /* according to the standard wait 20ms before sending EOF */
        platformDelay(20);

        buf = iso15693Buffer; /* valid buffer with 0 length: EOF */
        
        err = rfalTransceiveBlockingTxRx( buf, 0, iso15693Buffer, (2+2), &actlength, 
                (RFAL_TXRX_FLAGS_CRC_TX_MANUAL | RFAL_TXRX_FLAGS_CRC_RX_REMV | RFAL_TXRX_FLAGS_NFCIP1_OFF | RFAL_TXRX_FLAGS_AGC_ON | RFAL_TXRX_FLAGS_PAR_RX_REMV), 
                rfalConv64fcTo1fc(ISO15693_FWT) );

        EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);
    }
    else
    {
        /* Without option flag the tag should have answered after latest 20ms */
        EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);
    }
    
    if (actlength > 3)
    {
        err = RFAL_ERR_NOTSUPP;
    }
    if (actlength < 1)
    {
        err = RFAL_ERR_NOTSUPP;
    }
    memBlock->flags = iso15693Buffer[0];

    if (actlength == 2 && (ISO15693_RESP_FLAG_ERROR & iso15693Buffer[0]))
        memBlock->errorCode = iso15693Buffer[1];

out:
    return err;
}

ReturnCode iso15693TxRxNBytes(
                uint8_t* txBuf,
                uint16_t txLen,
                uint8_t* rxBuf,
                uint16_t rxLen,
                uint16_t* actlength,
                uint16_t response_wait_ms)
{
    ReturnCode err;
    uint8_t  flags = *txBuf;
    uint8_t* buf;
    uint16_t bufLen;
    uint32_t fwt;

    if (flags & ISO15693_REQ_FLAG_OPTION)
    {
        buf = NULL;
        bufLen = 0;
        fwt = rfalConv64fcTo1fc(ISO15693_FWT);
    }
    else
    {
        buf = rxBuf;
        bufLen = rxLen;
        fwt = rfalConvMsTo1fc(response_wait_ms);
    }
    
    err = rfalTransceiveBlockingTxRx( txBuf, txLen, buf, bufLen, actlength, 
                                     (RFAL_TXRX_FLAGS_CRC_TX_AUTO | RFAL_TXRX_FLAGS_CRC_RX_KEEP | RFAL_TXRX_FLAGS_NFCIP1_OFF | RFAL_TXRX_FLAGS_AGC_ON | RFAL_TXRX_FLAGS_PAR_RX_REMV), 
                                      fwt );    

    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    if (rxLen > 0 && response_wait_ms && (flags & ISO15693_REQ_FLAG_OPTION))
    {
        /* according to the standard wait 20ms before sending EOF */
        platformDelay(response_wait_ms);
        err = rfalISO15693TransceiveEOFAnticollision( rxBuf, bufLen, actlength );
    }

out:
    return err;
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
static ReturnCode iso15693SendRequest(uint8_t cmd,
                uint8_t flags,
                const iso15693ProximityCard_t* card,
                uint8_t* rcvbuffer,
                uint16_t rcvbuffer_len,
                uint16_t* actlength,
                uint8_t* addSendData,
                uint8_t addSendDataLength,
                uint32_t no_response_time_64fcs)
{
    uint8_t length;
 

    /* sanity checks first */
    if ((ISO15693_UID_LENGTH + addSendDataLength + 2) > ISO15693_BUFFER_SIZE)
    {
        return RFAL_ERR_NOMEM;
    }

    /* FLAG_SELECT and FLAG_ADDRESS will be added as required below */
    flags &= (~(ISO15693_REQ_FLAG_SELECT|ISO15693_REQ_FLAG_ADDRESS));
    if (card == NULL)
    {
        /* uid is NULL which means that selected PICC (using #iso15693SelectPicc)
           is used */
        /* set select flag */
        iso15693Buffer[0] = flags | ISO15693_REQ_FLAG_SELECT;
        length = 2;
    }
    else
    {
        /* set address flag */
        iso15693Buffer[0] = flags | ISO15693_REQ_FLAG_ADDRESS;
        /* copy UID */
        RFAL_MEMCPY((void*)&iso15693Buffer[2], (void*)card->uid, ISO15693_UID_LENGTH);
        length = 2 + ISO15693_UID_LENGTH;
    }

    iso15693Buffer[1] = cmd;

    /* append additional data to be sent */
    RFAL_MEMCPY(&iso15693Buffer[length], addSendData, addSendDataLength);
    length += addSendDataLength;

    return rfalTransceiveBlockingTxRx( iso15693Buffer, length, rcvbuffer, rcvbuffer_len, actlength, 
                                     (RFAL_TXRX_FLAGS_CRC_TX_AUTO | RFAL_TXRX_FLAGS_CRC_RX_REMV | RFAL_TXRX_FLAGS_NFCIP1_OFF | RFAL_TXRX_FLAGS_AGC_ON | RFAL_TXRX_FLAGS_PAR_RX_REMV), 
                                      rfalConv64fcTo1fc( no_response_time_64fcs * 4 ) );

}


ReturnCode iso15693FastReadSingleBlock(const iso15693ProximityCard_t* card,
                    iso15693PiccMemoryBlock_t* memBlock)
{
    ReturnCode err;
    uint16_t actlength;
    uint8_t txBuf[4];

    txBuf[0] = 0x02;  /* Req fags */
    txBuf[1] = ISO15693_CMD_FAST_READ_SINGLE_BLOCK;  /* CMD      */
    txBuf[2] = ISO15693_M24LR_IC_MFG_CODE;  /* Mfg code */
    txBuf[3] = memBlock->blocknr;  /* block number */


    err = iso15693TxRxNBytes( txBuf, sizeof(txBuf), iso15693Buffer, ISO15693_BUFFER_SIZE, &actlength, 50 );
    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    if (actlength >= 2)
    {
        memBlock->flags = iso15693Buffer[0];
        if (memBlock->flags & ISO15693_RESP_FLAG_ERROR)
        {
            memBlock->actualSize = actlength - 2;
            memBlock->errorCode = iso15693Buffer[1];
        }
        else
        {
            memBlock->actualSize = actlength - 1;
            memBlock->errorCode = 0;
            RFAL_MEMCPY(memBlock->data, &iso15693Buffer[1], memBlock->actualSize);
        }
    }

out:
    return err;
}


ReturnCode iso15693FastReadMultipleBlocks(const iso15693ProximityCard_t* card, uint8_t startblock, uint8_t numBlocks,
                                        uint8_t* res_flags, uint8_t* data, uint16_t dataLen, uint16_t *actLen )
{
    ReturnCode err;
    uint16_t actlength;
    uint8_t txBuf[5];

    /* Using the non Addressed mode */
    txBuf[0] = 0x02;  /* Req fags */
    txBuf[1] = ISO15693_CMD_FAST_READ_MULTI_BLOCK;  /* CMD              */
    txBuf[2] = ISO15693_M24LR_IC_MFG_CODE;          /* Mfg code         */
    txBuf[3] = startblock;                          /* block number     */
    txBuf[4] = (numBlocks - 1);                     /* number of blocks */

    err = iso15693TxRxNBytes( txBuf, sizeof(txBuf), iso15693Buffer, ISO15693_BUFFER_SIZE, &actlength, 50 );
    EVAL_ERR_NE_GOTO(RFAL_ERR_NONE, err, out);

    if (actlength >= 2)
    {
        *res_flags = iso15693Buffer[0];
        if (*res_flags & ISO15693_RESP_FLAG_ERROR)
        {
            *actLen = 0;
            err = iso15693Buffer[1];
        }
        else
        {
            *actLen = ((( actlength > dataLen ) ? dataLen : actlength) - 1);
            RFAL_MEMCPY( data, &iso15693Buffer[1], *actLen );
        }
    }

out:
    return err;
}
