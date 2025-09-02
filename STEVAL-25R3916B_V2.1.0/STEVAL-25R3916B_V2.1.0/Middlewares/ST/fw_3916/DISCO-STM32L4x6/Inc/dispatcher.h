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
 *      PROJECT:   ST25R3911 firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author Ulrich Herrmann
 *
 *  \brief Application dispatcher
 *
 */
/*!
 * 
 * Moudle used to dispatch the commands received by the PC application.
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! 
 *****************************************************************************
 *  \brief  Handle interrupt requests which are targetted for the dispatcher
 *
 * The GUI may enable interrupts and read out proper results at the time of the
 * interrupt. 
 * This function handles these interrupts and stores the results for a later 
 * time when the GUI polls it.
 */
extern void dispatcherWorker(void);


#endif /* DISPATCHER_H */

