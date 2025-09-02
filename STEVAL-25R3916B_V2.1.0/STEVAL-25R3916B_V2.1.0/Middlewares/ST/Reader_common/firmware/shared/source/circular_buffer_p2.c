
/******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/myliberty
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
 *      PROJECT:   NFCC
 *      $Revision: $
 *      LANGUAGE:  ISO C99
 */

/*! \file
 *
 *  \author F. Haas
 *
 *  \brief Implementation of a ring buffer for (power of 2) -1 sized buffers.
 *
 * Note: The ring buffer is not thread safe.
 *
 */

/*
******************************************************************************
* Includes
******************************************************************************
*/
#if defined (__CC_ARM)
    #include <stdlib.h>
#elif defined (__GNUC__)
    #include "malloc.h"
#endif

#include "circular_buffer_p2.h"
#include "utils.h"

/*
******************************************************************************
* Global Functions
******************************************************************************
*/

ReturnCode cbInit(CircularBuffer *cb, uint16_t size) {
    // checking parameters
    if( NULL  != cb && 0 != size )
    {
        uint32_t sizeToUse = 1;
        
        //rounding up
        while(sizeToUse < (uint32_t)(size+1) )
        {
            sizeToUse *= 2;
        }

        //initializing data
        cb->size = sizeToUse;
        cb->start = 0;
        cb->end   = 0;
        cb->data = (char*)malloc(cb->size * sizeof(char));
        if( NULL != cb->data )
        {
            return ERR_NONE;
        }
        return ERR_NOMEM;
    }
    return ERR_PARAM;

}

/*!
 *****************************************************************************
 * \brief do not use directly, internal usage only
 *
 * \param cb : do not use
 * \param elem : do not use
 * \param len : do not use
 *
 * \return do not use
 *****************************************************************************
 */


uint32_t cbWrite_insecure(CircularBuffer *cb, const char * elem, const uint16_t len ) 
{
    const uint32_t mask = cb->size - 1;
    const uint32_t free = cbFree(cb);
    //do it all or dont do anything
    if( free >= len )
    {
        char* dest = cb->data;
        const uint32_t end = cb->end;
        int32_t tmpLen = len;
        
        //actually copieing
        while(--tmpLen >= 0)
        {
            const uint32_t idx = (end + tmpLen) & mask;
            
            //as idx is unsigned and bitwise anded with mask it
            //can not reach out of the bound of the cb->data
            //buffer as defined by size.
            dest[idx] = elem[tmpLen];
        }
        cb->end = (end + len) & mask;;
        return len;
    }
    
    return 0; // indicate that no data byte is written
}

/*!
 *****************************************************************************
 * \brief do not use directly, internal usage only
 *
 * \param cb : do not use
 * \param elem : do not use
 * \param len : do not use
 *
 * \return do not use
 *****************************************************************************
 */
 
uint32_t cbRead_insecure(CircularBuffer *cb, char * elem, const uint16_t len) 
{
    const uint32_t mask = cb->size - 1;
    const uint32_t used =  cbUsed(cb);
    int32_t tmpLen = len;
    //do it all or dont do anything
    if( used >= len )
    {
        char* src = cb->data;
        uint32_t start = cb->start;
        //actually copieing
        while(--tmpLen >= 0)
        {
            const uint32_t idx = (start + tmpLen) & mask;
            
            //see comment at cbWrite
            elem[tmpLen] = src[idx];
        }
        cb->start = (start + len) & mask;;
        return len;
    }
    
    return 0; // indicate that no data byte is read
}

