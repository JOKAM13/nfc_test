/**
  ******************************************************************************
  * @file           : pac.h (Persistent Config)
  * @version        : v2.0_Cube
  * @brief          : This file implements the Persistent Analog Config management on flash
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/
#ifndef _PERSISTENT_CONFIG_H_
#define _PERSISTENT_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "st_errno.h"
#include "pc_id.h"

/*
******************************************************************************
* GLOBAL TYPES
******************************************************************************
*/

/* Public accessable configuration ID */
typedef enum pcID
{
    PC_ID_AC   = PERSISTENT_CONFIG_AC,  
    PC_ID_DPO  = PERSISTENT_CONFIG_DPO, 
    PC_ID_DLMA = PERSISTENT_CONFIG_DLMA
} pcID;

/* Internal Tag IDs appended behind public config IDs */
typedef enum pcTAG
{
    /* Sub IDs */
    PC_TAG_IS_DPO_ENABLED       = (NUM_PERSISTENT_CONFIG + 0),   /*!< Configuration TAG: IsDpoEnabled   */
    PC_TAG_IS_DLMA_ENABLED      = (NUM_PERSISTENT_CONFIG + 1),   /*!< Configuration TAG: IsDlmaEnabled  */
    PC_TAG_DPO_RAW_DATA         = (NUM_PERSISTENT_CONFIG + 2),   /*!< Configuration TAG: Raw DPO data   */
    PC_TAG_DLMA_RAW_DATA        = (NUM_PERSISTENT_CONFIG + 3),   /*!< Configuration TAG: Raw DLMA data  */
}pcTAG;

/*!
 *****************************************************************************
 * \brief Enable PC
 *
 * If no configuration is stored in flash (factory default) the 
 * default configuration from the modules is programmed to flash.
 * Otherwise the configuration stored in flash is applied to the modules.
 *
 * \return isInstalled
 *****************************************************************************
 */
bool    persistentConfig_Enable  (void);

/*!
 *****************************************************************************
 * \brief Program PC
 *
 * Program requested configurations.
 * 
 * \param pcID : Tag ID to program
 *
 * Return with one of the following Status Codes
 * - #ERR_NONE : No Error
 * - #ERR_IO : failed to program
 * 
 * \return stError status code
 *****************************************************************************
 */
stError persistentConfig_Program (pcID id);

/*!
 *****************************************************************************
 * \brief Get Hashes
 *
 * Calculate hash value for configurations
 * 
 * \param pcID : Retriev Hash Tag ID
 * \param hashOriginal : Hash value of original/inital configuration
 * \param hashFlash : Hash value of configuration stored in flash
 * \param hashRam : Hash value of configuration stored in Ram 
 *
 *****************************************************************************
 */
stError persistentConfig_GetHash(pcID id, uint32_t* hashOriginal, uint32_t* hashFlash, uint32_t* hashRam);

/*!
 *****************************************************************************
 * \brief Calculate Hash
 *
 * Calculate hash value over given data array.
 * 
 * Return result of hash value calculation
 * 
 * \param buf : data buffer as calculation input
 * \param bufLen : length of data buffer to use for calculation.
 *
 * \return Hash value
 *****************************************************************************
 */
 uint32_t persistentConfig_CalcHash(const uint8_t *buf, uint16_t bufLen);

/*!
 *****************************************************************************
 * \brief Request raw configuration data
 *
 * Request raw configuration data
 * 
 * Return with one of the following Status Codes
 * - #ERR_NONE : No Error
 * - #ERR_PARAM : Null pointer received or not enough memory available to store reply
 * 
 * \param tag : tag of requested configuration. 
 * \param data : memory to store requested configuration.
 * \param dataSize : size of data array allocated by caller.
 * \param dataLength : size of stored configuration in data array.
 *
 * \return stError status code
 *****************************************************************************
 */
stError persistentConfig_Get(const pcTAG tag, uint8_t* data, const uint16_t dataSize, uint16_t* dataLength);


#endif /* _PERSISTENT_ANALOG_CONFIG_H_ */
