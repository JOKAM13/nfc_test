/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2016 STMicroelectronics, all rights reserved
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
 *      PROJECT:   
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author 
 *
 *  \brief Debug log output via ITM
 *
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "logITM.h"

/*
******************************************************************************
* LOCAL DEFINES
******************************************************************************
*/
#define USE_LOGITM      LOGITM_ON

void logITMTx(uint8_t *data, uint16_t dataLen)
{
    #if (USE_LOGITM == LOGITM_ON)
    #if defined (STM32L4) || defined (STM32F4) || defined (STM32F7) || defined(STM32H7) || defined(STM32U5)
    
    while (dataLen != 0)
    {
        ITM_SendChar(*data);
        data++;
        dataLen--;
    }
    #endif /* STM32L4 || STM32F4 || STM32F7 */    
    #endif /* #if USE_LOGITM == LOGITM_ON */
    return;
}

int logITM(const char* format, ...)
{
  #if (USE_LOGITM == LOGITM_ON)
  {
    #define LOG_BUFFER_SIZE 256
    char buf[LOG_BUFFER_SIZE];
    va_list argptr;
    va_start(argptr, format);
    int cnt = vsnprintf(buf, LOG_BUFFER_SIZE, format, argptr);
    va_end(argptr);
      
    /* */
    logITMTx((uint8_t*)buf, strlen(buf));
    HAL_Delay((cnt + 9)/10); /* WA to avoid ITM overflow */
    //HAL_Delay(cnt); /* WA to avoid ITM overflow */
    return cnt;
  }
  #else
  {
    return 0;
  }
  #endif /* #if USE_LOGITM == LOGITM_ON */
}
