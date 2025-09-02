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
 *  \brief T2T read and write command implementation
 *
 */
/*!
 * 
 */

#ifndef _T2T_H
#define _T2T_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "rfal_utils.h"

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/
/*! 
 *****************************************************************************
 *  \brief  Read out a given number of bytes from a T2T PICC.
 *
 *  This function reads out a given number of bytes from a T2T
 *  compatible PICC in field.
 *  \note Due to the nature of the T2T read-command normally 16bytes are
 *  read out. This function however only returns the number of bytes given.
 *  \note T2T only has 16 pages. If a roll over occurs function
 *  continues at address 0x0 as T2T specifies.
 *  \note PICC must be in ACTIVE state using #iso14443ASelect
 *
 *  \param[in] startAddr: Address of the first page to read out.
 *  \param[out] readbuf: Buffer with size \a length where the result is stored.
 *  \param[in] length: Number of bytes to read out (size of \a readbuf)
 *  \param[in] actLength: Number of bytes actually read.
 *
 *  \return ERR_NOTFOUND : No answer from PICC.
 *  \return ERR_NOMSG : NAK received.
 *  \return ERR_PARAM : \a startAddr behind 0xf.
 *  \return ERR_IO : Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
extern ReturnCode t2tReadNBytes(uint8_t startAddr, uint8_t* readbuf, uint8_t length, uint8_t* actLength);

/*! 
 *****************************************************************************
 *  \brief  Write a page of a T2T PICC.
 *
 *  This function writes one page data to a T2T compatible PICC in field.
 *  \note PICC must be in ACTIVE state using #iso14443ASelect
 *
 *  \param[in] pageAddr: Address of page write.
 *  \param[in] writebuf: one page (4 bytes!) longe buffer to write to the
 *                      given page.
 *
 *  \return ERR_NOTFOUND : No answer from PICC.
 *  \return ERR_NOMSG : NAK received.
 *  \return ERR_PARAM : \a startAddr behind 0xf.
 *  \return ERR_WRITE : Write failed.
 *  \return ERR_IO : Error during communication.
 *  \return ERR_NONE : No error.
 *
 *****************************************************************************
 */
extern ReturnCode t2tWritePage(uint8_t pageAddr, const uint8_t* writebuf);

#endif /* _T2T_H */

