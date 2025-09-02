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
 *  \brief Streaming driver interface declarations.
 *  The defines allow switching between different stream drivers,
 *  currently implemented are:
 *  - USB   
 *  - UART  
 *
 */

/*!
 *
 *
 */
#ifndef STREAM_DRIVER_H
#define STREAM_DRIVER_H
/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdint.h>
#include "st_stream.h"  /* stream protocol definitions */

#include "usb_hid_stream_driver.h"
#include "usbd_custom_hid_if.h"


/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

/* redirect according to underlying communication protocol being usb-hid, uart */
#if USE_UART_STREAM_DRIVER

#define StreamInitialize       uartStreamInitialize
#define StreamConnect          uartStreamConnect
#define StreamDisconnect       uartStreamDisconnect
#define StreamReady            uartStreamReady
#define StreamHasAnotherPacket uartStreamHasAnotherPacket
#define StreamPacketProcessed  uartStreamPacketProcessed
#define StreamReceive          uartStreamReceive
#define StreamTransmit         uartStreamTransmit


#else /* USE_USB_STREAM_DRIVER */

#define StreamInitialize       usbStreamInitialize
#define StreamConnect          usbStreamConnect
#define StreamDisconnect       usbStreamDisconnect
#define StreamReady            usbStreamReady
#define StreamHasAnotherPacket usbStreamHasAnotherPacket
#define StreamPacketProcessed  usbStreamPacketProcessed
#define StreamReceive          usbStreamReceive
#define StreamTransmit         usbStreamTransmit

#endif



#endif /* STREAM_DRIVER_H */

