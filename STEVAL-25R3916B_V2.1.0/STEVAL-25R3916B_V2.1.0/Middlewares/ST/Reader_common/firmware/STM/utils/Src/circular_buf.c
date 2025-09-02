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
#include <string.h>
#include <stdarg.h>
#include "circular_buf.h"
#include "circular_log.h"

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

/*****************************************************************************/
bool circular_buf_reset(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return false;
    }

    h->head = 0;
    h->tail = 0;
    h->full = false;

    //for (uint32_t i = 0; i < h->capacity; i++)
    //{
    //    log_t* current = &((log_t*)h->buffer)[i];
    //    current->offset = 0;
    //    memset(current->buffer, 0xCB, h->maxLen);
    //}

#ifdef CIRCULAR_BUF_MONITOR
    h->put_count  = 0;
    h->full_count = 0;
    h->lost_count = 0;
    h->longest    = 0;
#endif

    return true;
}

/*****************************************************************************/
bool circular_buf_is_empty(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return false;
    }

    return ( (! h->full) && (h->head == h->tail));
}

/*****************************************************************************/
bool circular_buf_is_full(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return false;
    }

    return h->full;
}

/*****************************************************************************/
uint32_t circular_buf_capacity(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    return h->capacity;
}

/*****************************************************************************/
uint32_t circular_buf_size(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    uint32_t size = h->capacity;

    if (! h->full)
    {
        if(h->head >= h->tail)
        {
            size = (h->head - h->tail);
        }
        else
        {
            size = (h->capacity + h->head - h->tail);
        }
    }

    return size;
}

/*****************************************************************************/
uint32_t circular_buf_put_length_available(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    uint32_t size = 0;

    if (! h->full)
    {
        log_t* head = &((log_t*)h->buffer)[h->head];
        size = h->maxLen - head->offset;
    }

    return size;
}

#ifdef CIRCULAR_BUF_MONITOR
/*****************************************************************************/
uint32_t circular_buf_put_count(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    return h->put_count;
}

/*****************************************************************************/
uint32_t circular_buf_full_count(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    return h->full_count;
}

/*****************************************************************************/
uint32_t circular_buf_lost_count(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    return h->lost_count;
}

/*****************************************************************************/
uint32_t circular_buf_longest(cbuf_handle_t h)
{
    if (h == NULL)
    {
        return 0;
    }

    return h->longest;
}
#endif

/*****************************************************************************/
static inline void advance_head(cbuf_handle_t h)
{
    if (++(h->head) == h->capacity)
    {
        h->head = 0;
    }

    h->full = (h->head == h->tail);
#ifdef CIRCULAR_BUF_MONITOR
    if (h->full)
    {
        h->full_count++;
    }
    h->put_count++;
#endif
}

/*****************************************************************************/
static inline void advance_tail(cbuf_handle_t h)
{
    if (++(h->tail) == h->capacity)
    {
        h->tail = 0;
    }
}

/*****************************************************************************/
/*****************************************************************************/
cbuf_handle_t circular_buf_init(circular_buf_t* h, uint8_t* buffer, uint32_t length, uint32_t capacity)
{
    if ( (h == NULL) || (buffer == NULL) || (length == 0) || (capacity == 0))
    {
        return NULL;
    }

    cbuf_handle_t cbuf = h;

    cbuf->buffer   = buffer;
    cbuf->maxLen   = length;
    cbuf->capacity = capacity;

    circular_buf_reset(cbuf);

    for (uint32_t i = 0; i < h->capacity; i++)
    {
        log_t* current = &((log_t*)h->buffer)[i];
        current->offset = 0;
        memset(current->buffer, 0xCB, h->maxLen);
    }

    if (! circular_buf_is_empty(cbuf))
    {
        return NULL;
    }

    return cbuf;
}

/*****************************************************************************/
int32_t circular_buf_put(cbuf_handle_t h, uint8_t* data, uint32_t length, bool flush)
{
    int32_t res = 0;

    if ( (h == NULL) || (h->buffer == NULL) || (data == NULL) || (length > h->maxLen))
    {
        return 0;
    }

    if (! circular_buf_is_full(h))
    {
        log_t* head = &((log_t*)h->buffer)[h->head];
        if (head->offset + length > h->maxLen)
        {
            return head->offset - h->maxLen; /* <0, number of bytes available in this buffer */
        }
        memcpy(head->buffer + head->offset , data, length);
        head->offset += length;
        if ( (head->offset == h->maxLen) || flush )
        {
            advance_head(h);
        }
        res = length;
#ifdef CIRCULAR_BUF_MONITOR
        if (head->offset > h->longest)
        {
            h->longest = head->offset;
        }
#endif
    }
#ifdef CIRCULAR_BUF_MONITOR
    else
    {
        h->lost_count++;
    }
#endif

    return res;
}

/*****************************************************************************/
int32_t circular_buf_put_overwrite(cbuf_handle_t h, uint8_t* data, uint32_t length, bool flush)
{
    if ( (h == NULL) || (h->buffer == NULL) || (data == NULL) || (length > h->maxLen) )
    {
        return 0;
    }

    log_t* head = &((log_t*)h->buffer)[h->head];
    if (h->full)
    {
        head->offset = 0;
        /* advance tail, loose oldest data */
        advance_tail(h);
#ifdef CIRCULAR_BUF_MONITOR
        h->lost_count++;
#endif
    }

    if (head->offset + length > h->maxLen)
    {
        return head->offset - h->maxLen; /* <0, number of bytes available in this buffer */
    }

    memcpy(head->buffer + head->offset, data, length);
    head->offset += length;

    if ( (head->offset == h->maxLen) || flush || h->full )
    {
        advance_head(h);
    }
#ifdef CIRCULAR_BUF_MONITOR
    if (head->offset > h->longest)
    {
        h->longest = head->offset;
    }
#endif

    return length;
}

/*****************************************************************************/
bool circular_buf_get(cbuf_handle_t h, uint8_t** data, uint32_t* length)
{
    if ( (h == NULL) || (h->buffer == NULL) || (data == NULL) || (length == NULL))
    {
        return false;
    }

    bool res = false;

    *length = 0;

    if (! circular_buf_is_empty(h))
    {
        log_t* tail = &((log_t*)h->buffer)[h->tail];
        *data   = (uint8_t*)tail->buffer;
        *length = tail->offset;
        tail->offset = 0;

        advance_tail(h);
        h->full = false;
        res = true;
    }

    return res;
}

/*****************************************************************************/
/*****************************************************************************/
#ifdef TEST_CIRCULAR_BUF

#include <assert.h>
uint32_t test_circular_buf()
{
    circular_buf_t cbuf;

    log_t log_buffer[2];

    /* basic tests */
    circular_buf_init(&cbuf, (uint8_t*)log_buffer, CIRCLOG_BUFFER_SIZE, sizeof(log_buffer)/sizeof(log_buffer[0]));
    assert(circular_buf_is_empty(&cbuf) == true);
    assert(circular_buf_size(&cbuf) == 0);

    /* put, with flush */
    uint8_t data01[] = "test01";
    uint8_t data02[] = "test0202";
    volatile int32_t bytes;
    bytes = circular_buf_put(&cbuf, data01, sizeof(data01), true);
    assert(bytes == sizeof(data01));
#ifdef CIRCULAR_BUF_MONITOR
    assert(circular_buf_put_count(&cbuf) == 1);
#endif
    assert(circular_buf_size(&cbuf) == 1);
    bytes = circular_buf_put(&cbuf, data02, sizeof(data02), true);
    assert(bytes == sizeof(data02));
#ifdef CIRCULAR_BUF_MONITOR
    assert(circular_buf_put_count(&cbuf) == 2);
#endif
    assert(circular_buf_size(&cbuf) == 2);
    assert(circular_buf_is_full(&cbuf) == true);

    bytes = circular_buf_put(&cbuf, data01, sizeof(data01), true);
    assert(bytes == 0);

    assert(circular_buf_is_full(&cbuf) == true);
#ifdef CIRCULAR_BUF_MONITOR
    assert(circular_buf_full_count(&cbuf) == 1);
    assert(circular_buf_lost_count(&cbuf) == 1);
#endif
    assert(circular_buf_size(&cbuf) == 2);

    uint8_t* data;
    uint32_t length,
    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == sizeof(data01));
    assert(circular_buf_size(&cbuf) == 1);
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == false);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == sizeof(data02));
    assert(circular_buf_size(&cbuf) == 0);
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == true);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == false);
    assert(length == 0);
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == true);

    /* put, no flush */
    circular_buf_reset(&cbuf);
    assert(circular_buf_is_empty(&cbuf) == true);

    bytes = circular_buf_put(&cbuf, data01, sizeof(data01), false);
    assert(bytes == sizeof(data01));
    assert(circular_buf_size(&cbuf) == 0);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == false);
    assert(length == 0);

    uint8_t longData[] = "0102030405060708090A0B0C0D0E0F";
    uint32_t available = circular_buf_put_length_available(&cbuf);

    bytes = circular_buf_put(&cbuf, longData, available, false);
    assert(bytes == available);
    assert(circular_buf_size(&cbuf) == 1);
    assert(sizeof(data01) + available == CIRCLOG_BUFFER_SIZE);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == CIRCLOG_BUFFER_SIZE);

    /* overwrite, with flush */
    circular_buf_reset(&cbuf);
    assert(circular_buf_size(&cbuf) == 0);

    bytes = circular_buf_put_overwrite(&cbuf, data01, sizeof(data01), true);
    assert(bytes == sizeof(data01));
    assert(circular_buf_size(&cbuf) == 1);
    bytes = circular_buf_put_overwrite(&cbuf, data02, sizeof(data02), true);
    assert(bytes == sizeof(data02));
    assert(circular_buf_size(&cbuf) == 2);

    uint8_t data03[] = "test030303";
    bytes = circular_buf_put_overwrite(&cbuf, data03, sizeof(data03), true);
    assert(bytes == sizeof(data03));
    assert(circular_buf_is_full(&cbuf) == true);
#ifdef CIRCULAR_BUF_MONITOR
    assert(circular_buf_lost_count(&cbuf) == 1);
#endif
    assert(circular_buf_size(&cbuf) == 2);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == sizeof(data02));
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == false);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == sizeof(data03));
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == true);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == false);
    assert(length == 0);
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == true);

    /* overwrite, no flush */
    circular_buf_reset(&cbuf);

    uint8_t dataFull01[CIRCLOG_BUFFER_SIZE] = { 1 };
    uint8_t dataFull02[CIRCLOG_BUFFER_SIZE] = { 2 };
    bytes = circular_buf_put_overwrite(&cbuf, dataFull01, sizeof(dataFull01), false);
    assert(bytes == sizeof(dataFull01));
    assert(circular_buf_size(&cbuf) == 1);
    bytes = circular_buf_put_overwrite(&cbuf, dataFull02, sizeof(dataFull02), false);
    assert(bytes == sizeof(dataFull02));
    assert(circular_buf_size(&cbuf) == 2);
    assert(circular_buf_is_full(&cbuf) == true);

    bytes = circular_buf_put_overwrite(&cbuf, data01, sizeof(data01), false);
    assert(bytes == sizeof(data01));
    assert(circular_buf_is_full(&cbuf) == true);
#ifdef CIRCULAR_BUF_MONITOR
    assert(circular_buf_lost_count(&cbuf) == 1);
#endif
    assert(circular_buf_size(&cbuf) == 2);
    /* Get the 2 buffers */
    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == sizeof(dataFull02));
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == false);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == true);
    assert(length == sizeof(data01));
    assert(circular_buf_is_full(&cbuf) == false);
    assert(circular_buf_is_empty(&cbuf) == true);

    success = circular_buf_get(&cbuf, &data, &length);
    assert(success == false);
    assert(length == 0);

    return 0;
}
#endif
