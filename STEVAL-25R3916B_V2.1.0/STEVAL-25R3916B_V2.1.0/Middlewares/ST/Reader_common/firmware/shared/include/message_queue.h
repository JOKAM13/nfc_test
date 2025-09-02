
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
 *  \brief Interface for a c message queue. 
 * 
 * Note: The message queue is not thread safe.
 *
 * We assume that there is one producer (calls mqSend, mqNumAvailableToSend) and
 * one consumer (calls mqNumAvailableToReceive, mqReceive, mqPeek, mqPeekSize, 
 * mqDiscard) for each queue.
 * The producer uses strictly sequential calls. I.e. it does not interrupt
 * one call of himself by another call.
 * The consumer uses strictly sequential calls. I.e. it does not interrupt
 * one call of himself by another call.
 * The producer may interrupt any call of the consumer.
 * The consumer may interrupt any call of the producer. 
 */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

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
    
/*! Define the maximum number of message queues to be created */
#define MQ_MAX_NUMBER_OF_QUEUES  11

/*
******************************************************************************
* Functions
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Initialisation of the message queue. 
 *
 * Must be called before calling any other message queue function. Must only
 * be called once. Initialises all internal structures and variables.
 *****************************************************************************
 */
void mqInitialise( void );

/*!
 *****************************************************************************
 * \brief Create one message queue.
 *
 * Must be called to create a message queue. Must either be called by the
 * producer or the consumer. 
 *
 * \param id : identifier to be used to access this message queue later on.  
 * \param maxMsg : the maximum number of messages to be provided by this queue
 * \param maxMsgSize : each message can have a maximum size of this 
 *
 * \return ERR_NONE queue created
 * \return ERR_PARAM there exists already a queue with the given id or
 *                   if either the id is too big, or one of the sizes is <= 0
 * \return ERR_NOMEM there is not enough memory to create the queue
 *****************************************************************************
 */
stError mqCreate( uint8_t id, uint32_t maxMsg, uint32_t maxMsgSize );

/*!
 *****************************************************************************
 * \brief Resets the specified message queue to the state it had after
 * calling mqCreate.
 *
 * Must be called if communication should be reset. Should be called either
 * by producer or the consumer. 
 *
 * \param id : identifier for queue to be reset
 *
 * \return ERR_NONE queue reset
 * \return ERR_PARAM there exists no such queue (either id is wrong, or the
 *                   queue was never created
 *****************************************************************************
 */
stError mqReset( uint8_t id );

/* Functions to be called by the producer --------------------------------- */

/*!
 *****************************************************************************
 * \brief Returns the number of free messages. 
 *
 * Must be called by the producer only. 
 *
 * \param id : identifier for queue to enquire
 *
 * \return >= 0 : number of messages that can be sent
 *****************************************************************************
 */
uint32_t mqNumAvailableToSend( uint8_t id );

/*!
 *****************************************************************************
 * \brief Send the given message
 *
 * Must be called by the producer to transmit a single message to the queue.
 * This message can than be collected by the consumer.
 *
 * \param id : identifier for queue to transmit
 * \param ptr : pointer to the data to be transmit (a copy of this data is
 * transmitted) - must not be 0
 * \param size : the data size to be transmitted (must be > 0 and < maxMsgSize)
 *
 * \return ERR_NONE copy of message inserted into the queue
 * \return ERR_PARAM there exists no queue with this id, or ptr is 0, or the
 *                   size is too small or too big
 * \return ERR_NOMEM there is no free message in the queue
 *****************************************************************************
 */
stError mqSend( uint8_t id, const void * ptr, uint32_t size );


/* Functions to be called by the consumer --------------------------------- */

/*!
 *****************************************************************************
 * \brief Returns the number of received messages. 
 *
 * Must be called by the consumer only. 
 *
 * \param id : identifier for queue to enquire
 *
 * \return >= 0 : number of messages that can be received
 *****************************************************************************
 */
uint32_t mqNumAvailableToReceive( uint8_t id );

/*!
 *****************************************************************************
 * \brief Returns the size of the next messages to be received. 
 *
 * Must be called by the consumer only. 
 *
 * \param id : identifier for queue to enquire
 *
 * \return > 0 : the size of the next message waiting to be received.
 * \return ==0 : there is no message ready to be received. 
 *****************************************************************************
 */
uint32_t mqPeekSize( uint8_t id ); 

/*!
 *****************************************************************************
 * \brief Peek the next message
 *
 * Must be called by the consumer to "peek" the next message in the queue.
 * I.e. the message is copied from the queue to the given buffer, but the
 * message stays in the queue (until it is removed by mqDiscard or mqReceive).
 *
 * \param id : identifier for queue to receive
 * \param ptr : pointer to the buffer where the data received shall be copied
 * - must not be 0
 * \param *size : pointer containing the buffer size, the function will insert
 * the copied data size in this 
 *
 * \return ERR_NONE copy of message inserted into the buffer, and size is 
 * set approriately
 * \return ERR_PARAM there exists no queue with this id, or ptr is 0, or the
 * size is too small 
 * \return ERR_NOMSG there is no message ready for peek (receive)
 * \return ERR_REQUEST the parameter size is too small
 *****************************************************************************
 */
stError mqPeek( uint8_t id, void * ptr, uint32_t * size );

/*!
 *****************************************************************************
 * \brief Peek the next message
 *
 * Must be called by the consumer to discard the next message in the queue
 * without copying the data.
 *
 * \param id : identifier for queue to receive
 *
 * \return ERR_NONE message is removed from the queue
 * \return ERR_PARAM there exists no queue with this id
 * \return ERR_NOMSG there is no message to be discarded
 *****************************************************************************
 */
stError mqDiscard( uint8_t id );

/*!
 *****************************************************************************
 * \brief Receive the next message
 *
 * Must be called by the consumer to receive the next message in the queue.
 * I.e. the message is copied from the queue to the given buffer and the
 * message is removed from the queue.
 *
 * \param id : identifier for queue to receive
 * \param ptr : pointer to the buffer where the data received shall be copied
 * - must not be 0
 * \param *size : pointer containing the buffer size, the function will insert
 * the copied data size in this 
 *
 * \return ERR_NONE copy of message inserted into the buffer, size is 
 * set approriately, and message is removed from the queue
 * \return ERR_PARAM there exists no queue with this id, or ptr is 0, or the
 * size is too small 
 * \return ERR_NOMSG there is no message ready for receive
 * \return ERR_REQUEST the parameter size is too small
 *****************************************************************************
 */
stError mqReceive( uint8_t id, void * ptr, uint32_t * size );


/*!
 *****************************************************************************
 * \brief Read out usage high water mark
 *
 * This function may be called to analyse the actuall usage of the
 * message queue.
 *
 * \param id : identifier for queue to receive
 * \param ptrHighWaterMark : pointer to the buffer where hig water mark shall be copied
 *
 * \return ERR_NONE copy of current high water mark is stored at *ptrhighWaterMark.
 * \return ERR_PARAM there exists no queue with this id, or ptrhighWaterMark is 0
 * \return ERR_NOT_IMPLEMENTED The feature is not available due to \#ifdef
 *****************************************************************************
 */
stError mqGetHighWaterMark(uint8_t id, uint32_t* ptrHighWaterMark);

/*!
 *****************************************************************************
 * \brief Reset high water mark
 *
 * This function may be called to analyse the actuall usage of the
 * message queue, under different work loads without reseting.
 *
 * \param id : identifier for queue to receive
 *
 * \return ERR_NONE The high water mark was reset to the current usage level
 * \return ERR_PARAM there exists no queue with this id
 * \return ERR_NOT_IMPLEMENTED The feature is not available due to \#ifdef
 *****************************************************************************
 */
stError mqResetHighWaterMark(uint8_t id);


#ifdef __cplusplus
}
#endif

#endif /* MESSAGE_QUEUE_H */

