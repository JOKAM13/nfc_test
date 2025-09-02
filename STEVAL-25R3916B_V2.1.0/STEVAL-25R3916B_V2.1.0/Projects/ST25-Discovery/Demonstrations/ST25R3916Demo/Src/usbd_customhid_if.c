/**
  ******************************************************************************
  * @file    usbd_customhid_if.c
  * @author  MCD Application Team
  * @brief   USB Device Custom HID interface file.
  *        This template should be copied to the user folder, renamed and customized
  *          following user needs.
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

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid_if.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static int8_t FS_CUSTOM_HID_Init(void);
static int8_t FS_CUSTOM_HID_DeInit(void);
static int8_t FS_CUSTOM_HID_OutEvent(uint8_t event_idx, uint8_t state);

#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
static int8_t FS_CUSTOM_HID_CtrlReqComplete(uint8_t request, uint16_t wLength);
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */

#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
static uint8_t *FS_CUSTOM_HID_GetReport(uint16_t *ReportLength);
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */
/* Private variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef USBD_Device;

__ALIGN_BEGIN static uint8_t FS_CUSTOM_HID_ReportDesc[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */ 
  0x06, 0x00, 0xFF,                   /* USAGE_PAGE (Vendor Defined Page 1) */
  0x09, 0x01,                         /* USAGE (Vendor Usage 1) */

  0xa1, 0x01,                         /* COLLECTION (Application) */
  0x19, 0x01,                         /*   USAGE_MINIMUM (0) */
  0x29, 0x40,                         /*   USAGE_MAXIMUM (64) */
  0x15, 0x00,                         /*   LOGICAL_MINIMUM (0) */
  0x26, 0xFF, 0x00,                   /*   LOGICAL_MAXIMUM (255) */
  0x75, 0x08,                         /*   REPORT_SIZE (8) */
  0x95, 0x40, 						            /*   REPORT_COUNT (64) */
  0x81, 0x00,                         /* */
  0x19, 0x01,                         /*   USAGE MINIMUM */
  0x29, 0x40,						              /*   USAGE_MAXIMUM */
  0x91, 0x00,                         /*   OUTPUT (Data,Var,Abs) (note: output from host)  */
  /* USER CODE END 0 */ 
  0xC0    /*     END_COLLECTION	             */
   
}; 

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  FS_CUSTOM_HID_ReportDesc,
  FS_CUSTOM_HID_Init,
  FS_CUSTOM_HID_DeInit,
  FS_CUSTOM_HID_OutEvent,
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
  FS_CUSTOM_HID_CtrlReqComplete,
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
  FS_CUSTOM_HID_GetReport,
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  FS_CUSTOM_HID_Init
  *         Initializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t FS_CUSTOM_HID_Init(void)
{
  return (0);
}

/**
  * @brief  FS_CUSTOM_HID_DeInit
  *         DeInitializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t FS_CUSTOM_HID_DeInit(void)
{
  /*
     Add your deinitialization code here
  */
  return (0);
}


/**
  * @brief  FS_CUSTOM_HID_Control
  *         Manage the CUSTOM HID class events
  * @param  event_idx: event index
  * @param  state: event state
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t FS_CUSTOM_HID_OutEvent(uint8_t event_idx, uint8_t state)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)USBD_Device.pClassData;
  UNUSED(event_idx);
  UNUSED(state);

  /* Start next USB packet transfer once data processing is completed */
//  if (USBD_CUSTOM_HID_ReceivePacket(&USBD_Device) != (uint8_t)USBD_OK)
//  {
//    return -1;
//  }
  hhid->IsReportAvailable = 1;

  return (0);
}

uint8_t UsbTransmitReady()
{
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)USBD_Device.pClassData;
  return (hhid->state == CUSTOM_HID_IDLE) ? 1 : 0;
}

uint8_t UsbTransmit(uint8_t *data, uint16_t dataLen)
{
  return USBD_CUSTOM_HID_SendReport(&USBD_Device, data, dataLen);
}

uint8_t UsbReceive(uint8_t *data, uint16_t *dataLen)
{
    uint8_t status = USBD_FAIL;
    USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)USBD_Device.pClassData;
    *dataLen = 0;
  
    if(hhid->IsReportAvailable == 0) return USBD_FAIL;
  
    hhid->IsReportAvailable = 0;

    for(uint8_t i = 0; i < USBD_CUSTOMHID_OUTREPORT_BUF_SIZE; i++)
    {
        data[i] = hhid->Report_buf[i];
    }
    *dataLen = USBD_CUSTOMHID_OUTREPORT_BUF_SIZE;
    USBD_CUSTOM_HID_ReceivePacket(&USBD_Device);
    status = USBD_OK;

    return status;
}

#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
/**
  * @brief  FS_CUSTOM_HID_CtrlReqComplete
  *         Manage the CUSTOM HID control request complete
  * @param  request: control request
  * @param  wLength: request wLength
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t FS_CUSTOM_HID_CtrlReqComplete(uint8_t request, uint16_t wLength)
{
  UNUSED(wLength);

  switch (request)
  {
    case CUSTOM_HID_REQ_SET_REPORT:

      break;

    case CUSTOM_HID_REQ_GET_REPORT:

      break;

    default:
      break;
  }

  return (0);
}
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */


#ifdef USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED
/**
  * @brief  FS_CUSTOM_HID_GetReport
  *         Manage the CUSTOM HID control Get Report request
  * @param  event_idx: event index
  * @param  state: event state
  * @retval return pointer to HID report
  */
static uint8_t *FS_CUSTOM_HID_GetReport(uint16_t *ReportLength)
{
  UNUSED(ReportLength);
  uint8_t *pbuff;

  return (pbuff);
}
#endif /* USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */
