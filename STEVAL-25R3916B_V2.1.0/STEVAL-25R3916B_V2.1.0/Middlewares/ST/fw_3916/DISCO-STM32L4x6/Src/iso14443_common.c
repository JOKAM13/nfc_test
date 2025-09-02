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
 *  \brief Common functions for 14443A and 14443B
 *
 */
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "iso14443_common.h"
#include "rfal_rf.h"
#include "rfal_isoDep.h"

/*
******************************************************************************
* LOCAL DEFINES
******************************************************************************
*/
#define ISO14443_CMD_DESELECT  0xca /*!< command DESELECT */

rfalIsoDepApduTxRxParam iso14443L4TxRxParams; 
rfalIsoDepApduBufFormat iso14443TxBuf;
rfalIsoDepApduBufFormat iso14443RxBuf;
rfalIsoDepBufFormat     iso14443TmpBuf;          /*!< Temp buffer for Rx I-Blocks (internal)   */

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

ReturnCode iso14443TransmitAndReceiveL4(const uint8_t* txbuf,
                                    uint16_t txlen,
                                    uint8_t* rxbuf,
                                    uint16_t rxlen,
                                    uint16_t* actrxlength)
{
    ReturnCode err;

    *actrxlength = 0;

    memcpy(iso14443TxBuf.apdu,txbuf, txlen); 
    iso14443L4TxRxParams.txBuf = &iso14443TxBuf; 
    iso14443L4TxRxParams.txBufLen = txlen; 
    iso14443L4TxRxParams.rxBuf = &iso14443RxBuf; 
    iso14443L4TxRxParams.rxLen = &rxlen; 
    iso14443L4TxRxParams.tmpBuf = &iso14443TmpBuf; 

    err = rfalIsoDepStartApduTransceive( iso14443L4TxRxParams );
    if (err) return err;

    while(RFAL_ERR_BUSY == (err= rfalIsoDepGetApduTransceiveStatus()))
    {
        rfalWorker();
    }
    if (RFAL_ERR_NONE == err)
    {
        *actrxlength = rxlen;
        memcpy(rxbuf,iso14443RxBuf.apdu,*actrxlength); 
    }
    return err;
}
ReturnCode iso14443TransmitAndReceive(const uint8_t* txbuf, uint16_t txlen, uint8_t* rxbuf, uint16_t rxlen, uint16_t* actrxlength, uint32_t fwt_64fcs)
{
    ReturnCode err;

    err = rfalTransceiveBlockingTxRx( (uint8_t*)txbuf, txlen, rxbuf, rxlen, actrxlength, RFAL_TXRX_FLAGS_DEFAULT, rfalConv64fcTo1fc(fwt_64fcs) );

    if ( (RFAL_ERR_TIMEOUT == err) || (RFAL_ERR_NOMSG == err) )
    {
        err = RFAL_ERR_NOTFOUND;
    }

    return err;
}


ReturnCode iso14443Deselect( void )
{
    return rfalIsoDepDeselect();
}
