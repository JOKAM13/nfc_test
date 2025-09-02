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
 *  \brief Implementation of Common CardEmulation parts
 *
 */
/*!
 *
 */

#ifndef CARDEMULATION_H
#define CARDEMULATION_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <stdint.h>
#include "rfal_utils.h"

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL DATATYPES
******************************************************************************
*/


enum cardEmulationMode
{
    CARDEMULATION_MODE_NDEF                     = 0x01,
    CARDEMULATION_MODE_REFLECT                  = 0x02,
    CARDEMULATION_PROCESS_INTERNAL              = 0x80,
};

enum cardEmulationCommand
{
    CARDEMULATION_CMD_START                     = 0x01, /*!< start listen mode. */
    CARDEMULATION_CMD_STOP                      = 0x02, /*!< stop listen mode. */

    CARDEMULATION_CMD_GET_RX_A                  = 0x11,
    CARDEMULATION_CMD_SET_TX_A                  = 0x12,
    CARDEMULATION_CMD_GET_RX_B                  = 0x13,
    CARDEMULATION_CMD_SET_TX_B                  = 0x14,
    CARDEMULATION_CMD_GET_RX_F                  = 0x15,
    CARDEMULATION_CMD_SET_TX_F                  = 0x16,
    
    CARDEMULATION_CMD_GET_LISTEN_STATE          = 0x21,
};
//
//enum cardEmulationState
//{
//    CARDEMULATION_STATE_OFF                     = 0x00, /*!<  */
//    CARDEMULATION_STATE_IDLE                    = 0x01, /*!<  */
//    CARDEMULATION_STATE_DOWNLOAD                = 0x02, /*!<  */
//    CARDEMULATION_STATE_UPLOAD                  = 0x03, /*!<  */
//};

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/
extern void ceInitalize( void );
extern void ceHandler( void );



extern ReturnCode ceStart(const uint8_t *rxData, const uint16_t rxSize);
extern ReturnCode ceStop( void );

extern ReturnCode ceGetRx(const uint8_t cmd, uint8_t *txData, uint16_t *txSize);
extern ReturnCode ceSetTx(const uint8_t cmd, const uint8_t* rxData, const uint16_t rxSize);

extern ReturnCode ceGetListenState(uint8_t *txData, uint16_t *txSize);

#endif /* CARDEMULATION_H */
