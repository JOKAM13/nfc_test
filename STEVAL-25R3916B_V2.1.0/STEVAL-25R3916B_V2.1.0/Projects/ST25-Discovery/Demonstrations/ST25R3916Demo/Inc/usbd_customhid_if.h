/**
  ******************************************************************************
  * @file    usbd_customhid_if.h
  * @author  MCD Application Team
  * @brief   Header for usbd_customhid_if_template.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USBD_CUSTOMHID_IF_H
#define USBD_CUSTOMHID_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops;

extern uint8_t UsbReceive(uint8_t *data, uint16_t *dataLen);

extern uint8_t UsbTransmitReady(void);
extern uint8_t UsbTransmit(uint8_t *data, uint16_t dataLen);
#ifdef __cplusplus
}
#endif

#endif /* USBD_CUSTOMHID_IF_H */

