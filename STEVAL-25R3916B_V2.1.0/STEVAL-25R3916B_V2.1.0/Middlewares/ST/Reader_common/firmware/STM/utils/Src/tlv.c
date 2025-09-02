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

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "utils.h"
#include "tlv.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

/* Get (V)alue as uint8_t */
uint16_t getTlv8(const uint8_t *data, TLV_TAG *tag, uint8_t *value)
{
    uint16_t i = 0;
    /* Get T */
    *tag = data[i++];
    i++;    /* sizeof L for V of type uint32_t is uint8_t, therefore increase by one.. *len = data[1]; */
    /* Get V */    
    if (value != NULL) {
        *value = data[i++];
    }
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)sizeof(uint8_t));
    return i;
}
/* Get (V)alue as uint16_t */
uint16_t getTlv16(const uint8_t *data, TLV_TAG *tag, uint16_t *value)
{
    uint16_t i = 0;
    /* Get T */
    *tag = data[i];
    i++;    /* sizeof L for V of type uint32_t is uint8_t, therefore increase by one.. *len = data[1]; */
    /* Get V */    
    if (value != NULL) {
        *value = GETU16(&data[i]);
    }
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)sizeof(uint16_t));
    return i;
}
/* Get (V)alue as uint32_t */
uint16_t getTlv32(const uint8_t *data, TLV_TAG *tag, uint32_t *value)
{
    uint16_t i = 0;
    /* Get T */
    *tag = data[i++];    
    i++;    /* sizeof L for V of type uint32_t is uint8_t, therefore increase by one.. *len = data[1]; */
    /* Get V */    
    if (value != NULL) {
        *value = GETU32(&data[i]);
    }
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)sizeof(uint32_t));
    return i;
}
/* Get (V)alue as array */
uint16_t getTlvExt(const uint8_t *data, TLV_TAG *tag, uint16_t *len, void *value)
{
    uint16_t i = 0;
    /* Get T */
    *tag = data[i++];
    /* Get L */
    if (data[i] & 0x80) {
        *len = (uint16_t)((data[i++] & 0x7F) << 8);
        *len = (uint16_t)(*len | (uint16_t)data[i++]);
    } else {
        *len = data[i++];
    }
    /* Get V */
    if (value != NULL) {
        memcpy(value, &data[i], *len);
    }
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)*len);
    return i;
}
/* Get (T)ag */
TLV_TAG getTlvTag(const uint8_t *data)
{
    return data[0];
}
/* Get (L)ength */
uint16_t getTlvLen(const uint8_t *data)
{
    uint16_t i = 1;
    uint16_t len = 0;
    if (data[i] & 0x80) {
        len = (uint16_t)(data[i++] << 8);
        len = (uint16_t)(len | (uint16_t)data[i++]);
        len &= 0x7FFF;
    } else {
        len = data[i++];
    }
    return len;
}

/* sizeof(len) */
int getTlvLenSizeof(const uint8_t *data)
{
    return (data[1] & 0x80) ? sizeof(uint16_t) : sizeof(uint8_t);
}

/* Set (V)alue as uint8_t */
uint16_t addTlv8(uint8_t *data, TLV_TAG tag, uint8_t value)
{
    uint16_t i = 0;
    /* Set T */
    data[i++] = tag;
    /* Set L */
    data[i++] = sizeof(uint8_t);
    /* Set V */
    data[i++] = value;
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    return i;
}
/* Set (V)alue as uint16_t */
uint16_t addTlv16(uint8_t *data, TLV_TAG tag, uint16_t value)
{
    uint16_t i = 0;
    /* Set T */
    data[i++] = tag;
    /* Set L */
    data[i++] = sizeof(uint16_t);
    /* Set V */
    WORD2ARRAY(value, &data[i]);
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)sizeof(uint16_t));
    return i;
}
/* Set (V)alue as uint32_t */
uint16_t addTlv32(uint8_t *data, TLV_TAG tag, uint32_t value)
{
    uint16_t i = 0;
    /* Set T */
    data[i++] = tag;
    /* Set L */
    data[i++] = sizeof(uint32_t);
    /* Set V */
    DWORD2ARRAY(value, &data[i]);
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)sizeof(uint32_t));
    return i;
}
/* Set (V)alue as data array */
uint16_t addTlvExt(uint8_t *data, TLV_TAG tag, uint16_t len, void *value)
{
    uint16_t i = 0;
    /* Set T */
    data[i++] = tag;
    /* Set L */
    if (len > 0x7F) {
        data[i++] = (uint8_t)(0x80U | (len >> 8));
        data[i++] = (uint8_t)(len & 0x00FF);
    } else {
        data[i++] = (uint8_t)len;
    }
    /* Set V */
    if(value){
        memcpy(&data[i], value, len);
    }
    /* reply sizeof(T) + sizeof(L) + sizeof(V) */
    i = (uint16_t)(i + (uint16_t)len);
    return i;
}
/* Set (V)alue as list of data array */
uint16_t addTlvExt2(uint8_t *data, TLV_TAG tag, uint8_t cnt, uint16_t *len, void **value)
{
    uint8_t x;
    uint16_t i = 0;
    data[i++] = tag;

    uint16_t l = 0;
    for (x = 0; x < cnt; x++) {
        l = (uint16_t)(l + (uint16_t)len[x]);
    }

    if (l > 0x7F) {
        data[i++] = (uint8_t)(0x80 | (l >> 8));
        data[i++] = (uint8_t)(l & 0x00FF);
    } else {
        data[i++] = (uint8_t)l;
    }
    for (x = 0; x < cnt; x++) {
        if(value[x]){
            memcpy(&data[i], value[x], len[x]);
        }
        i = (uint16_t)(i + (uint16_t)len[x]);
    }

    return i;
}

/**/
bool findTlvExt (const uint8_t *data, uint16_t dataSize, TLV_TAG tag, uint16_t *foundTagOffset)
{
    uint16_t offset = *foundTagOffset;
    *foundTagOffset = 0;
    
    bool found = false;
    int len = 0;
    
    do{
        if(data[offset] == tag){
            found = true;
            break;
        }
        len = getTlvLen(&data[offset]);
        if(len == 0){
            found = false;
            break;            
        }
        len += sizeof(TLV_TAG);
        len += getTlvLenSizeof(&data[offset]);
        offset += len;        
    } while (offset < dataSize);

    if(found){
        *foundTagOffset = offset;
    }
    
    return found;
}

