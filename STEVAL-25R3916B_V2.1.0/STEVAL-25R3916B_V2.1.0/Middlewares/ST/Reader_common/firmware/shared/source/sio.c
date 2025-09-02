
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

/*! \file sio.c
 *
 *  \author Programmer Franz Haas
 *  \author gpat
 *  
 *
 *  \brief  This module implements the middle layer between software IO 
 *  and the hardware UART 
 *
 *  The required stubs for Serial IO are implemented. 
 */


#ifdef STM32L476xx
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#else
  #undef _EWL_CONSOLE_SUPPORT
  #define _EWL_CONSOLE_SUPPORT 1            /*!< Freescale's Embedded Warrior Library Console Support switch */
#endif /* STM32L476xx */


/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "uart_driver.h"
#include "utils.h"
#include "lock.h"
#include "st_errno.h"
#include "sio.h"



/*
******************************************************************************
* DEFINES
******************************************************************************
*/

#define SIO_SYS_CLOCK                 (20972000)     /*!< System Clock                     */
#define SIO_UART_ID                   (1)            /*!< UART ID to be used by sio        */
#define SIO_TX_MAXRETRYS              (0xFFFF)       /*!< Num times to check for Tx buffer */
#define SIO_TX_BUFLEN                 (511)          /*!< Size of Tx buffer                */
#define SIO_RX_BUFLEN                 (255)          /*!< Size of Rx buffer                */

/*
******************************************************************************
* GLOBAL VARIABLE DECLARATIONS
******************************************************************************
*/

static unsigned char sioIEerrorMsg[] = "!! LOG IE !!\r\n";

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/


#ifdef STM32L476xx
/*!
*****************************************************************************
* \brief fputc implementation
*
* Reads the available bytes from the serial input
*
*****************************************************************************
*/
PUTCHAR_PROTOTYPE
{
  WriteUARTN( (uint8_t *)&ch, 1);
  return ch;
}
#endif


/*!
*****************************************************************************
* \brief Read UART
*
* Reads the available bytes from the serial input
*
* \param[out] bytes : buffer where to write the input bytes
* \param[in] length : length of the provided buffer 
*
* \return ERR_NONE  : No error
* \return ERR_IO    : Error while retrieving input
* \return ERR_NOMSG : Nothing available to read
*****************************************************************************
*/
int ReadUARTN(void* bytes, unsigned long length )
{  
  if( uartRxBytesReadyForReceive(SIO_UART_ID) > 0 ) 
  {
    if( uartRxNBytes(SIO_UART_ID, bytes, length) > 0)
      return ERR_NONE;
    return ERR_INSERT_SIO_GRP(ERR_IO);
  }
  return ERR_INSERT_SIO_GRP(ERR_NOMSG);  
}

/*!
*****************************************************************************
* \brief Write UART
*
* Writes the given bytes on the serial output
*
* \param[in] bytes : buffer to be written on output
* \param[in] length : length of the provided buffer 
*
* \return ERR_NONE  : No error
* \return ERR_NOMEM : Tx buffer is full
*****************************************************************************
*/
int WriteUARTN(const void* bytes, unsigned long length)
{    
    
    unsigned long  txDone;        
    unsigned long  txAvailable;
    unsigned char* ptr;
    
    ptr     = (unsigned char*) bytes;                                                                   // avoid void pointer arithmetics
    txDone  = 0; 
        
    if( lockInterruptStatus() != 0 )                                                                    // if interrupt are disabled 
    {
        uartTxNBytes(SIO_UART_ID, sioIEerrorMsg, SIZEOF_ARRAY(sioIEerrorMsg) );                         // error message
    }
    else
    {
        while( (txDone < length) )                                                                      // try outputting all out within retrys
        {
            txAvailable = uartMaxTxBytes(SIO_UART_ID);                                                  // check if output buffer is available
            if(txAvailable > 0)
            {
                txAvailable = uartTxNBytes(SIO_UART_ID, ptr, MIN( txAvailable, (length - txDone) ) );   // send available bytes
                ptr    += txAvailable;                                                                  // point forward on the buffer
                txDone += txAvailable;                                                                  // update bytes already done
            }
        }
    }
          
   return ERR_NONE;
}


/*!
*****************************************************************************
* \brief Initialise UART
*
* Initialises the SIO peripheral (UART) 
*
* \param[in] baudrate : the baudrate to be used  
*
* \return ERROR_NONE : No error
* \return ERR_IO     : Error while initialising peripheral
*****************************************************************************
*/
int InitializeUART( int baudrate )
{
  stError res;
  uint32_t realBaudRate;  
  
  res = uartCreate(SIO_UART_ID, SIO_RX_BUFLEN, SIO_TX_BUFLEN);
  if(res != ERR_NONE)
    return ERR_INSERT_SIO_GRP(ERR_IO);
  
  res = uartInitialize(SIO_UART_ID, SIO_SYS_CLOCK, baudrate, &realBaudRate);
  if(res != ERR_NONE)
    return ERR_INSERT_SIO_GRP(ERR_IO);    
    
  return ERR_NONE; 
}

stError sioWriteIsCompleted (void)
{
    if (SIO_TX_BUFLEN == uartMaxTxBytes(SIO_UART_ID))
    {
        return ERR_NONE;
    }
    
    return ERR_INSERT_SIO_GRP(ERR_BUSY);
}
