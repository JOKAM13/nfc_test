
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
 *  \author F. Haas
 *
 *  \brief Implementation of a lock.
 *
 *
 */

/*
******************************************************************************
* Includes
******************************************************************************
*/
#include "lock.h"

/*
******************************************************************************
* Variables
******************************************************************************
*/

/** global lock variable */
volatile int32_t lockLockCount = 1;

/*
******************************************************************************
* Global Functions
******************************************************************************
*/

void lockGlobalLockInitialize ( void ) {
    lockLockCount = 1;
}
	
int32_t lockGlobalUnLock ( void ) 
{
    int32_t temp; 
    __disable_irq();
    //Unconditionally disable interrupts. If everything
    //is ok this is a nop operation. If something goes
    //wrong it increases confidence in the
    //lockLockCount.

    temp = lockLockCount;
    temp--;
    if ( temp < 0 )
    {
        temp = 0;
    }
    lockLockCount=temp;
    if ( 0 == temp ) 
    {
        __enable_irq();
    }
    return temp;
}

void lockGlobalLock( void ){
    int32_t temp; 
    __disable_irq();
    temp = lockLockCount;
    temp++;
    lockLockCount = temp;
}

int32_t lockGlobalStatus ( void )
{
    //As long as lockLockCount is transfered atomically no lock is necessary
    return lockLockCount;
}
