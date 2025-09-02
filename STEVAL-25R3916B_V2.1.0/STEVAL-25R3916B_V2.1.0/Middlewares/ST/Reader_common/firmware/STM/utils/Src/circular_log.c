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

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include "circular_buf.h"
#include "circular_log.h"
#include "st_errno.h"

#define ADD_TIMESTAMP
#ifdef ADD_TIMESTAMP
#include <limits.h>
#include "main.h"
#endif

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

static circular_buf_t fifo_logs;

static log_t log_buffer[CIRCLOG_BUFFER_COUNT];

static bool isEnabled = false;


/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

/*****************************************************************************/
stError circularLogInit(bool enable)
{
    circular_buf_init(&fifo_logs, (uint8_t*)log_buffer, CIRCLOG_BUFFER_SIZE, CIRCLOG_BUFFER_COUNT);

    /* Enable logging temporarily to send the init message */
    isEnabled = true;

    circularLogString("READER LOG: %s", enable ? "enabled" : "disabled");

    isEnabled = enable;

    return ERR_NONE;
}

/*****************************************************************************/
stError circularLogBuffer(const uint8_t* data, uint16_t length, bool flush)
{
    if ( (data == NULL) || (! isEnabled) )
    {
        return ERR_IGNORE;
    }

    uint32_t offset = 0;
    while (offset < length)
    {
        uint32_t bufLen = length - offset;
        uint32_t available = circular_buf_put_length_available(&fifo_logs);
        if (bufLen > available) {
            bufLen = available;
        }
        uint8_t* buf = (uint8_t*)&data[offset];
        /* Keep oldest logs, discard newest log */
        int r = circular_buf_put(&fifo_logs, buf, bufLen, flush);
        if (r <= 0)
        {
            return ERR_INTERNAL;
        }
        offset += bufLen;
    }

    return ERR_NONE;
}

#ifdef ADD_TIMESTAMP
/*****************************************************************************/
static uint32_t numDigits(int32_t n)
{
    uint32_t r = 1;
    if (n < 0) n = (n == INT_MIN) ? INT_MAX: -n;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}
#endif

/*****************************************************************************/
stError circularLogHex(const uint8_t* data, uint16_t length)
{
    if ( (data == NULL) || (! isEnabled) )
    {
        return ERR_IGNORE;
    }

    uint8_t temp[(4 * CIRCLOG_BUFFER_SIZE) + 1];
    char bytes[3]; /* needs an extra byte for '\0' */

    uint16_t stringLen = length; /* 1 byte makes 2 characters */
    if (stringLen > sizeof(temp) / 2)
    {
        stringLen = sizeof(temp) / 2;
        circularLogString("Buffer truncated");
    }

    uint32_t offset = 0;
    for (uint32_t i = 0; i < stringLen; i++)
    {
        volatile int r = snprintf((char*)&bytes, sizeof(bytes), "%02X", data[i]);
        UNUSED(r);
        temp[offset++] = bytes[0];
        temp[offset++] = bytes[1];
    }

    return circularLogBuffer(temp, stringLen * 2, true);
}

/*****************************************************************************/
stError circularLogString(const char* format, ...)
{
    if (! isEnabled)
    {
        return ERR_IGNORE;
    }

    uint8_t temp[2 * CIRCLOG_BUFFER_SIZE] = { 0 };
    uint32_t offset = 0;

#ifdef ADD_TIMESTAMP
    /* Add timestamp */
    uint32_t tick = HAL_GetTick();
    snprintf((char*)temp, sizeof(temp), "%"PRIu32 , tick);
    offset = numDigits(tick) + 1;
#endif

    /* Write the formatted log string to its destination buffer */
    va_list list;
    va_start(list, format);

    vsnprintf((char*)temp + offset, sizeof(temp) - offset, format, list);

    va_end(list);

    /* Add the formatted log string to the log buffer */
    return circularLogBuffer(temp, strlen((char*)temp), true);
}

/*****************************************************************************/
stError circularLogGet(uint8_t** string, uint32_t* length)
{
    if ((string == NULL) || (length == NULL))
    {
        return ERR_PARAM;
    }

    *string = NULL;
    *length = 0;

    bool found = circular_buf_get(&fifo_logs, string, length);
    UNUSED(found);

    return ERR_NONE;
}

/*****************************************************************************/
/*****************************************************************************/
#ifdef TEST_CIRCULAR_LOG

#include <assert.h>
uint32_t test_circular_log()
{
    circularLogInit(true);

    uint8_t* message;
    uint32_t messageLen;
    do {
        circularLogGet(&message, &messageLen);
    } while (messageLen != 0);

    char string01[] = "1. hello";
    char string02[] = "2. hello %s";
    char string03[] = "3. count %d";
    char string04[] = "4. count 0x%x";
    char string05[] = "5. hello too";

    stError err;
    err = circularLogString(string01);
    assert(err == ERR_NONE);
    err = circularLogString(string02, "world");
    assert(err == ERR_NONE);
    err = circularLogString(string03, 7);
    assert(err == ERR_NONE);
    err = circularLogString(string04, 0x77);
    assert(err == ERR_NONE);
    err = circularLogString(string05);
    assert(err != ERR_NONE);

    err = circularLogGet(&message, &messageLen);
    assert(err == ERR_NONE);
    assert(messageLen == sizeof(string01) -1);

    err = circularLogGet(&message, &messageLen);
    assert(err == ERR_NONE);
    assert(messageLen == /*sizeof(string02) -1 + sizeof("world")*/ 0x0E);

    err = circularLogGet(&message, &messageLen);
    assert(messageLen == /*sizeof(string03)*/0x0A);
    assert(err == ERR_NONE);

    err = circularLogGet(&message, &messageLen);
    //assert(messageLen == sizeof(string04) -1);
    assert(err == ERR_NONE);

    err = circularLogGet(&message, &messageLen);
    assert(messageLen == 0);

    uint8_t data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08 };

    circularLogBuffer(data, 4, false);
    err = circularLogGet(&message, &messageLen);
    assert(messageLen == 0);
    circularLogBuffer(data, 4, false);
    err = circularLogGet(&message, &messageLen);
    assert(messageLen == 0);
    circularLogBuffer(data, 4, true);

    err = circularLogGet(&message, &messageLen);
    //assert(err == ERR_NONE);
    assert(messageLen == 12);

    circularLogInit(true);
    do {
        circularLogGet(&message, &messageLen);
    } while (messageLen != 0);

    //circularLogBuffer(&data[0], 4, false);
    //circularLogBuffer(&data[4], 4, false);
    //circularLogBuffer(&data[8], 4, true);
    //err = circularLogGet(&message, &messageLen);
    //err = circularLogGet(&message, &messageLen);
    //err = circularLogGet(&message, &messageLen);
    //err = circularLogGet(&message, &messageLen);

    //circularLogInit(true);

    uint8_t apdu[20];
    err = circularLogHex((uint8_t*)apdu, sizeof(apdu));
    assert(err == ERR_NONE);

    err = circularLogBuffer((uint8_t*)"EMV: R-APDU ", strlen("EMV: R-APDU "), false);
    err = circularLogString("EMV: R-APDU ");
    err = circularLogHex((uint8_t*)apdu, sizeof(apdu));
    //assert(err == ERR_NONE);
    //
    //circularLogGet(&message, &messageLen);
    //assert(messageLen == 16);
    //circularLogGet(&message, &messageLen);
    //assert(messageLen == 16);
    //circularLogGet(&message, &messageLen);
    //assert(messageLen == 16);

    return 0;
}
#endif
