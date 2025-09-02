
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
 *  \brief Implementation of a ring buffer.
 *
 * Note: The ring buffer is not thread safe.
 *
 */

/*
******************************************************************************
* Includes
******************************************************************************
*/

#include "ring_buffer.h"
#include "malloc.h"

/*
******************************************************************************
* Macros
******************************************************************************
*/

/** returns a pointer to the ring buffer indicated by id */
#define RB_GET_RING_BUFFER(id) \
  ( &( rbInfo.ringBuffers[ (id) ] ) ) 

/** returns <>0 if ring buffer was created and 0 if ring buffer was not yet created.
   Any valid ring buffer has a length != 0. */
#define RB_IS_CREATED( rb ) \
  ( (rb)->length )
 
/** The ring buffer is full, when the fullHead == ( fullTail + length ) */
#define RB_IS_FULL( rb ) \
  ( (rb)->fullHead == (rb)->fullTail + (rb)->length )

/** The ring buffer is empty, when the fullHead == fullTail */
#define RB_IS_EMPTY(rb) \
  ( (rb)->fullHead == (rb)->fullTail )

/** number of entries in ring buffer: fullTail is always <= fullHead. As we use a
   32-bit value, the difference will always be positive, even when fullHead
   wraps around to 0. This is part of the unsigned calculation rules. */
#define RB_NO_OF_ENTRIES(rb) \
  ( (rb)->fullHead - (rb)->fullTail )

/** number of free places in the ring buffer: length - ( fullHead - fullTail). As
   for the rules see RB_NO_OF_ENTRIES */
#define RB_NO_OF_FREE(rb) \
  ( (rb)->length - RB_NO_OF_ENTRIES(rb) )


/*
******************************************************************************
* Variables
******************************************************************************
*/

/** This ring buffer inserts at the head, and than increments the head afterwards. 
 * This ring buffer removes at the tail, and than increments the tail afterwards. 
 * So Head points to the next location to be filled, and tail points to the 
 * next location to be removed. 
 * The "pointers" head and tail are only used to find the location in the
 * ring buffer. They are not used for "fullness" or "emptiness" calculations. 
 * For this we use the fullHead and fullTail "pointers". These are 32-bit
 * counters that use 32-bit arithmetic for calculations. I.e. the fullHead
 * will always be "bigger" or equal to the fullTail. A subtract from 
 * fullHead - fullTail will always be zero or a positive 32-bit value.
 * Even if we do something like: e.g. fullHead = 2, fullTail = 0xFFFFFFFE,
 * the difference will be 4. 
 * With this unsigned calculation arithmetic it becomes easy to know whether
 * the ring buffer is full or empty, and also to get the number of elements in the
 * the ring buffer and the number of elements free.
 * The ring buffer is empty when: (fullHead == fullTail ).
 * The ring buffer is full when: (fullHead == fullTail + length ).
 * The number of elements in the ring buffer is: (fullHead - fullTail).
 * The number of elements free in the ring buffer is: (length - (fullHead - fullTail) ).
 *
 * The "pointer" fullHead is incremented as the last statement in the insertion,
 * so that the consumer will only see this data in the ring buffer, when this step
 * is done. (As the "seeing" of the consumer depends on the value of the
 * fullHead counter.)
 * The "pointer" fullTail is incremented as the last statement in the removal,
 * so that the producer will only see the free place in the ring buffer, when this
 * step is done. (As the "seeing" of the producer depends on the value of the
 * fullTail counter.)
 */ 

typedef struct _rbRingBuffer
{
  uint32_t length; /**< number of bytes in this ring buffer */
  uint32_t head; /**< use this as index in the ring buffer to "point" to the next place for insert */
  uint32_t tail; /**< use this as index in the ring buffer to "point" to the next place for removal */
  uint32_t fullHead; /**< index of head with natural wrap at 32-bit. */
  uint32_t fullTail; /**< index of tail with natural wrap at 32-bit. */
  uint8_t * data; /**< the data array */ 
} rbRingBuffer;

/** overall struct, contains the array of ring buffers. Each ring buffer can have a different 
   length (= maximum number of bytes) */
typedef struct _rbInformation 
{
    rbRingBuffer ringBuffers[ RB_MAX_NUMBER_OF_RING_BUFFERS ]; /**< actual instantiation of the ringbuffers*/
} rbInformation;

/* The instance containing all individual ring buffers */
static rbInformation rbInfo;


/*
******************************************************************************
* Local Functions
******************************************************************************
*/

/* increment head and fullHead of ring buffer. Takes care that head always rolls
   over at "length". fullHead will only roll over when its natural limit
   is reached (which is 0xFFFFFFFF). For this roll-over nothing has to be
   done in sw. */
static void rbIncHead ( rbRingBuffer * rb, uint32_t inc )
{
    uint32_t newHead = rb->head + inc;
    if ( newHead >= rb->length )
    {
        newHead -= rb->length;
    }
    rb->head = newHead;
    /* show that object is in ring buffer only as last step */
    newHead = rb->fullHead + inc;
    (rb->fullHead) = newHead;
}

/* increment tail and fullTail of ring buffer. Takes care that tail always rolls
   over at "length". fullTail will only roll over when its natural limit
   is reached (which is 0xFFFFFFFF). For this roll-over nothing has to be
   done in sw. */
static void rbIncTail ( rbRingBuffer * rb, uint32_t inc )
{
    uint32_t newTail  = rb->tail + inc;
    if ( newTail >= rb->length )
    {
        newTail -= rb->length;
    }
    rb->tail = newTail;
    /* show that ring buffer place is free again, only as last step */
    newTail = rb->fullTail + inc;
    (rb->fullTail) = newTail;
}


/* gets or discards up to size bytes. Depending on value of ptr this is
   a discard or get */
static ReturnCode rbGetOrDiscard  ( uint8_t id, void * ptr, uint32_t * size )
{
    rbRingBuffer * rb;
    uint8_t * data;
    uint8_t * outData = (uint8_t *)ptr;
    uint32_t i;
    uint32_t noEntries;
    /* 1. sanity checks for parameters */
    if (  id >= RB_MAX_NUMBER_OF_RING_BUFFERS 
       || ! size
       || *size == 0 
       )
    {
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 2. get ring buffer and check that it exists */
    rb = RB_GET_RING_BUFFER( id );
    if ( ! RB_IS_CREATED( rb ) )
    { /* no such ring buffer was created */
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 3. and check there is at least 1 data */
    if ( RB_IS_EMPTY( rb ) )
    {
        return ERR_INSERT_RINGBUF_GRP(ERR_NOMSG);
    }
    /* 4. find out which is bigger, the stored data or the buffer */
    noEntries = RB_NO_OF_ENTRIES( rb );
    if ( *size < noEntries )
      { /* there is more data in the ring buffer than we can copy in the buffer */ 
	noEntries = *size;
    }
    /* 5. update size information */
    *size = noEntries;
    if ( ptr )
    { /* if ptr is zero this means discard */
	data = &( rb->data[ rb->tail ] );
	/* 6. copy data */
	for ( i = 0 ; i < noEntries; i++ )
	{
	    *outData++ = *data++;
	}
    }
    /* 7. now remove it from ring buffer */
    rbIncTail( rb, noEntries );
    return ERR_NONE;
}

/*
******************************************************************************
* Global Functions
******************************************************************************
*/

/* Functions to be called either by producer (exclusive-)or by consumer */

void rbInitialise ( )
{
    int i = 0;
    /* 1. for all possible ring buffer, clear the "pointers". length == 0 shows
       that the ring buffer was not yet created. */
    for ( i = 0; i < RB_MAX_NUMBER_OF_RING_BUFFERS; ++i )
    {
        rbRingBuffer * rb = RB_GET_RING_BUFFER( i );
        rb->length = 0;
        rb->head = 0;
        rb->tail = 0;
        rb->fullHead = 0;
        rb->fullTail = 0;
    }
}

ReturnCode rbCreate ( uint8_t id, uint32_t maxSize )
{
    rbRingBuffer * rb;
    /* 1. sanity check for parameters */
    if (  id >= RB_MAX_NUMBER_OF_RING_BUFFERS
       || maxSize == 0
       )
    {
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }

    /* 2. get a pointer to the ring buffer corresponding to the id */
    rb = RB_GET_RING_BUFFER( id );
    if ( RB_IS_CREATED( rb ) )
    { /* for this id a ring buffer has already been created */
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 3. allocate memory for the data headers and data data */
    rb->data = (uint8_t *)malloc( maxSize );
    if ( ! rb->data )
    { /* either could not allocate data */
        return ERR_INSERT_RINGBUF_GRP(ERR_NOMEM);
    }
    /* 4. set up sizes == indicate that RB_IS_CREATED */
    rb->length = maxSize;
    /* 5. clear internal states */
    return rbReset( id );
}
  
ReturnCode rbReset ( uint8_t id )
{
    rbRingBuffer * rb;
    uint32_t i;
    uint8_t * data;
    /* 1. sanity check for parameter */
    if (  id >= RB_MAX_NUMBER_OF_RING_BUFFERS )
    {
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 2. check that ring buffer exists */
    rb = RB_GET_RING_BUFFER( id );
    if ( ! RB_IS_CREATED( rb ) )
    { /* no such ring buffer was created */
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 3. make ring buffer empty */
    rb->head = 0;
    rb->tail = 0;
    rb->fullHead = 0;
    rb->fullTail = 0;
    /* 4. clear whole data memory of this ring buffer */
    data = rb->data;
    for ( i = 0; i < rb->length; ++i )
    {
        *data++ = 0; 
    }
    return ERR_NONE;
}

/* Functions to be called by producer -------------------------- */

uint32_t rbNumAvailableToPut ( uint8_t id )
{
    rbRingBuffer * rb;
    /* 1. sanity check of parameter */
    if (  id >= RB_MAX_NUMBER_OF_RING_BUFFERS )
    {
        return 0;
    }
    /* 2. get ring buffer and calc the number of free */
    rb = RB_GET_RING_BUFFER(id);
    /* Note: if ring buffer was never created, this function will also 
       return 0 as free */
    return (uint32_t)RB_NO_OF_FREE( rb ); 
}

ReturnCode rbPut ( uint8_t id, const void * ptr, uint32_t size )
{
    rbRingBuffer * rb;
    uint8_t * data;
    const uint8_t * inData = (const uint8_t*)ptr;
    uint32_t i;
    /* 1. sanity checks for parameters */
    if (  id >= RB_MAX_NUMBER_OF_RING_BUFFERS 
       || ! ptr
       || size == 0 
       )
    {
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 2. get ring buffer and check that it exists */
    rb = RB_GET_RING_BUFFER( id );
    if ( ! RB_IS_CREATED( rb ) )
    { /* no such ring buffer was created */
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    /* 3. see that there is enough space */ 
    if ( RB_NO_OF_FREE( rb ) < size )
    {
        return ERR_INSERT_RINGBUF_GRP(ERR_NOMEM);
    }
    /* 4. get data data pointer and copy data */
    data = &( rb->data[ rb->head ] );
    for ( i = 0; i < size; ++i )
    {
        *data++ = *inData++;
    }
    /* 6. increment fullHead and head counters */
    rbIncHead( rb, size );

    return ERR_NONE;
}

/* Functions to be called by consumer -------------------------- */

uint32_t rbNumAvailableToGet ( uint8_t id )
{
    rbRingBuffer * rb;
    /* 1. sanity check of parameter */
    if (  id >= RB_MAX_NUMBER_OF_RING_BUFFERS )
    {
        return 0;
    }
    /* 2. get ring buffer and calc the number of filled */
    rb = RB_GET_RING_BUFFER( id );
    /* Note: if ring buffer was never created, this function will also 
       return 0 as available */
    return (uint32_t)RB_NO_OF_ENTRIES( rb ); 
}

ReturnCode rbGet ( uint8_t id, void * ptr, uint32_t * size )
{
    /* 1. sanity checks for parameters */
    if ( ! ptr )
    { /* get must have a valid ptr parameter */
        return ERR_INSERT_RINGBUF_GRP(ERR_PARAM);
    }
    return rbGetOrDiscard( id, ptr, size );
}

ReturnCode rbDiscard ( uint8_t id, uint32_t * size )
{
    return rbGetOrDiscard( id, 0, size );
}



