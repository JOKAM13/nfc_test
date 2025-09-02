/**
  ******************************************************************************
  * @file    st25_discovery_nfctag.h
  * @author  MMY Application Team
  * @brief   This file contains definitions for the st25_discovery_nfctag.c
  *          specific functions.
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2021 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ST25_DISCOVERY_NFCTAG_H
#define ST25_DISCOVERY_NFCTAG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "st25_discovery.h"
#include "st25_discovery_errno.h"


/** @addtogroup BSP
  * @{
  */
   
/** @addtogroup ST25_DISCOVERY_NFCTAG
  * @{
  */
/* Exported types ------------------------------------------------------------*/
/**
 * @brief  NFCTAG standard driver API structure definition.
 */
typedef struct
{
  int32_t       (*Init)(const void *const);
  int32_t       (*ReadID)(const void *const, uint8_t *const);
  int32_t       (*IsReady)(const void *const, const uint32_t);
  int32_t       (*GetITStatus)(const void *const, uint16_t *const);
  int32_t       (*ConfigIT)(const void *const, const uint16_t);
  int32_t       (*ReadData)(const void *const, uint8_t *const, const uint16_t, const uint16_t);
  int32_t       (*WriteData)(const void *const, const uint8_t *const, const uint16_t, const uint16_t);
} NFCTAG_DrvTypeDef;
/* Exported constants --------------------------------------------------------*/
/** @brief Number of Bytes for the NFCTAG 4Kbits */
#define NFCTAG_4K_SIZE            ((uint32_t) 0x200)
/** @brief Number of Bytes for the NFCTAG 16Kbits */
#define NFCTAG_16K_SIZE           ((uint32_t) 0x800)
/** @brief Number of Bytes for the NFCTAG 64Kbits */
#define NFCTAG_64K_SIZE           ((uint32_t) 0x2000)

/**
 * @brief  NFCTAG status enumerator definition.
 */
#define NFCTAG_OK      (0)
#define NFCTAG_ERROR   (-1)
#define NFCTAG_BUSY    (-2)
#define NFCTAG_TIMEOUT (-3)
#define NFCTAG_NACK    (-102)

/**
  * @brief NFCTAG INSTANCE
  */
#define BSP_NFCTAG_INSTANCE 0

/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function	--------------------------------------------------------*/
int32_t BSP_NFCTAG_Init(const uint32_t Instance);
void BSP_NFCTAG_DeInit(const uint32_t Instance);
int32_t BSP_NFCTAG_isInitialized(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadID(const uint32_t Instance, uint8_t *const wai_id);
int32_t BSP_NFCTAG_ConfigIT(const uint32_t Instance, const uint16_t ITConfig);
int32_t BSP_NFCTAG_GetITStatus(const uint32_t Instance, uint16_t *const ITConfig);
int32_t BSP_NFCTAG_ReadData(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr, const uint16_t Size);
int32_t BSP_NFCTAG_WriteData(const uint32_t Instance, const uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t Size);
int32_t BSP_NFCTAG_ReadRegister(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t Size);
int32_t BSP_NFCTAG_WriteRegister(const uint32_t Instance, const uint8_t *const pData, const uint16_t TarAddr,\
                                                          const uint16_t Size);
int32_t BSP_NFCTAG_IsDeviceReady(const uint32_t Instance,const uint32_t Trials);

uint32_t BSP_NFCTAG_GetByteSize(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadICRev(const uint32_t Instance, uint8_t *const pICRev);
int32_t BSP_NFCTAG_ReadITPulse(const uint32_t Instance, void *const pITtime);
int32_t BSP_NFCTAG_WriteITPulse(const uint32_t Instance, const uint8_t ITtime);
int32_t BSP_NFCTAG_ReadUID(const uint32_t Instance, void *const pUid);
int32_t BSP_NFCTAG_ReadDSFID(const uint32_t Instance, uint8_t *const pDsfid);
int32_t BSP_NFCTAG_ReadDsfidRFProtection(const uint32_t Instance, void *const pLockDsfid);
int32_t BSP_NFCTAG_ReadAFI(const uint32_t Instance, uint8_t *const pAfi);
int32_t BSP_NFCTAG_ReadAfiRFProtection(const uint32_t Instance, void *const pLockAfi);
int32_t BSP_NFCTAG_ReadI2CProtectZone(const uint32_t Instance, void *const pProtZone);
int32_t BSP_NFCTAG_WriteI2CProtectZonex(const uint32_t Instance, const uint8_t Zone, const uint8_t ReadWriteProtection);
int32_t BSP_NFCTAG_ReadLockCCFile(const uint32_t Instance, void *const pLockCCFile);
int32_t BSP_NFCTAG_WriteLockCCFile(const uint32_t Instance, const uint8_t NbBlockCCFile, const uint8_t LockCCFile);
int32_t BSP_NFCTAG_ReadLockCFG(const uint32_t Instance, void *const pLockCfg);
int32_t BSP_NFCTAG_WriteLockCFG(const uint32_t Instance, const uint8_t LockCfg);
int32_t BSP_NFCTAG_PresentI2CPassword(const uint32_t Instance, const void *const PassWord);
int32_t BSP_NFCTAG_WriteI2CPassword(const uint32_t Instance, const void *const PassWord);
int32_t BSP_NFCTAG_ReadRFZxSS(const uint32_t Instance, const uint8_t Zone, void *const pRfprotZone);
int32_t BSP_NFCTAG_WriteRFZxSS(const uint32_t Instance, const uint8_t Zone, const void *const RfProtZone);
int32_t BSP_NFCTAG_ReadEndZonex(const uint32_t Instance, const uint8_t EndZone, uint8_t *pEndZ);
int32_t BSP_NFCTAG_WriteEndZonex(const uint32_t Instance, const uint8_t EndZone, const uint8_t EndZ);
int32_t BSP_NFCTAG_InitEndZone(const uint32_t Instance);
int32_t BSP_NFCTAG_CreateUserZone(const uint32_t Instance, uint16_t Zone1Length, uint16_t Zone2Length, \
                                                          uint16_t Zone3Length, uint16_t Zone4Length);
int32_t BSP_NFCTAG_ReadMemSize(const uint32_t Instance, void *const pSizeInfo);
int32_t BSP_NFCTAG_ReadEHMode(const uint32_t Instance, void *const pEH_mode);
int32_t BSP_NFCTAG_WriteEHMode(const uint32_t Instance, const uint8_t EH_mode);
int32_t BSP_NFCTAG_ReadRFMngt(const uint32_t Instance, void *const pRF_Mngt);
int32_t BSP_NFCTAG_WriteRFMngt(const uint32_t Instance, const uint8_t Rfmngt);
int32_t BSP_NFCTAG_GetRFDisable(const uint32_t Instance, void *const pRFDisable);
int32_t BSP_NFCTAG_SetRFDisable(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFDisable(const uint32_t Instance);
int32_t BSP_NFCTAG_GetRFSleep(const uint32_t Instance, void *const pRFSleep);
int32_t BSP_NFCTAG_SetRFSleep(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFSleep(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadMBMode(const uint32_t Instance, void *const pMB_mode);
int32_t BSP_NFCTAG_WriteMBMode(const uint32_t Instance, const uint8_t MB_mode);
int32_t BSP_NFCTAG_ReadMBWDG(const uint32_t Instance, uint8_t *const pWdgDelay);
int32_t BSP_NFCTAG_WriteMBWDG(const uint32_t Instance, const uint8_t WdgDelay);
int32_t BSP_NFCTAG_ReadMailboxData(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t NbByte);
int32_t BSP_NFCTAG_WriteMailboxData(const uint32_t Instance, const uint8_t *const pData, const uint16_t NbByte);
int32_t BSP_NFCTAG_ReadMailboxRegister(const uint32_t Instance, uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t NbByte);
int32_t BSP_NFCTAG_WriteMailboxRegister(const uint32_t Instance, const uint8_t *const pData, const uint16_t TarAddr, \
                                                          const uint16_t NbByte);
int32_t BSP_NFCTAG_ReadI2CSecuritySession_Dyn(const uint32_t Instance, void *const pSession);
int32_t BSP_NFCTAG_ReadITSTStatus_Dyn(const uint32_t Instance, uint8_t *const pITStatus);
int32_t BSP_NFCTAG_ReadGPO_Dyn(const uint32_t Instance, uint8_t *GPOConfig);
int32_t BSP_NFCTAG_GetGPO_en_Dyn(const uint32_t Instance, void *const pGPO_en);
int32_t BSP_NFCTAG_SetGPO_en_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetGPO_en_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadEHCtrl_Dyn(const uint32_t Instance, void *const pEH_CTRL);
int32_t BSP_NFCTAG_GetEHENMode_Dyn(const uint32_t Instance, void *const pEH_Val);
int32_t BSP_NFCTAG_SetEHENMode_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetEHENMode_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_GetEHON_Dyn(const uint32_t Instance, void *const pEHON);
int32_t BSP_NFCTAG_GetRFField_Dyn(const uint32_t Instance, void *const pRF_Field);
int32_t BSP_NFCTAG_GetVCC_Dyn(const uint32_t Instance, void *const pVCC);
int32_t BSP_NFCTAG_ReadRFMngt_Dyn(const uint32_t Instance, void *const pRF_Mngt);
int32_t BSP_NFCTAG_WriteRFMngt_Dyn(const uint32_t Instance, const uint8_t RF_Mngt);
int32_t BSP_NFCTAG_GetRFDisable_Dyn(const uint32_t Instance, void *const pRFDisable);
int32_t BSP_NFCTAG_SetRFDisable_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFDisable_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_GetRFSleep_Dyn(const uint32_t Instance, void *const pRFSleep);
int32_t BSP_NFCTAG_SetRFSleep_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetRFSleep_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadMBCtrl_Dyn(const uint32_t Instance, void *const pCtrlStatus);
int32_t BSP_NFCTAG_GetMBEN_Dyn(const uint32_t Instance, void *const pMBEN);
int32_t BSP_NFCTAG_SetMBEN_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ResetMBEN_Dyn(const uint32_t Instance);
int32_t BSP_NFCTAG_ReadMBLength_Dyn(const uint32_t Instance, uint8_t *const pMBLength);


int32_t NFC_IO_Init(void);
int32_t NFC_IO_DeInit(void);
int32_t NFC_IO_ReadReg16(const uint16_t DevAddr, const uint16_t Reg, uint8_t *const pData, const uint16_t Length);
int32_t NFC_IO_WriteReg16(const uint16_t DevAddr, const uint16_t Reg, const uint8_t *const pData, const uint16_t Length);
int32_t NFC_IO_IsDeviceReady(const uint16_t Addr, const uint32_t Trials);
uint32_t NFC_IO_Tick(void);

/**
  * @}
  */

/**
  * @}
  */ 


#endif /* ST25_DISCOVERY_NFCTAG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
