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
 *  \brief Implementation of ISO-14443B
 *
 */
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "iso14443b_st25tb.h"
#include "logger.h"
#include "string.h"
#include "rfal_rf.h"
#include "rfal_st25tb.h"

/*
******************************************************************************
* LOCAL DEFINES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL MACROS
******************************************************************************
*/


/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
ReturnCode iso14443B_ST25TB_Initiate(iso14443B_ST25TB_t *card)
{
    return rfalSt25tbPollerInitiate( &card->Chip_ID );
}

ReturnCode iso14443B_ST25TB_Pcall16(iso14443B_ST25TB_t *card)
{
    return rfalSt25tbPollerPcall( &card->Chip_ID );
}

ReturnCode iso14443B_ST25TB_Slot_marker(uint8_t slot_num, iso14443B_ST25TB_t *card)
{
    return rfalSt25tbPollerSlotMarker( slot_num, &card->Chip_ID);
}

ReturnCode iso14443B_ST25TB_Select(const iso14443B_ST25TB_t *card)
{
    return rfalSt25tbPollerSelect( card->Chip_ID );
}

ReturnCode iso14443B_ST25TB_Get_UID(iso14443B_ST25TB_t *card)
{
    return rfalSt25tbPollerGetUID( (rfalSt25tbUID*) card->uid );
}

ReturnCode iso14443B_ST25TB_Completion()
{
    return rfalSt25tbPollerCompletion();
}

ReturnCode iso14443B_ST25TB_Reset_to_inventory()
{
    return rfalSt25tbPollerResetToInventory();
}

ReturnCode iso14443B_ST25TB_Read_block(uint8_t address, uint8_t rdata[4])
{
    return rfalSt25tbPollerReadBlock( address, (rfalSt25tbBlock*) rdata);
}

ReturnCode iso14443B_ST25TB_Write_block(uint8_t address, const uint8_t wdata[4])
{
    return rfalSt25tbPollerWriteBlock( address, (const rfalSt25tbBlock*) wdata);
}

ReturnCode iso14443B_ST25TB_SingulateAndGetUID(iso14443B_ST25TB_t *card)
{
    ReturnCode err;
    rfalSt25tbListenDevice st25tbDev;
    uint8_t devCnt = 0;
    
    err = rfalSt25tbPollerCollisionResolution( 1, &st25tbDev, &devCnt );
    if(devCnt > 0)
    {
        card->Chip_ID = st25tbDev.chipID;
        RFAL_MEMCPY( card->uid, st25tbDev.UID, RFAL_ST25TB_UID_LEN );
        return RFAL_ERR_NONE;
    }
    else if (RFAL_ERR_NONE == err)
    {
        return RFAL_ERR_TIMEOUT;
    }
    
    return err;
}
