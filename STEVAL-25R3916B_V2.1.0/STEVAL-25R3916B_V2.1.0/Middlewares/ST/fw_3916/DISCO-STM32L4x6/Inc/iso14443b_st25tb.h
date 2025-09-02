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

#ifndef ISO_14443_B_ST25TB_H
#define ISO_14443_B_ST25TB_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "rfal_utils.h"
#include <stdbool.h>

/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/
#define ISO14443B_ST25TB_UIDSIZE 8
/*
******************************************************************************
* GLOBAL DATATYPES
******************************************************************************
*/

/*!  
 * struct representing the content of ATQB
 * See ISO14443b spec. for more information.
 */
typedef struct
{
    uint8_t Chip_ID; /*<!< session ID of the card */
    uint8_t uid[ISO14443B_ST25TB_UIDSIZE]; /*!< UID of the ST25TB series card (or ST SR(I|T)(512/2K/4K)) */
    bool    collision; /*!< true, if there was a collision which has been resolved,
                        otherwise no collision occured */
}iso14443B_ST25TB_t;

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! 
 *****************************************************************************
 *  \brief  Send Initiate command to all tags in the field.
 *
 *  Put the tags into 'Inventory' state and try to retrieve one Chip_ID.
 *
 *  \param[out] card : If there was a valid response it will stored as Chip_ID.
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Initiate( iso14443B_ST25TB_t* card);


/*! 
 *****************************************************************************
 *  \brief  Send Pcall16 command to all tags in the field.
 *
 *  Start the polling loop with up to 16 slots. The first slots gets 
 *  immediately executed.
 *
 *  \param[out] card : If there was a valid response it will stored as Chip_ID.
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Pcall16(iso14443B_ST25TB_t *card);


/*! 
 *****************************************************************************
 *  \brief  Send slot marker command to all tags in the field.
 *
 *  Execute one slot after Pcall16 command.
 *
 *  \param[in] slot_num : current slot to be executed: values 1-15.
 *  \param[out] card : If there was a valid response it will stored as Chip_ID.
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Slot_marker(uint8_t slot_num, iso14443B_ST25TB_t *card);

/*! 
 *****************************************************************************
 *  \brief  Send Select command to tag with specified Chip_ID
 *
 *  \param[in] card : The card with Chip_ID will be addressed
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Select(const iso14443B_ST25TB_t *card);

/*! 
 *****************************************************************************
 *  \brief  Send Get_UID command to selected tag
 *
 *  \param[out] card : If there was a valid response it will stored as UID.
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Get_UID(iso14443B_ST25TB_t *card);

/*! 
 *****************************************************************************
 *  \brief  Send Completion command to selected tag
 *
 *  Tag(s) won't respond to this command.
 *
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Completion(void);

/*! 
 *****************************************************************************
 *  \brief  Send Reset_to_Inventory command to selected tag
 *
 *  Tag(s) won't respond to this command.
 *
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Reset_to_inventory(void);


/*! 
 *****************************************************************************
 *  \brief  Send Read_block command to selected tag
 *
 *  \param[in] address: of the block to be read
 *  \param[out] rdata : 4 bytes being read
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Read_block(uint8_t address, uint8_t rdata[4]);

/*! 
 *****************************************************************************
 *  \brief  Send Write_block command to selected tag and verify
 *
 *  As the write command does not have any response. This function will after the 
 *  write command a read command to verify the memory content.
 *
 *  \param[in] address: of the block to be read
 *  \param[out] wdata : 4 bytes to be written
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error while reading back the block
 *  \return ERR_FRAMING : Error while reading back the block
 *  \return ERR_WRITE : Read back data does not match the written data
 *  \return ERR_NONE : No error, read back data matches the written data
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_Write_block(uint8_t address, const uint8_t wdata[4]);

/*! 
 *****************************************************************************
 *  \brief  Singulate one card and get its UID
 *
 *  Complete Sequenc of Initiate() followed optionally by PCall16 and Slot 
 *  markers until one card could be read. This card will then be selected and 
 *  its UID read. The card will remain in Selected state.
 *
 *  \param[out] card : Chip_ID and UID of the singulated card.
 *
 *  \return ERR_TIMEOUT : no recognisable reply from cards.
 *  \return ERR_FRAMING : Error during communication.
 *  \return ERR_CRC:      Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
ReturnCode iso14443B_ST25TB_SingulateAndGetUID(iso14443B_ST25TB_t *card);

#endif /* ISO_14443_B_ST25TB_H */
