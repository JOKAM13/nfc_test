
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
 *      PROJECT:   $
 *      LANGUAGE:  ISO C99
 */

/*! \file sio.h
 *
 *  \author Gustavo Patricio
 *  
 *
 *  \brief  This module implements the middle layer between software IO 
 *  and the hardware UART 
 *
 *  The required stubs for Serial IO are implemented. 
 */
#ifndef SIO_H_
#define SIO_H_

#include "st_errno.h"


int WriteUARTN(const void* bytes, unsigned long length);
int ReadUARTN(void* bytes, unsigned long length );
int InitializeUART( int baudrate );

/*!
 *****************************************************************************
 * \brief Check if logging transmission is completed and transport driver buffer is empty
 *
 * Implementation:
 * - Check if the buffer for the respective transport driver in use is fully available, 
 *   which indicates that no data is pending to be transmitted
 *
 * \return ERR_NONE if all log data has been transmitted
 * \return ERR_BUSY if some log data is still in buffer to be transmitted
 *****************************************************************************
 */
stError sioWriteIsCompleted (void);

#endif /* SIO_H_ */
