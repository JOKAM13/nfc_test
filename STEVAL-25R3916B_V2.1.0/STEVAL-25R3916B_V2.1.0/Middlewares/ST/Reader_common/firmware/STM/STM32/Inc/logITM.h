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
 *  \brief Serial log with ITM 
 *
 */

/*!
 *
 * This driver provides a printf-like way to output log messages
 * via the ITM interface. It makes use SWD.
 *
 */

#ifndef LOGITM_H
#define LOGITM_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <string.h>
#include "main.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/
#define LOGITM_ON   1
#define LOGITM_OFF  0

/*!
 *****************************************************************************
 *  \brief  Writes out a formated string via ITM interface
 *
 *  This function is used to write a formated string via the ITM interface.
 *
 *****************************************************************************
 */
extern int logITM(const char* format, ...);

#endif /* LOGGER_H */

