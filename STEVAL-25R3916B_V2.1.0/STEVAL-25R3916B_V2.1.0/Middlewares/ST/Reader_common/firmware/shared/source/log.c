
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
 *      PROJECT:   NFCC firmware
 *      LANGUAGE:  ISO C99
 */

 /*! \file
 *
 *  \author Oliver Regenfelder
 *
 *  \brief Logging API implementation.
 *  
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "sio.h"
#include "utils.h"


/*
******************************************************************************
* DEFINES
******************************************************************************
*/
#define snprintf     snprintf           /*!< snprintf macro   */

/*
******************************************************************************
* MACROS
******************************************************************************
*/

/*
******************************************************************************
* LOCAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Log output callback which discards any log messages.
 *
 * \note Declared without parameters to avoid "unused parameter" warnings.
 *****************************************************************************
 */
static void logNullOutput(const char *string);

/*!
 *****************************************************************************
 * \brief Callback to handle logging output.
 *
 * This callback is called by the logging module to perform the actual
 * output operation.
 *****************************************************************************
 */
static void (*logOutput)(const char *string) = logNullOutput;

/*
******************************************************************************
* LOCAL TABLES
******************************************************************************
*/

/*! Stores the active log level of each module. */
static uint8_t logLogLevel[LOG_MAX_MODULES];
/*! Buffer to compose log output strings. */
static char logBuffer[LOG_BUFFER_SIZE];

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/


/**
 *****************************************************************************
 * Fill to be printed bytes at proper positon of logBuffer
 *
 *****************************************************************************
 */
static void fillLogBufferAndPrint(const char *format, va_list args);

/*
******************************************************************************
* GLOBAL VARIABLE DEFINITIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

void logInit( void (*outputHandler)(const char *string) )
{
    uint32_t index;
    
    // Assign logOutput handler if it has been passed
    if(outputHandler)
    {
        logOutput = outputHandler;
    }
    else  // otherwise use logNullOutput()
    {
        InitializeUART( LOG_BAUDRATRE );
    }

    // Set default module to warn level
    logLogLevel[LOG_MODULE_DEFAULT] = LOG_LEVEL_WARN;

    for (index = 1; index < LOG_MAX_MODULES; index++)
        logLogLevel[index] = LOG_LEVEL_NONE;
}

void logSetLevel(uint8_t module, uint8_t level)
{
    if (module >= LOG_MAX_MODULES)
    {
        /* log error here. */
        return;
    }
    logLogLevel[module] = level;
}

bool logLoggingIsEnabled(uint8_t module, uint8_t level)
{
    if (  (module < LOG_MAX_MODULES)
       && (level >= logLogLevel[module]))
        return 1;
       
    return 0;
}

void testPrintf(const char *format, ...)
{
    va_list args;

    logBuffer[0] = 'T';
    logBuffer[1] = ':';

    va_start(args, format);
    fillLogBufferAndPrint(format, args);
    va_end(args);
}

void logPrintf(uint8_t module, uint8_t level, const char *format, ...)
{
    if (module >= LOG_MAX_MODULES)
    {
        /* log error here. */
        return;
    }
    if (level <= logLogLevel[module])
    {
        va_list args;

        switch (level)
        {
            case LOG_LEVEL_ERROR: logBuffer[0] = 'E'; break;
            case LOG_LEVEL_WARN:  logBuffer[0] = 'W'; break;
            case LOG_LEVEL_INFO:  logBuffer[0] = 'I'; break;
            case LOG_LEVEL_DEBUG: logBuffer[0] = 'D'; break;
            default: logBuffer[0] = ' ';
        }
        logBuffer[1] = ':';

        va_start(args, format);
        fillLogBufferAndPrint(format, args);
        va_end(args);
    }
}

void logDump(uint8_t module, uint8_t level, const void *address, uint32_t length)
{
    char *dst = logBuffer;
    const char *src = (char*)address;
    uint32_t idx = 0;

    if (module >= LOG_MAX_MODULES)
    {
        /* log error here. */
        return;
    }
    if (level > logLogLevel[module])
    {
        return;
    }

    while (idx < length)
    {
        snprintf(dst, 4, "%02X ", *src++);   
        dst += 3;
        idx++;
        if (0 == idx % 16)
        {
            *dst++ = '\n';
            *dst++ = '\r';
            *dst++ = '\0';
            logOutput(logBuffer);
            dst = logBuffer;
            *dst = '\0';
        }
    }
    *dst++ = '\n';
    *dst++ = '\r';
    *dst++ = '\0';
    logOutput(logBuffer);
}
    
/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/

static void fillLogBufferAndPrint(const char *format, va_list args)
{
    int numCharsInLogBuffer;

    numCharsInLogBuffer = vsnprintf(logBuffer + 2, sizeof(logBuffer) - 2, format, args);

    if (numCharsInLogBuffer < 0)
    {
        logOutput("vsnprintf failed\n");
    }
    else
    {
        logOutput(logBuffer);
        if (((size_t) numCharsInLogBuffer) >= sizeof(logBuffer) - 2)
            logOutput("log message truncated\n");
    }
}

static void logNullOutput(const char *string)
{
    WriteUARTN( string, MIN(strlen(string), LOG_BUFFER_SIZE) );
}
