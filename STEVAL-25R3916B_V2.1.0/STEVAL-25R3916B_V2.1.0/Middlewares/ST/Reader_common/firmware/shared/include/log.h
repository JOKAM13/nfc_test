
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
 *  \brief Logging API.
 *
 */

#ifndef LOG_H
#define LOG_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
******************************************************************************
* DEFINES
******************************************************************************
*/

#define LOG_MODULE_DEFAULT  0      /*!< Default log module ID */
#define LOG_MAX_MODULES     32     /*!< Maximum number of logging modules */
#define LOG_BUFFER_SIZE     256    /*!< Maximum buffer size */
#define LOG_BAUDRATRE       115200 /*!< Log UART baudrate */

#define LOG_LEVEL_NONE    0   /*!< Log disabled */
#define LOG_LEVEL_ERROR   1   /*!< Critical error which we don't know how to handle */
#define LOG_LEVEL_WARN    2   /*!< Regular behavior but hint of possible bug */
#define LOG_LEVEL_INFO    3   /*!< Low frequent printing of important events */
#define LOG_LEVEL_DEBUG   4   /*!< Any other behavior which are expected */


#ifdef __GNUC__
    /*! Prinf Format Check macro */
    #define LOG_PRINTF_FORMAT_CHECK    __attribute__((format(printf, 3, 4)))
    /*! Test prinf Format Check macro */
    #define TEST_PRINTF_FORMAT_CHECK    __attribute__((format(printf, 1, 2)))
#else
    /*! Prinf Format Check macro */
    #define LOG_PRINTF_FORMAT_CHECK   
    /*! Test prinf Format Check macro */
    #define TEST_PRINTF_FORMAT_CHECK  
#endif

/*
******************************************************************************
* GLOBAL MACROS
******************************************************************************
*/

#ifdef LOG_DEBUG
    #define LOG_LEVEL           LOG_LEVEL_DEBUG   /*!< DEBUG Log level. */

    #define LOG_DEBUG_ENABLED   (true)      /*!< DEBUG Log level with debug logging turned on. */
    #define LOG_INFO_ENABLED    (true)      /*!< DEBUG Log level with information logging turned on. */
    #define LOG_WARN_ENABLED    (true)      /*!< DEBUG Log level with warning logging turned on. */
    #define LOG_ERR_ENABLED     (true)      /*!< DEBUG Log level with error logging turned on. */
#elif defined  LOG_INFO
    #define LOG_LEVEL           LOG_LEVEL_INFO    /*!< INFO Log level. */

    #define LOG_INFO_ENABLED    (true)      /*!< INFO Log level with information logging turned on. */
    #define LOG_WARN_ENABLED    (true)      /*!< INFO Log level with warning logging turned on. */
    #define LOG_ERR_ENABLED     (true)      /*!< INFO Log level with error logging turned on. */
#elif defined LOG_WARN
    #define LOG_LEVEL           LOG_LEVEL_WARN    /*!< WARN Log level. */

    #define LOG_WARN_ENABLED    (true)      /*!< WARN Log level with warning logging turned on. */
    #define LOG_ERR_ENABLED     (true)      /*!< WARN Log level with error logging turned on. */
#elif defined LOG_ERR
    #define LOG_LEVEL           LOG_LEVEL_ERROR   /*!< ERROR Log level. */

    #define LOG_ERR_ENABLED     (true)      /*!< WARN Log level with error logging turned on. */
#endif

#ifndef LOG_DEBUG_ENABLED
    #define LOG_DEBUG_ENABLED   (false)     /*!< DEBUG Log level disabled. */
#endif

#ifndef LOG_INFO_ENABLED
    #define LOG_INFO_ENABLED    (false)     /*!< INFO Log level disabled. */
#endif

#ifndef LOG_WARN_ENABLED
    #define LOG_WARN_ENABLED    (false)     /*!< WARN Log level disabled. */
#endif

#ifndef LOG_ERR_ENABLED
    #define LOG_ERR_ENABLED    (false)      /*!< ERROR Log level disabled. */
#endif


#if (true == LOG_ERR_ENABLED)
    #define LOGE(...)                   logPrintf(LOG_MODULE_DEFAULT, LOG_LEVEL_ERROR, __VA_ARGS__)   /*!< ERROR log output when enabled. */
    #define LOGE_DUMP(address, length)  logDump(LOG_MODULE_NCI, LOG_LEVEL_ERROR, address, length) /*!< ERROR dump log output when enabled. */
#else
    #define LOGE(...)                   /*!< ERROR log output when disabled. */
    #define LOGE_DUMP(address, length)  /*!< ERROR dump log output when disabled. */
#endif

#if (true == LOG_WARN_ENABLED)
    #define LOGW(...)                   logPrintf(LOG_MODULE_DEFAULT, LOG_LEVEL_WARN, __VA_ARGS__)    /*!< WARN log output when enabled. */
    #define LOGW_DUMP(address, length)  logDump(LOG_MODULE_NCI, LOG_LEVEL_WARN, address, length)  /*!< WARN dump log output when enabled. */
#else
    #define LOGW(...)                   /*!< WARN log output when disabled. */
    #define LOGW_DUMP(address, length)  /*!< WARN dump log output when disabled. */
#endif

#if (true == LOG_INFO_ENABLED)
    #define LOGI(...)                   logPrintf(LOG_MODULE_DEFAULT, LOG_LEVEL_INFO, __VA_ARGS__)    /*!< INFO log output when enabled. */
    #define LOGI_DUMP(address, length)  logDump(LOG_MODULE_NCI, LOG_LEVEL_INFO, address, length)  /*!< INFO dump log output when enabled. */
#else
    #define LOGI(...)                   /*!< INFO log output when disabled. */
    #define LOGI_DUMP(address, length)  /*!< INFO dump log output when disabled. */
#endif

#if (true == LOG_DEBUG_ENABLED)
    #define LOGD(...)                   logPrintf(LOG_MODULE_DEFAULT, LOG_LEVEL_DEBUG, __VA_ARGS__)   /*!< DEBUG log output when enabled. */
    #define LOGD_DUMP(address, length)  logDump(LOG_MODULE_NCI, LOG_LEVEL_DEBUG, address, length) /*!< DEBUG dump log output when enabled. */
#else
    #define LOGD(...)                   /*!< DEBUG log output when disabled. */
    #define LOGD_DUMP(address, length)  /*!< DEBUG dump log output when disabled. */
#endif

/*
******************************************************************************
* GLOBAL DATA TYPES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*!
 *****************************************************************************
 * \brief Initialize the logging module.
 *
 * \note Sets the log level of all modules to LOG_NONE.
 *
 * \param[in] logOutput Callback to handle output of logging strings, if NULL is 
 *                  passed will use default log output
 *****************************************************************************
 */
void logInit(void (*logOutput)(const char *string));

/*!
 *****************************************************************************
 * \brief Set the logging level of a module.
 *
 * \param[in] module : The module whose logging level must be set.
 * \param[in] level  : The new logging level for this module.
 *****************************************************************************
 */
void logSetLevel(uint8_t module, uint8_t level);

/*!
 *****************************************************************************
 * \brief Check whether log level \a level is enabled for module \a module.
 * 
 * \param[in] module : the module to check the log 
 * \param[in] level  : the logging level for this module
 *
 * \return != 0: Log level \a level is enabled for module \a module.
 * \return == 0: Log level \a level is disabled for module \a module.
 *****************************************************************************
 */
bool logLoggingIsEnabled(uint8_t module, uint8_t level);

/*!
 *****************************************************************************
 * \brief Log a messge.
 *
 * The log message will be output if the logging level set for module \a module
 * is higher than the level \a level of the log message.
 *
 * \param[in] module : The module whose logging level must be set.
 * \param[in] level  : The new logging level for this module.
 * \param[in] format : Printf style format string of the log message
 * \param[in] ... Additional parameters according to the format string.
 *****************************************************************************
 */
void logPrintf(uint8_t module, uint8_t level, const char *format, ...) LOG_PRINTF_FORMAT_CHECK;

/*!
 *****************************************************************************
 * \brief Log a test messge.
 *
 * This function is only to be used for tests. It unconditionally prints the given message
 * to the stream defined in logInit().
 *
 * \param [in] format : Printf style format string of the log message
 *****************************************************************************
 */
void testPrintf(const char *format, ...) TEST_PRINTF_FORMAT_CHECK;

/*!
 *****************************************************************************
 * \brief Dump memory as hex bytes.
 *
 * \param[in] module : The module from which to log a message.
 * \param[in] level  : The log level of the message to be logged.
 * \param[in] address: Start address of the memory to dump.
 * \param[in] length : Number of bytes to dump.
 *****************************************************************************
 */
void logDump(uint8_t module, uint8_t level, const void *address, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
