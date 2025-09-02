
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
 *  \brief Implementation of a message queue.
 *
 * Note: The message queue is not thread safe.
 *
 */

/*
******************************************************************************
* Includes
******************************************************************************
*/
#if defined (__CC_ARM) || defined(__ARMCC_VERSION)
    #include <stdlib.h>
#elif defined (__GNUC__)
    #include "malloc.h"
#endif
#include "utils.h"
#include "message_queue.h"


/*
******************************************************************************
* Macros
******************************************************************************
*/

/** returns a pointer to the queue indicated by id */
#define MQ_GET_QUEUE(id) \
  ( &( mqInfo.queues[ (id) ] ) ) 

/** returns <>0 if queue was created and 0 if queue was not yet created.
   Any valid queue has a length != 0. */
#define MQ_IS_CREATED(queue) \
  ( (queue)->length )
 
/** get a pointer to the n-th message header */
#define MQ_GET_MSG_HEADER(queue,index) \
  ( (mqMessageHeader*)( &( (queue)->msgHeader[ index ] ) ) )

/** get the n-th message */
#define MQ_GET_MSG(queue,index) \
  ( (uint8_t*)( &( (queue)->msgData[ (queue)->msgSize * (index) ] ) ) ) 



/** The queue is full, when the fullHead == ( fullTail + length ) */
#define MQ_IS_FULL(queue) \
  ( (queue)->fullHead == (queue)->fullTail + (queue)->length )

/** The queue is empty, when the fullHead == fullTail */
#define MQ_IS_EMPTY(queue) \
  ( (queue)->fullHead == (queue)->fullTail )

/** number of entries in queue: fullTail is always <= fullHead. As we use a
   32-bit value, the difference will always be positive, even when fullHead
   wraps around to 0. This is part of the unsigned calculation rules. */
#define MQ_NO_OF_ENTRIES(queue) \
  ( (queue)->fullHead - (queue)->fullTail )

/** number of free places in the queue: length - ( fullHead - fullTail). As
   for the rules see MQ_NO_OF_ENTRIES */
#define MQ_NO_OF_FREE(queue) \
  ( (queue)->length - MQ_NO_OF_ENTRIES(queue) )


/*
******************************************************************************
* Variables
******************************************************************************
*/

/** internal data to manage individual messages*/
typedef struct _mqMessageHeader
{
  uint32_t dataSize; /**< the actual size of the data in the message */
} mqMessageHeader;

/** This queue inserts at the head, and than increments the head afterwards. 
 * This queue removes at the tail, and than increments the tail afterwards. 
 * So Head points to the next location to be filled, and tail points to the 
 * next location to be removed. 
 * The "pointers" head and tail are only used to find the location in the
 * queue. They are not used for "fullness" or "emptiness" calculations. 
 * For this we use the fullHead and fullTail "pointers". These are 32-bit
 * counters that use 32-bit arithmetic for calculations. I.e. the fullHead
 * will always be "bigger" or equal to the fullTail. A subtract from 
 * fullHead - fullTail will always be zero or a positive 32-bit value.
 * Even if we do something like: e.g. fullHead = 2, fullTail = 0xFFFFFFFE,
 * the difference will be 4. 
 * With this unsigned calculation arithmetic it becomes easy to know whether
 * the queue is full or empty, and also to get the number of elements in the
 * the queue and the number of elements free.
 * The queue is empty when: (fullHead == fullTail ).
 * The queue is full when: (fullHead == fullTail + length ).
 * The number of elements in the queue is: (fullHead - fullTail).
 * The number of elements free in the queue is: (length - (fullHead - fullTail) ).
 *
 * The "pointer" fullHead is incremented as the last statement in the insertion,
 * so that the consumer will only see this message in the queue, when this step
 * is done. (As the "seeing" of the consumer depends on the value of the
 * fullHead counter.)
 * The "pointer" fullTail is incremented as the last statement in the removal,
 * so that the producer will only see the free place in the queue, when this
 * step is done. (As the "seeing" of the producer depends on the value of the
 * fullTail counter.)
 */ 

typedef struct _mqQueue
{
  uint32_t length; /**< number of messages in this queue */
  uint32_t msgSize; /**< each message in this queue can hold up to this number of bytes */
  uint32_t head; /**< use this as index in the queue to "point" to the next place for insert */
  uint32_t tail; /**< use this as index in the queue to "point" to the next place for removal */
  uint32_t fullHead; /**< index of head with natural wrap at 32-bit. */
  uint32_t fullTail; /**< index of tail with natural wrap at 32-bit. */
  /* 2 seperate arrays: one containing the message headers and the other the data
     (flat 1-dimensional array) */

#ifdef MQ_WATER_MARK_ANALYSIS
  uint32_t highWaterMark; /**< used for measuring the actuall resource usage
                      * of the message queue*/
#endif

  mqMessageHeader * msgHeader; /**< all necessary information about the data */
  uint8_t * msgData; /**< the real message array containing only plain data */ 
} mqQueue;

/** overall struct, contains the array of queues. Each queue can have a different 
   length (= maximum number of messages) and a different msgSize (=maximum size of
   each message). */
typedef struct _mqInformation 
{
  mqQueue queues[ MQ_MAX_NUMBER_OF_QUEUES ];/**< actual instantiation of the message queuesbuffers*/
} mqInformation;

/** The instance containing all individual message queues */
static mqInformation mqInfo;


/*
******************************************************************************
* Local Functions
******************************************************************************
*/

/* increment head and fullHead of queue. Takes care that head always rolls
   over at "length". fullHead will only roll over when its natural limit
   is reached (which is 0xFFFFFFFF). For this roll-over nothing has to be
   done in sw. */
static void mqIncHead ( mqQueue * queue )
{
    uint32_t newHead = queue->head + 1;
    if ( newHead >= queue->length )
    {
        newHead = 0;
    }
    queue->head = newHead;
    /* show that object is in queue only as last step */
    (queue->fullHead)++;
}

/* increment tail and fullTail of queue. Takes care that tail always rolls
   over at "length". fullTail will only roll over when its natural limit
   is reached (which is 0xFFFFFFFF). For this roll-over nothing has to be
   done in sw. */
static void mqIncTail ( mqQueue * queue )
{
    uint32_t newTail  = queue->tail + 1;
    if ( newTail >= queue->length )
    {
        newTail = 0;
    }
    queue->tail = newTail;
    /* show that queue place is free again, only as last step */
    (queue->fullTail)++;
}


/*
******************************************************************************
* Global Functions
******************************************************************************
*/

/* Functions to be called either by producer (exclusive-)or by consumer */

void mqInitialise ( void )
{
    int i = 0;
    /* 1. for all possible queue, clear the "pointers". length == 0 shows
       that the queue was not yet created. */
    for ( i = 0; i < MQ_MAX_NUMBER_OF_QUEUES; ++i )
    {
        mqQueue * queue = MQ_GET_QUEUE( i );
        queue->length = 0;
        queue->msgSize = 0;
        queue->head = 0;
        queue->tail = 0;
        queue->fullHead = 0;
        queue->fullTail = 0;
#ifdef MQ_WATER_MARK_ANALYSIS
        queue->highWaterMark = 0;
#endif
        queue->msgHeader = 0;
        queue->msgData = 0;
    }
}

stError mqCreate ( uint8_t id, uint32_t maxMsg, uint32_t maxMsgSize )
{
    mqQueue * queue;
    /* 1. sanity check for parameters */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES
       || maxMsg == 0
       || maxMsgSize == 0 
       )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }

    /* 2. get a pointer to the queue corresponding to the id */
    queue = MQ_GET_QUEUE( id );
    if ( MQ_IS_CREATED( queue ) )
    { /* for this id a queue has already been created */
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 3. allocate memory for the message headers and message data */
    queue->msgHeader = (mqMessageHeader *) malloc( maxMsg * sizeof( mqMessageHeader ) );
    queue->msgData   = (uint8_t *) malloc( maxMsg * maxMsgSize );
    if ( ! queue->msgHeader || ! queue->msgData )
    { /* either could not allocate msgHeader or msgData */
        return ERR_INSERT_MQ_GRP(ERR_NOMEM);
    }
    /* 4. set up sizes == indicate that MQ_IS_CREATED */
    queue->length = maxMsg;
    queue->msgSize = maxMsgSize;
#ifdef MQ_WATER_MARK_ANALYSIS
    queue->highWaterMark = 0;
#endif
    /* 5. clear internal states */
    return mqReset( id );
}
  
stError mqReset ( uint8_t id )
{
    mqQueue * queue;
    uint32_t i;
    uint8_t * data;
    /* 1. sanity check for parameter */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. check that queue exists */
    queue = MQ_GET_QUEUE( id );
    if ( ! MQ_IS_CREATED( queue ) )
    { /* no such queue was created */
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 3. make queue empty */
    queue->head = 0;
    queue->tail = 0;
    queue->fullHead = 0;
    queue->fullTail = 0;
#ifdef MQ_WATER_MARK_ANALYSIS
    queue->highWaterMark = 0;
#endif

    /* 4. clear message header of this queue */
    for ( i = 0; i < queue->length; ++i )
    {
        queue->msgHeader[i].dataSize = 0;
    }
    /* 5. clear whole data memory of this queue */
    data = queue->msgData;
    for ( i = queue->length * queue->msgSize; i > 0; --i )
    {
        *data++ = 0; 
    }
    return ERR_NONE;
}

/* Functions to be called by producer -------------------------- */

uint32_t mqNumAvailableToSend ( uint8_t id )
{
    mqQueue * queue;
    /* 1. sanity check of parameter */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return 0;
    }
    /* 2. get queue and calc the number of free */
    queue = &( mqInfo.queues[ id ] );
    /* Note: if queue was never created, this function will also 
       return 0 as free */
    return (uint32_t)MQ_NO_OF_FREE( queue ); 
}

stError mqSend ( uint8_t id, const void * ptr, uint32_t size )
{
    mqQueue * queue;
    mqMessageHeader * hdr;
    uint8_t * data;
    const uint8_t * inData = (const uint8_t*)ptr;
    uint32_t i;
    /* 1. sanity checks for parameters */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES 
       || ! ptr
       || size == 0 
       )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. get queue and check that it exists */
    queue = MQ_GET_QUEUE( id );
    if ( ! MQ_IS_CREATED( queue ) )
    { /* no such queue was created */
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 3. sanity check for parameters that depend on queue data */
    if( size > queue->msgSize  )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 4. see that there is at least 1 free message */ 
    if ( MQ_IS_FULL( queue ) )
    {
        return ERR_INSERT_MQ_GRP(ERR_NOMEM);
    }
    /* 5. set header of queue */
    hdr = MQ_GET_MSG_HEADER( queue, queue->head );
    hdr->dataSize = size;
    /* 6. get message data pointer and copy data */
    data = MQ_GET_MSG( queue, queue->head );
    for ( i = 0; i < size; ++i )
    {
        *data++ = *inData++;
    }
    /* 7. increment fullHead and head counters */
    mqIncHead( queue );
#ifdef MQ_WATER_MARK_ANALYSIS
    const uint32_t waterMark = MQ_NO_OF_ENTRIES(queue);

    if(queue->highWaterMark < waterMark)
    {
        queue->highWaterMark = waterMark;
    }
#endif
    return ERR_NONE;
}

/* Functions to be called by consumer -------------------------- */

uint32_t mqNumAvailableToReceive ( uint8_t id )
{
    mqQueue * queue;
    /* 1. sanity check of parameter */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return 0;
    }
    /* 2. get queue and calc the number of filled */
    queue = &( mqInfo.queues[ id ] );
    /* Note: if queue was never created, this function will also 
       return 0 as available */
    return (uint32_t)MQ_NO_OF_ENTRIES( queue ); 
}

uint32_t mqPeekSize ( uint8_t id )
{
    mqQueue * queue;
    mqMessageHeader * hdr;
    /* 1. sanity check for parameter */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. get queue and check that it exists */
    queue = &( mqInfo.queues[ id ] );
    if ( ! MQ_IS_CREATED( queue ) )
    { /* no such queue was created */
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 3. and check there is at least 1 message */
    if ( MQ_IS_EMPTY( queue ) )
    {
        return ERR_INSERT_MQ_GRP(ERR_NOMSG);
    }
    /* 4. get header of message, and read the size of the data */
    hdr = MQ_GET_MSG_HEADER( queue, queue->tail );
    return hdr->dataSize;
}

stError mqPeek ( uint8_t id, void * ptr, uint32_t * size )
{
    mqQueue * queue;
    mqMessageHeader * hdr;
    uint8_t * data;
    uint8_t * outData = (uint8_t *)ptr;
    uint32_t i;
    /* 1. sanity checks for parameters */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES 
       || ! ptr
       || ! size
       || *size == 0 
       )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. get queue and check that it exists */
    queue = &( mqInfo.queues[ id ] );
    if ( ! MQ_IS_CREATED( queue ) )
    { /* no such queue was created */
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 3. and check there is at least 1 message */
    if ( MQ_IS_EMPTY( queue ) )
    {
        return ERR_INSERT_MQ_GRP(ERR_NOMSG);
    }
    /* 4. get header of message, and check that the given buffer is large enough
          for the data in the message */
    hdr = MQ_GET_MSG_HEADER( queue, queue->tail );
    i = hdr->dataSize;
    if ( *size < i )
    { /* buffer size given is less than size required for message */
	return ERR_INSERT_MQ_GRP(ERR_REQUEST);
    }
    /* 5. update size information */
    *size = i;
    data = MQ_GET_MSG( queue, queue->tail );
    /* 6. copy data */
    for ( ; i > 0; i-- )
    {
        *outData++ = *data++;
    }
    return ERR_NONE;
}

stError mqDiscard ( uint8_t id )
{
    mqQueue * queue;
    mqMessageHeader * hdr;
    /* 1. sanity checks for parameters */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. get queue and check that there is at least 1 message */
    queue = &( mqInfo.queues[ id ] );
    if ( MQ_IS_EMPTY( queue ) )
    {
        return ERR_INSERT_MQ_GRP(ERR_NOMSG);
    }
    /* 3. get header of queue, and mark message as empty */
    hdr = MQ_GET_MSG_HEADER( queue, queue->tail );
    hdr->dataSize = 0; /* not really necessary */
    /* 4. now remove it really from queue */
    mqIncTail(queue);
    return ERR_NONE;
}

stError mqReceive ( uint8_t id, void * ptr, uint32_t * size )
{
    /* 1. peek the message from the queue */
    stError res = mqPeek( id, ptr, size );
    if ( res == ERR_NONE )
    {
        /* 2. discard the message if peek was successfull */
        return mqDiscard( id );
    }
    return res;
}

stError mqGetHighWaterMark (uint8_t id, uint32_t* ptrHighWaterMark)
{
#ifdef MQ_WATER_MARK_ANALYSIS
    mqQueue * queue;
    /* 1. sanity checks for parameters */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    if( NULL == ptrHighWaterMark )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. get queue and return high water mark */
    queue = &( mqInfo.queues[ id ] );
    return queue->highWaterMark;
#else
    NO_WARNING(id);
    NO_WARNING(ptrHighWaterMark);
    return ERR_INSERT_MQ_GRP(ERR_NOT_IMPLEMENTED);
#endif
}

stError mqResetHighWaterMark (uint8_t id)
{
#ifdef MQ_WATER_MARK_ANALYSIS
    mqQueue * queue;
    /* 1. sanity checks for parameters */
    if (  id >= MQ_MAX_NUMBER_OF_QUEUES )
    {
        return ERR_INSERT_MQ_GRP(ERR_PARAM);
    }
    /* 2. get queue and reset high water mark to the current level */
    queue = &( mqInfo.queues[ id ] );
    queue->highWaterMark = MQ_NO_OF_ENTRIES(queue);
    return ERR_NONE;
#else
    NO_WARNING(id);
    return ERR_INSERT_MQ_GRP(ERR_NOT_IMPLEMENTED);
#endif
}
