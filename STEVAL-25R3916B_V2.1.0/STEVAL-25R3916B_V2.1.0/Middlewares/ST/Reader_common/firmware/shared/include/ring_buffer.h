
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
 *  \author M. Arpa
 *
 *  \brief Interface for a c ring buffer.
 * 
 * Note: The ring buffer not thread safe.
 *
 * We assume that there is one producer (calls rbPut, rbNumAvailableToPut) and
 * one consumer (calls rbNumAvailableToGet, rbDiscard, mqGet) 
 * for each ring buffer.
 * The producer uses strictly sequential calls. I.e. it does not interrupt
 * one call of himself by another call.
 * The consumer uses strictly sequential calls. I.e. it does not interrupt
 * one call of himself by another call.
 * The producer may interrupt any call of the consumer.
 * The consumer may interrupt any call of the producer. 
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/*
******************************************************************************
* Includes
******************************************************************************
*/

#include <stdint.h>
#include "st_errno.h"
#ifdef __cplusplus
extern "C" {
#endif

    
/*
******************************************************************************
* Defines
******************************************************************************
*/
    
/*! Define the maximum number of ring buffers to be created */
#define RB_MAX_NUMBER_OF_RING_BUFFERS  4

/*
******************************************************************************
* Functions
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Initialisation of the ring buffer. 
 *
 * Must be called before calling any other ring buffer function. Must only
 * be called once. Initialises all internal structures and variables.
 *****************************************************************************
 */
void rbInitialise( );

/*!
 *****************************************************************************
 * \brief Create one ring buffer.
 *
 * Must be called to create a ring buffer. Must either be called by the
 * producer or the consumer. 
 *
 * \param id : identifier to be used to access this ring buffer later on.  
 * \param maxSize : maximum of bytes to be stored in this ring buffer
 *
 * \return ERR_NONE buffer created
 * \return ERR_PARAM size was zero
 * \return ERR_NOMEM there is not enough memory to create the ring buffer
 *****************************************************************************
 */
ReturnCode rbCreate( uint8_t id, uint32_t maxSize );

/*!
 *****************************************************************************
 * \brief Resets the specified ring buffer to the state it had after
 * calling rbCreate.
 *
 * Must be called if communication should be reset. Should be called either
 * by producer or the consumer. 
 *
 * \param id : identifier for ring buffer to be reset
 *
 * \return ERR_NONE ring buffer reset
 * \return ERR_PARAM there exists no such ring buffer (either id is wrong, or the
 *                   ring buffer was never created
 *****************************************************************************
 */
ReturnCode rbReset( uint8_t id );

/* Functions to be called by the producer --------------------------------- */

/*!
 *****************************************************************************
 * \brief Returns the number of free bytes in the ring buffer
 *
 * Must be called by the producer only. 
 *
 * \param id : identifier for ring buffer to enquire
 *
 * \return >= 0 : number of bytes that can be put into the ring buffer
 *****************************************************************************
 */
uint32_t rbNumAvailableToPut( uint8_t id );

/*!
 *****************************************************************************
 * \brief Puts the given bytes in the ring buffer
 *
 * Must be called by the producer to transmit n bytes to the ring buffer.
 * This bytes can than be collected by the consumer.
 *
 * \param id : identifier for ring buffer to put
 * \param ptr : pointer to the data to be put (a copy of this data is
 * put in the ring buffer) - must not be 0
 * \param size : the data size to be put (must be > 0 and < maxSize)
 *
 * \return ERR_NONE copy of data inserted into the ring buffer
 * \return ERR_PARAM there exists no ring buffer with this id, or ptr is 0, or the
 *                   size is too small or too big
 * \return ERR_NOMEM there is not enough space in the ring buffer
 *****************************************************************************
 */
ReturnCode rbPut( uint8_t id, const void * ptr, uint32_t size );


/* Functions to be called by the consumer --------------------------------- */

/*!
 *****************************************************************************
 * \brief Returns the number of byte in the ring buffer.
 *
 * Must be called by the consumer only. 
 *
 * \param id : identifier for ring buffer to enquire
 *
 * \return >= 0 : number of bytes that can be get.
 *****************************************************************************
 */
uint32_t rbNumAvailableToGet( uint8_t id );

/*!
 *****************************************************************************
 * \brief Removes n bytes
 *
 * Must be called by the consumer to discard the n bytes in the ring buffer
 *
 * \param id : identifier for ring buffer to receive
 * \param *size : pointer containing the number of bytes the caller wants
 * to discard. The function will set the value to the number of bytes
 * really discared (may be less).
 *
 * \return ERR_NONE copy of data inserted into the buffer, size is 
 * set approriately, and data is removed from the ring buffer
 * \return ERR_PARAM there exists no ring buffer with this id, or ptr is 0, or the
 * size is 0
 *****************************************************************************
 */
ReturnCode rbDiscard( uint8_t id, uint32_t * size );

/*!
 *****************************************************************************
 * \brief Get n bytes
 *
 * Must be called by the consumer to get the next n bytes in the ring buffer.
 * I.e. the data is copied from the ring buffer to the given buffer and the
 * data is removed from the ring buffer.
 *
 * \param id : identifier for ring buffer to receive
 * \param ptr : pointer to the buffer where the data shall be copied
 * - must not be 0
 * \param *size : pointer containing the buffer size, the function will insert
 * the copied data size in this 
 *
 * \return ERR_NONE copy of data inserted into the buffer, size is 
 * set approriately, and data is removed from the ring buffer
 * \return ERR_PARAM there exists no ring buffer with this id, or ptr is 0, or the
 * size is 0
 * \return ERR_NOMSG there is no data ready for receive
 *****************************************************************************
 */
ReturnCode rbGet( uint8_t id, void * ptr, uint32_t * size ); 


#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H */

