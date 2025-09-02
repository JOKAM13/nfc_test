/******************************************************************************
  * @attention
  *
  * COPYRIGHT 2024 STMicroelectronics, all rights reserved
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

/*! \file sysNfc.h
 *
 *  \author Gustavo Patricio 
 *
 *  \brief ST25R Demo abstraction for NFC system initialization
 *  
 *  Provides a common abstraction|example for ST25R demos how to initialize 
 *  a system using RFAL and persistent configuration
 */

#ifndef SYSNFC_H_
#define SYSNFC_H_


/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "rfal_utils.h"
#include "rfal_dpo.h"


/*
******************************************************************************
* GLOBAL TYPES
******************************************************************************
*/


/*! System DPO Adjustment configuration */
typedef enum sysNfcDpoAdjustMethod
{
    SYS_NFC_DPO_ADJUST_NONE = 0,   /*!< DPO Adjustment: Not set               */
    SYS_NFC_DPO_ADJUST_RFO,        /*!< DPO Adjustment: RFO Resistance        */
    SYS_NFC_DPO_ADJUST_REGULATOR,  /*!< DPO Adjustment: Regulator drop        */
} sysNfcDpoAdjustMethod;


/*! System DPO Measurement configuration */
typedef enum sysNfcDpoMeasureMethod
{
    SYS_NFC_DPO_MEASURE_NONE = 0,       /*!< DPO Measurement: Not set         */
    SYS_NFC_DPO_MEASURE_AMPLITUDE,      /*!< DPO Measurement: Amplitude       */
    SYS_NFC_DPO_MEASURE_PHASE,          /*!< DPO Measurement: Phase           */
    SYS_NFC_DPO_MEASURE_ATAN2,          /*!< DPO Measurement: Atan2           */
    SYS_NFC_DPO_MEASURE_CAPACITANCE,    /*!< DPO Measurement: Capacitance     */
    SYS_NFC_DPO_MEASURE_COMBINED_IQ,    /*!< DPO Measurement: Combined IQ     */
    SYS_NFC_DPO_MEASURE_I_ONLY,         /*!< DPO Measurement: I Only          */
    SYS_NFC_DPO_MEASURE_Q_ONLY,         /*!< DPO Measurement: Q Only          */
    SYS_NFC_DPO_MEASURE_FALL,           /*!< DPO Measurement: Fall            */
    SYS_NFC_DPO_MEASURE_RISE,           /*!< DPO Measurement: Rise            */
} sysNfcDpoMeasureMethod;


/*! System DPO information */
typedef struct sysNfcDpoInfo
{
    bool                   enabled;           /*!< enabled state                   */
    rfalDpoEntry           table[RFAL_DPO_TABLE_MAX_ENTRIES];/*!< table of entries */
    uint8_t                tableEntries;      /*!< number of entries used          */
    uint8_t                tableEntry;        /*!< current entry used              */
    sysNfcDpoAdjustMethod  adjustId;          /*!< adjust method index             */
    sysNfcDpoMeasureMethod measureId;         /*!< measure method index            */
    uint8_t                refMeasure;        /*!< last measure used to adjust     */
} sysNfcDpoInfo;


/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*! 
 *****************************************************************************
 * \brief  Sys NFC Initialization
 *
 * 
 * \return  RFAL_ERR_IO          : Internal error
 * \return  RFAL_ERR_NOTSUPP     : Feature not supported
 * \return  RFAL_ERR_HW_MISMATCH : Expected HW do not match or communication error
 * \return  RFAL_ERR_NONE        : No error
 *****************************************************************************
 */
ReturnCode sysNfcInitialize( void );


/*! 
 *****************************************************************************
 * \brief  Sys NFC Reinitialization
 *
 *
 * \return  RFAL_ERR_IO          : Internal error
 * \return  RFAL_ERR_NOTSUPP     : Feature not supported
 * \return  RFAL_ERR_HW_MISMATCH : Expected HW do not match or communication error
 * \return  RFAL_ERR_NONE        : No error
 *****************************************************************************
 */
ReturnCode sysNfcReset( void );


/*!
 *****************************************************************************
 * \brief  Configure the System NFC DPO Adjust method
 *
 *  DPO Adjustment method is set by default during initialization.
 *  This method allows to configure these differently.
 *
 * \warning This method shall be used after sysNfcReset | sysNfcInitialize.
 *          Reset|Initialization methods restore default configuration.
 *
 * \return  RFAL_ERR_DISABLED : Feature disabled
 * \return  RFAL_ERR_PARAM    : Invalid parameter | configuration
 * \return  RFAL_ERR_NONE if successful or any standard error code
 *****************************************************************************
 */
ReturnCode sysNfcDpoSetAdjustMethod( sysNfcDpoAdjustMethod adjustId );


/*!
 *****************************************************************************
 * \brief  Configure the System NFC DPO Measure method
 *
 *  DPO Measurement method is set by default during initialization.
 *  This method allows to configure these differently.
 *
 * \warning This method shall be used after sysNfcReset | sysNfcInitialize.
 *          Reset|Initialization methods restore default configuration. 
 *
 * \return  RFAL_ERR_DISABLED : Feature disabled
 * \return  RFAL_ERR_PARAM    : Invalid parameter | configuration
 * \return  RFAL_ERR_NONE if successful or any standard error code
 *****************************************************************************
 */
ReturnCode sysNfcDpoSetMeasureMethod( sysNfcDpoMeasureMethod measureId );


/*!
 *****************************************************************************
 * \brief  Convert the RFAL DPO structure into System level DPO structure
 *
 * It essentially maps function pointers to matching enumerated values.
 *
 * \return  RFAL_ERR_PARAM : Invalid parameter
 * \return  RFAL_ERR_NONE if successful or any standard error code
 *****************************************************************************
 */
ReturnCode sysNfcDpoGetInfo( sysNfcDpoInfo* sysDpoInfo );

#endif /* SYSNFC_H_ */
