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

/*! \file sysNfc.c
 *
 *  \author Gustavo Patricio 
 *
 *  \brief ST25R Demo abstraction for NFC system initialization
 *  
 *  Provides a common abstraction|example for ST25R demos how to initialize 
 *  a system using RFAL and persistent configuration
 */


/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "sysNfc.h"
#include "rfal_rf.h"
#include "rfal_chip.h"
#include "rfal_dpo.h"
#if RFAL_FEATURE_DLMA
#include "rfal_dlma.h"
#endif /* RFAL_FEATURE_DLMA */
#include "pc.h"
#include "rfal_analogConfig.h"


/*
******************************************************************************
* DEFINES
******************************************************************************
*/


/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/


/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
static ReturnCode sysNfcInit( bool keepCurConfig );


/*!
 *****************************************************************************
 * \brief Pre TxRx callback
 *
 * Callback for RFAL preTxRx DPO adjustment
 *
 *****************************************************************************
 */
__weak void sysNfcPreTxRxCallback( void )
{
#if RFAL_FEATURE_DPO
    if( rfalDpoIsEnabled() )
    {
        rfalDpoAdjust();
    }
#endif /* RFAL_FEATURE_DPO */
}


/*!
 *****************************************************************************
 * \brief LM EON callback
 *
 * Callback for RFAL Listen mode LMA adjustment
 *
 *****************************************************************************
 */
__weak void sysNfcLmEonCallback( void )
{
#if RFAL_FEATURE_DLMA
    if( rfalDlmaIsEnabled() )
    {
        rfalDlmaAdjust();
    }
#endif /* RFAL_FEATURE_DLMA */
}


/*******************************************************************************/
ReturnCode sysNfcInitialize( void )
{
    return sysNfcInit( false );
}


/*******************************************************************************/
ReturnCode sysNfcReset( void )
{
    ReturnCode ret;
    
    RFAL_EXIT_ON_ERR( ret, rfalDeinitialize() );
    return sysNfcInit( true );
}


/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
static ReturnCode sysNfcInit( bool keepCurConfig )
{
    ReturnCode ret;
    uint8_t    getEn;
    uint16_t   getLen;

    
    /*******************************************************************************/
    /* Check if AC and optional modules are to be initialized */
    if( (!keepCurConfig) )
    {
    #if RFAL_FEATURE_DPO    
        rfalDpoInitialize();
    #endif /* RFAL_FEATURE_DPO */

    #if (RFAL_FEATURE_DLMA && RFAL_SUPPORT_CE)
        rfalDlmaInitialize();
    #endif /* RFAL_FEATURE_DLMA */
        
        /* Initialize RFAL Analog Configuration table */    
        rfalAnalogConfigInitialize();
        
        /* Initialize ST25R Demo Persistent Configuration */
        persistentConfig_Enable();
    }

    /*******************************************************************************/
    /* Initialize RFAL */
    RFAL_EXIT_ON_ERR( ret, rfalInitialize() );

    
    /*******************************************************************************/
    /* Configure optional features if needed */
#if RFAL_FEATURE_DPO
    
    getEn = (rfalDpoIsEnabled() ? 1 : 0);
    if( (!keepCurConfig) )
    {
        EXIT_ON_ERR( ret, persistentConfig_Get( PC_TAG_IS_DPO_ENABLED, &getEn, sizeof(uint8_t), &getLen ) );
        rfalDpoSetEnabled( (getEn != 0U) );
    }
    
    if( (getEn != 0U) )
    {
        rfalSetPreTxRxCallback( sysNfcPreTxRxCallback );
    }
#endif /* RFAL_FEATURE_DPO */


#if (RFAL_FEATURE_DLMA && RFAL_SUPPORT_CE)
    
    getEn = (rfalDlmaIsEnabled() ? 1 : 0);
    if( (!keepCurConfig) )
    {
        EXIT_ON_ERR( ret, persistentConfig_Get( PC_TAG_IS_DLMA_ENABLED, &getEn, sizeof(uint8_t), &getLen ) );
        rfalDlmaSetEnabled( (getEn != 0U) );
    }
    
    if( (getEn != 0U) )
    {
        rfalSetLmEonCallback( sysNfcLmEonCallback );
    }
#endif /* RFAL_FEATURE_DLMA */

    return ret;
}


/*******************************************************************************/
#if defined(ST25R200)
#include "st25r200_com.h"
#elif defined(ST25R500)
#include "st25r500_com.h"
#endif

ReturnCode sysChipSetRegulator(uint8_t index)
{
#if defined(ST25R200)
    return st25r200WriteRegister(ST25R200_REG_REGULATOR, index);
#elif defined(ST25R500)
    return st25r500WriteRegister(ST25R500_REG_REGULATOR, index | ST25R500_REG_REGULATOR_reg_s);
#else
    return RFAL_ERR_NOT_IMPLEMENTED;
#endif
}


/*******************************************************************************/
static rfalDpoAdjustFunc adjustEntry[] =
{
    NULL,                            /* SYS_NFC_DPO_ADJUST_NONE      */
    rfalChipSetRFO,                  /* SYS_NFC_DPO_ADJUST_RFO       */
    sysChipSetRegulator              /* SYS_NFC_DPO_ADJUST_REGULATOR */
};


/*******************************************************************************/
static rfalDpoMeasureFunc measureEntry[] =
{
    NULL,                           /* SYS_NFC_DPO_MEASURE_NONE         */
    rfalChipMeasureAmplitude,       /* SYS_NFC_DPO_MEASURE_AMPLITUDE    */
    rfalChipMeasurePhase,           /* SYS_NFC_DPO_MEASURE_PHASE        */
    NULL,                           /* SYS_NFC_DPO_MEASURE_ATAN2,       */
    rfalChipMeasureCapacitance,     /* SYS_NFC_DPO_MEASURE_CAPACITANCE  */
    rfalChipMeasureCombinedIQ,      /* SYS_NFC_DPO_MEASURE_COMBINED_IQ  */
    NULL,                           /* SYS_NFC_DPO_MEASURE_I_ONLY,      */
    NULL                            /* SYS_NFC_DPO_MEASURE_Q_ONLY,      */
};


/*******************************************************************************/
ReturnCode sysNfcDpoSetAdjustMethod( sysNfcDpoAdjustMethod adjustId )
{
#if RFAL_FEATURE_DPO
    if ( (adjustId < 0) || (adjustId > RFAL_SIZEOF_ARRAY(adjustEntry)) )
    {
        return RFAL_ERR_PARAM;
    }

    rfalDpoSetAdjustCallback( adjustEntry[adjustId] );
    return RFAL_ERR_NONE;
#else
    return RFAL_ERR_DISABLED;
#endif /* RFAL_FEATURE_DPO */
}


/*******************************************************************************/
ReturnCode sysNfcDpoSetMeasureMethod( sysNfcDpoMeasureMethod measureId )
{
#if RFAL_FEATURE_DPO
    if ( (measureId < 0) || (measureId > RFAL_SIZEOF_ARRAY(measureEntry)) )
    {
        return RFAL_ERR_PARAM;
    }

    rfalDpoSetMeasureCallback( measureEntry[measureId] );
    return RFAL_ERR_NONE;
#else
    return RFAL_ERR_DISABLED;
#endif /* RFAL_FEATURE_DPO */
}


/*******************************************************************************/
static sysNfcDpoAdjustMethod sysNfcDpoAdjustFuncToId( rfalDpoAdjustFunc pFunc )
{
    uint32_t i;
    for (i = 0; i < RFAL_SIZEOF_ARRAY(adjustEntry); i++)
    {
        if (adjustEntry[i] == pFunc)
        {
            return (sysNfcDpoAdjustMethod)i;
        }
    }
    return SYS_NFC_DPO_ADJUST_NONE;
}

/*******************************************************************************/
static sysNfcDpoMeasureMethod sysNfcDpoMeasureFuncToId( rfalDpoMeasureFunc pFunc )
{
    uint32_t i;
    for (i = 0; i < RFAL_SIZEOF_ARRAY(measureEntry); i++)
    {
        if (measureEntry[i] == pFunc)
        {
            return (sysNfcDpoMeasureMethod)i;
        }
    }
    return SYS_NFC_DPO_MEASURE_NONE;
}


/*******************************************************************************/
ReturnCode sysNfcDpoGetInfo( sysNfcDpoInfo* sysDpoInfo )
{
    ReturnCode  ret;
    rfalDpoInfo dpoInfo;
    uint32_t    i;

    if( sysDpoInfo == NULL )
    {
        return RFAL_ERR_PARAM;
    }

    ret = rfalDpoGetInfo( &dpoInfo );
    if( ret != RFAL_ERR_NONE )
    {
        return ret;
    }

    /* Clear info structure */
    RFAL_MEMSET( sysDpoInfo, 0, sizeof(sysNfcDpoInfo) );

    sysDpoInfo->enabled      = dpoInfo.enabled;
    for (i = 0; i < RFAL_DPO_TABLE_MAX_ENTRIES; i++)
    {
        sysDpoInfo->table[i] = dpoInfo.table[i]; /* copy structure */
    }
    sysDpoInfo->tableEntries = dpoInfo.tableEntries;
    sysDpoInfo->tableEntry   = dpoInfo.tableEntry;
    // Convert function pointers to enumerated values
    sysDpoInfo->adjustId     = sysNfcDpoAdjustFuncToId(dpoInfo.adjustCallback);
    sysDpoInfo->measureId    = sysNfcDpoMeasureFuncToId(dpoInfo.measureCallback);
    sysDpoInfo->refMeasure   = dpoInfo.refMeasure;

    return RFAL_ERR_NONE;
}
