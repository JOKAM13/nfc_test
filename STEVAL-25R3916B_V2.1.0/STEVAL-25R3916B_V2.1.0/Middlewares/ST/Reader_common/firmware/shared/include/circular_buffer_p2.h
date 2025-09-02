
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

#ifndef RING_BUFFER_P2_H
#define RING_BUFFER_P2_H

#include <stdint.h>
#include "st_errno.h"
#include "utils.h"
 

/** Circular buffer object */
typedef struct {
    uint32_t    size;   /**< maximum number of elements           */
    uint16_t    start;  /**< index of oldest element              */
    uint16_t    end;    /**< index at which to write new element  */
    char        *data;  /**< vector of elements                   */
} CircularBuffer;
 

/*!
 *****************************************************************************
 * \brief Initializes the given ringbuffer to a buffer with at least
 * size elements
 *
 * \param cb : Buffer to be initialized
 * \param size : Buffer size the buffer should be initialized to.
 *
 * \return ERR_NONE ring buffer reset
 * \return ERR_PARAM : the passed parameters are invalid
 * \return ERR_NOMEM : the system is out of heap
 *****************************************************************************
 */
ReturnCode cbInit(CircularBuffer *cb, uint16_t size);

/*!
 *****************************************************************************
 * \brief Resets the specified ring buffer to the state it had after
 * calling cbInit.
 *
 * Must be called if communication should be reset. Should be called either
 * by producer or the consumer. 
 *
 * \param cb : ring buffer to be reset
 *
 *****************************************************************************
 */
static inline void cbReset(CircularBuffer *cb)
{
    // checking parameters
    if( NULL != cb )
    {
        cb->start = 0;
        cb->end = 0;
    }
}

/*!
 *****************************************************************************
 * \brief Returns number of unused bytes in the buffer
 *
 * \param cb : ring buffer to be analyzed
 * \return number of free bytes 
 * 
 *****************************************************************************
 */
static inline uint32_t cbFree(CircularBuffer *cb)
{
    // checking parameters
    if( NULL  != cb )
    {
        //calculating free elements
        const uint32_t mask = cb->size - 1;
        const uint32_t free =  ( cb->start -  cb->end -1) & mask;
        return free;
    }
    return 0;
}

/*!
 *****************************************************************************
 * \brief Returns number of used bytes in the buffer
 *
 * \param cb : ring buffer to be analyzed
 * \return number of used bytes 
 * 
 *****************************************************************************
 */
static inline uint32_t cbUsed(CircularBuffer *cb)
{
    // checking parameters
    if( NULL != cb )
    {
        //callculating usage count
        const uint32_t mask = cb->size - 1;
        const uint32_t used = (cb->end - cb->start) & mask;
        return used;
    }
    return 0;
}

/*!
 *****************************************************************************
 * \brief Add buffer items to ringbuffer
 *
 * \param cb : ring buffer to be written into
 * \param elem : buffer to be written into the ringbuffer cb
 * \param len : the size of the buffer to be written
 * \return number of bytes written
 * 
 *****************************************************************************
 */
static inline uint32_t cbWrite(CircularBuffer *cb, const char *elem, const uint16_t len )
{
    extern uint32_t cbWrite_insecure(CircularBuffer *cb, const char * elem, const uint16_t len );

    // checking parameters
    if( NULL != cb && NULL != elem )
    {
        return cbWrite_insecure(cb, elem, len );
    }
    return 0;
}
 
/*!
 *****************************************************************************
 * \brief Read data from ringbuffer to buffer
 *
 * \param cb   : ring buffer to be read out
 * \param elem : buffer to store ringbuffer data
 * \param len  : the size of the buffer to be written
 * \return number of bytes copied
 * 
 *****************************************************************************
 */
static inline uint32_t cbRead(CircularBuffer *cb, char *elem, const uint16_t len)
{
    extern uint32_t cbRead_insecure(CircularBuffer *cb, char * elem, const uint16_t len );

    // checking parameters
    if( NULL != cb && NULL != elem )
    {
        return cbRead_insecure(cb, elem, len );
    }
    return 0;
}

/*!
 *****************************************************************************
 * \brief Remove data from ringbuffer
 *
 * Note that data is ignored/skipped by moving pointer over the length of data in memory.
 * 
 * \param cb   : ring buffer from which data should be removed
 * \param len  : length of data to remove 
 * \return number of bytes actually removed
 * 
 *****************************************************************************
 */
static inline uint16_t cbDiscardUpTo(CircularBuffer *cb, uint16_t len) 
{
    // checking parameters
    if( NULL  != cb )
    {
        const uint32_t mask = cb->size - 1;
        const uint32_t used =  cbUsed(cb);

        if( used < len )    
        {
            len = used;
        }
        // data is ignored/skipped by moving pointer over the length of data in memory
        cb->start = ((cb->start+len) & mask);
        
        return len;
    }
    
    return 0;
}
#endif
