/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2023 STMicroelectronics, all rights reserved
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
 *  \brief TLV Encode and Decode Utilities
 *
 */
/*!
 * 
 */

#ifndef TLV_H_
#define TLV_H_

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>
#include "st_errno.h"

/*
******************************************************************************
* TYPEDEF
******************************************************************************
*/
typedef uint8_t                                 TLV_TAG;

/*
******************************************************************************
* GLOBAL FUNCTION DECLARATION
******************************************************************************
*/

/*! 
 *****************************************************************************
 *  \brief  TLV encode and append a uint8_t value to a data array.
 *
 *  \param[in] data: Existing data array
 *  \param[in] tag: tag represending the value content
 *  \param[in] value: value to be encoded
 *
 *  \return sizeof new added TLV packet as 16 bit value.
 *
 *****************************************************************************
 */
uint16_t addTlv8    (uint8_t *data, TLV_TAG tag, uint8_t value);
uint16_t addTlv16   (uint8_t *data, TLV_TAG tag, uint16_t value);
uint16_t addTlv32   (uint8_t *data, TLV_TAG tag, uint32_t value);
uint16_t addTlvExt  (uint8_t *data, TLV_TAG tag, uint16_t len, void *value);
uint16_t addTlvExt2 (uint8_t *data, TLV_TAG tag, uint8_t cnt, uint16_t *len, void **value);


uint16_t getTlv8    (const uint8_t *data, TLV_TAG *tag, uint8_t *value);
uint16_t getTlv16   (const uint8_t *data, TLV_TAG *tag, uint16_t *value);
uint16_t getTlv32   (const uint8_t *data, TLV_TAG *tag, uint32_t *value);
uint16_t getTlvExt  (const uint8_t *data, TLV_TAG *tag, uint16_t *len, void *value);

TLV_TAG getTlvTag   (const uint8_t *data);
uint16_t getTlvLen  (const uint8_t *data);
int getTlvLenSizeof (const uint8_t *data);

bool findTlvExt (const uint8_t *data, uint16_t dataSize, TLV_TAG tag, uint16_t *foundTagOffset);

#endif /* TLV_H_ */

