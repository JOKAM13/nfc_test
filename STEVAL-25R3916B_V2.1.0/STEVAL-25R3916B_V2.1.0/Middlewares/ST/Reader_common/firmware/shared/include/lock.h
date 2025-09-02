
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
 *  \brief Interface for a gloabl interrupt lock
 * 
 */

#ifndef LOCK_H
#define LOCK_H

/*
******************************************************************************
* Includes
******************************************************************************
*/
#include <stdint.h>
#include "rfal_platform.h"
#include "st_errno.h"

/*
******************************************************************************
* Functions
******************************************************************************
*/
/*!
*****************************************************************************
* \brief Locking
*
* Can be called in the interrupt handler context, and within the
* supperloop context. Disables all interupts with configurable
* priority.
*****************************************************************************
*/
void lockGlobalLock ( void );

/*!
*****************************************************************************
* \brief Unlocking
*
* Can be called the interrupt handler context, and within the
* supperloop context. If it has been called as many times as
* lockGlobalLock it Enables all interrupts with configurable priority.
*
* \return Number of remaining calls necessary to reenable interrupts.
*****************************************************************************
*/
int32_t lockGlobalUnLock ( void );

/*!
*****************************************************************************
* \brief Read Semaphore value
*
* Can be called within and without interrupts. Returns the value of
* the internal semaphore. If the value is greater than zero the
* interrupts are disabled.
*
* If the value of the semaphore is zero the interrupts may or may not
* be enabled.
*
* \return Number of remaining calls necessary to reenable interrupts.
*****************************************************************************
*/
int32_t lockGlobalStatus ( void );


/*!
*****************************************************************************
* \brief Read Interrupt status
*
* Can be called within and without interrupts. Returns the value of
* the PRIMASK. 
*
* \return If the value is  zero the interrupts are enabled. If the
* value of the semaphore is not zero the interrupts are disabled.
*****************************************************************************
*/
static inline int32_t lockInterruptStatus ( void ) 
{
    return (__get_PRIMASK()&1);
}


/*!
*****************************************************************************
* \brief Initializes the lock
*
* Initializes the internal semaphore and exits with one lock hold
* to enable the interrupts call lockGlobalUnLock
*****************************************************************************
*/
void lockGlobalLockInitialize ( void );

#endif
