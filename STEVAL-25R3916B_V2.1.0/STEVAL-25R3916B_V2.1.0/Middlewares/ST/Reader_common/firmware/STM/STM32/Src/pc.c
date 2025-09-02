/**
  ******************************************************************************
  * @file           : pac.c   (Persistent Config)
  * @version        : v2.0_Cube
  * @brief          : This file implements the Persistent Config management on flash
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

/* Includes ------------------------------------------------------------------*/

#include "st_errno.h"
#include "pc.h"
#include "tlv.h"
#include "rfal_analogConfig.h"
#if RFAL_FEATURE_DPO
#include "rfal_dpo.h"
#endif /* RFAL_FEATURE_DPO */
#if RFAL_FEATURE_DLMA 
#include "rfal_dlma.h"
#endif /* RFAL_FEATURE_DLMA */
#include "flash_driver.h"

/* Private typedef -----------------------------------------------------------*/

/* Persistent Config header info */
typedef struct
{
    /* use volatile to prevent the compiler to get rid of Flash access on high optimization level*/
    const uint8_t* volatile data;           /* Persistent data starting pointer     */    
    volatile uint32_t       acHash;        /* Hash of the current AC stored on flash   */
    volatile uint32_t       acHash_orig;   /* Hash of the original AC */
    volatile uint32_t       dpoHash;       /* Hash of the current Analog Config   */
    volatile uint32_t       dpoHash_orig;  /* Hash of the original Analog Config  */
    volatile uint32_t       dlmaHash;      /* Hash of the current Analog Config   */
    volatile uint32_t       dlmaHash_orig; /* Hash of the original Analog Config  */
    volatile uint32_t       unused[1];    /* Padding to reach multiple of double-words (8 bytes alignment)
                                             and quad-words (16 bytes alignment)*/
} t_PersistentConfigHeader;

/* Private define ------------------------------------------------------------*/
#define TLV_TAG_AC                          PC_ID_AC

#define TLV_TAG_DPO                         PC_ID_DPO
#define TLV_TAG_DPO_IS_ENABLED              PC_TAG_IS_DPO_ENABLED
#define TLV_TAG_DPO_DATA                    PC_TAG_DPO_RAW_DATA

#define TLV_TAG_DLMA                        PC_ID_DLMA
#define TLV_TAG_DLMA_IS_ENABLED             PC_TAG_IS_DLMA_ENABLED
#define TLV_TAG_DLMA_DATA                   PC_TAG_DLMA_RAW_DATA


#define PERSISTENT_CONFIG_HASH_INITVAL     (0x6363)     /* Initial value to compute hash */

#ifndef PERSISTENT_CONFIG_OFFSET 
#define PERSISTENT_CONFIG_OFFSET           (0x00007f000 ) /* Config offset in Flash memory */
#endif                                      
#define PERSISTENT_CONFIG_ADDRESS          (FLASH_BASE + PERSISTENT_CONFIG_OFFSET)
                                            
#define PERSISTENT_CONFIG_FLASH_PAGE_NUM   ((PERSISTENT_CONFIG_OFFSET % FLASH_BANK_SIZE) / (FLASH_PAGE_SIZE))

#define PERSISTENT_CONFIG_FLASH_BANK_NUM   (((PERSISTENT_CONFIG_OFFSET / FLASH_BANK_SIZE) == 0) ? FLASH_BANK_1 : FLASH_BANK_2)
                                            
#ifndef PERSISTENT_CONFIG_FLASH_PAGE_COUNT 

#define PERSISTENT_CONFIG_DATA_MAX_SIZE_AC     (sizeof(TLV_TAG) + sizeof(uint16_t) + RFAL_ANALOG_CONFIG_TBL_SIZE)
#if RFAL_FEATURE_DPO
#define PERSISTENT_CONFIG_DATA_MAX_SIZE_DPO    ((sizeof(TLV_TAG) + sizeof(uint16_t)) + \
                                                (sizeof(TLV_TAG) + sizeof(uint16_t) + ((RFAL_DPO_TABLE_MAX_ENTRIES * sizeof(rfalDpoEntry)) / sizeof(uint64_t))) + \
                                                (sizeof(TLV_TAG) + (2*sizeof(uint8_t))))
#else
#define PERSISTENT_CONFIG_DATA_MAX_SIZE_DPO    0U
#endif /* RFAL_FEATURE_DPO */
                                                
                                                #if RFAL_FEATURE_DLMA 
#define PERSISTENT_CONFIG_DATA_MAX_SIZE_DLMA   ((sizeof(TLV_TAG) + sizeof(uint16_t)) + \
                                                (sizeof(TLV_TAG) + sizeof(uint16_t) + ((RFAL_DLMA_TABLE_MAX_ENTRIES * sizeof(rfalDlmaEntry)) / sizeof(uint64_t))) + \
                                                (sizeof(TLV_TAG) + (2*sizeof(uint8_t))))
#else
#define PERSISTENT_CONFIG_DATA_MAX_SIZE_DLMA    0U
#endif /* RFAL_FEATURE_DLMA */
#define PERSISTENT_CONFIG_DATA_MAX_SIZE        (PERSISTENT_CONFIG_DATA_MAX_SIZE_AC + \
                                                PERSISTENT_CONFIG_DATA_MAX_SIZE_DPO + \
                                                PERSISTENT_CONFIG_DATA_MAX_SIZE_DLMA)
                                            
#define PERSISTENT_CONFIG_FLASH_PAGE_COUNT ((PERSISTENT_CONFIG_DATA_MAX_SIZE + (FLASH_PAGE_SIZE - 1)) / (FLASH_PAGE_SIZE))
#endif                                      
                                            
#define PERSISTENT_CONFIG_FLASH_SIZE       ((FLASH_PAGE_SIZE) * PERSISTENT_CONFIG_FLASH_PAGE_COUNT)
                                            
#define PERSISTENT_CONFIG_HEADER_ADDRESS   (PERSISTENT_CONFIG_ADDRESS)
                                            
#define PERSISTENT_CONFIG_DATA_ADDRESS     (PERSISTENT_CONFIG_ADDRESS    + sizeof(t_PersistentConfigHeader))
#define PERSISTENT_CONFIG_DATA_LENGTH      (PERSISTENT_CONFIG_FLASH_SIZE - sizeof(t_PersistentConfigHeader))
                                            
                                            
#define PERSISTENT_CONFIG_HASH_MARKER_BAD   0xffddaabb
#define PERSISTENT_CONFIG_HASH_MARKER_DEAD  0xddaaeedd
#define PERSISTENT_CONFIG_HASH_MARKER_CACA  0xcacacaca


/* Reserve an entire page of Flash memory */

#if defined( __CC_ARM ) || defined(__ARMCC_VERSION) /* compiled with Keil v5 or v6 */

#define PERSISTENT_CONFIG_HEADER_ADDR 0x0807f000
#define PERSISTENT_CONFIG_DATA_ADDR   0x0807f020

/* Trick to check the value manually set matches the computed defines */
extern bool checker_header_addr[ PERSISTENT_CONFIG_HEADER_ADDRESS == PERSISTENT_CONFIG_HEADER_ADDR ? 1 : -1 ];
extern bool checker_data_addr  [ PERSISTENT_CONFIG_DATA_ADDRESS   == PERSISTENT_CONFIG_DATA_ADDR   ? 1 : -1 ];

#define str(address)            #address
#define sectionAt(address)      ".ARM.__at_" str(address)
const t_PersistentConfigHeader __attribute__(( section(sectionAt(PERSISTENT_CONFIG_HEADER_ADDR)) )) PersistentConfig  = {.data = NULL, .acHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .acHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD, .dpoHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .dpoHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD, .dlmaHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .dlmaHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD };
const uint8_t                   __attribute__(( section(sectionAt(PERSISTENT_CONFIG_DATA_ADDR))   )) PersistentConfigTable[PERSISTENT_CONFIG_DATA_LENGTH] = {0};

#elif defined( __ICCARM__ ) /* compiled with IAR */

const t_PersistentConfigHeader PersistentConfig @ PERSISTENT_CONFIG_HEADER_ADDRESS = {.data = NULL, .acHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .acHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD, .dpoHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .dpoHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD, .dlmaHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .dlmaHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD };
const uint8_t                   PersistentConfigTable[PERSISTENT_CONFIG_DATA_LENGTH] @ PERSISTENT_CONFIG_DATA_ADDRESS = {0};

#elif defined( __GNUC__ ) /* compiled with GCC */

const t_PersistentConfigHeader __attribute__((section(".ST25R_PC"))) PersistentConfig  = {.data = NULL, .acHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .acHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD, .dpoHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .dpoHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD, .dlmaHash = PERSISTENT_CONFIG_HASH_MARKER_BAD, .dlmaHash_orig = PERSISTENT_CONFIG_HASH_MARKER_DEAD };
const uint8_t                   __attribute__((section(".ST25R_PC"))) PersistentConfigTable[PERSISTENT_CONFIG_DATA_LENGTH] = {0};

#endif


/* Private function prototypes -----------------------------------------------*/
static bool persistentConfig_Install(const t_PersistentConfigHeader * cfg);

/*!
 *****************************************************************************
 * \brief Install Persistent Analog Config table
 *
 * return true when Analog Config has been properly written to RAM
 *
 *****************************************************************************
 */
static bool persistentConfig_Install(const t_PersistentConfigHeader * cfg)
{
    uint16_t offset = 0;
#if RFAL_FEATURE_DPO
    uint16_t parentDpoTlvPos = 0;
#endif /* RFAL_FEATURE_DPO */
#if RFAL_FEATURE_DLMA
    uint16_t parentDlmaTlvPos = 0;
#endif /* RFAL_FEATURE_DLMA */
    uint16_t length = 0;
    ReturnCode ret = -1;

#if RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG
    /* Get AC and apply */
    if(findTlvExt ((uint8_t *)cfg->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_AC, &offset)){
        length = getTlvLen((uint8_t*)&cfg->data[offset]);
        ret = rfalAnalogConfigListWriteRaw(&cfg->data[offset + sizeof(TLV_TAG) + getTlvLenSizeof(&cfg->data[offset])], length);
    }
#endif /* RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG */

    
#if RFAL_FEATURE_DPO
    /* Get DPO configurtaion and apply */
    if(findTlvExt ((uint8_t *)cfg->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DPO, &offset)){
        parentDpoTlvPos = offset;
        parentDpoTlvPos += sizeof(TLV_TAG);
        parentDpoTlvPos += getTlvLenSizeof(&cfg->data[offset]);
        
        /* parent TAG found, now look for data */        
        if(findTlvExt ((uint8_t *)cfg->data, PERSISTENT_CONFIG_DATA_MAX_SIZE - (parentDpoTlvPos + sizeof(TLV_TAG)), TLV_TAG_DPO_DATA, &parentDpoTlvPos)){
            length = getTlvLen((uint8_t*)&cfg->data[parentDpoTlvPos]);
            ret |= rfalDpoTableWrite((rfalDpoEntry*)&cfg->data[parentDpoTlvPos + sizeof(TLV_TAG) + getTlvLenSizeof(&cfg->data[parentDpoTlvPos])], length/sizeof(rfalDpoEntry));
        }      
    }
#endif /* RFAL_FEATURE_DPO */

    
#if RFAL_FEATURE_DLMA
    /* Get DLMA configurtaion and apply */
    if(findTlvExt ((uint8_t *)cfg->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DLMA, &offset)){
        parentDlmaTlvPos = offset;
        parentDlmaTlvPos += sizeof(TLV_TAG);
        parentDlmaTlvPos += getTlvLenSizeof(&cfg->data[offset]);
        
        /* parent TAG found, now look for data */        
        if(findTlvExt ((uint8_t *)cfg->data, PERSISTENT_CONFIG_DATA_MAX_SIZE - (parentDlmaTlvPos + sizeof(TLV_TAG)), TLV_TAG_DLMA_DATA, &parentDlmaTlvPos)){
            length = getTlvLen((uint8_t*)&cfg->data[parentDlmaTlvPos]);
            ret |= rfalDlmaTableWrite((rfalDlmaEntry*)&cfg->data[parentDlmaTlvPos + sizeof(TLV_TAG) + getTlvLenSizeof(&cfg->data[parentDlmaTlvPos])], length/sizeof(rfalDlmaEntry));
        }       
    }    
#endif /* RFAL_FEATURE_DLMA */
    
    return (ret == ERR_NONE);
}

/*!
 *****************************************************************************
 * \brief Enable Persistent Analog Config
 *
 * Install the Persistent Analog Config table if required or initialise the RAM table at startup
 *
 *****************************************************************************
 */
bool persistentConfig_Enable(void)
{
    bool ret = true;
    
    /* Test if persistent config table points to correct location 
       as indication of persistent table is proper programmed to flash */
    if(PersistentConfig.data != PersistentConfigTable)
    {
        /* Create the persistent table with all configurations when it is empty or invalid */
        for(int i = 0; i < NUM_PERSISTENT_CONFIG; i++){        
            persistentConfig_Program(i);
        }
    } 
    else 
    {
        /* Install Persistent Analog Config stored in Flash memory */
        ret = persistentConfig_Install(&PersistentConfig);
    }

    return ret;
}

/*!
 *****************************************************************************
 * \brief Calculate Hash
 *
 * Calculate the CRC of the given buffer
 *
 *****************************************************************************
 */
uint32_t persistentConfig_CalcHash( const uint8_t * buf, uint16_t bufLen )
{
    /* Compute CRC32_FNV-1a */
    const uint32_t  init = ((uint32_t)0x811c9dc5ul);
    uint32_t        crc  = init;
    const uint8_t * end  = buf + bufLen;
    while ( buf < end )
    {
        /* xor the bottom with the current octet */
        crc ^= ( uint32_t ) * buf;
        /* multiply by the 32 bit FNV magic prime mod 2^32 FNV_32_PRIME  = 0x01000193 --> 0001 0000 0000 0000 0001 1001 0011 */
        crc += ( crc << 1 ) + ( crc << 4 ) + ( crc << 7 ) + ( crc << 8 ) + ( crc << 24 );/* crc *= FNV_32_PRIME; */
        ++ buf;
    }
    return ( crc );
}

/*!
 *****************************************************************************
 * \brief Get Hash of configuration
 *
 * Retrieve the hash calculate from the specified configuration data 
 *
 *****************************************************************************
 */
stError persistentConfig_GetHash (pcID hashId, uint32_t* hashOriginal, uint32_t* hashFlash, uint32_t* hashRam)
{
    /* use volatile to prevent the compiler to get rid of Flash access on high optimization level */
    const volatile t_PersistentConfigHeader * cfg = & PersistentConfig;
    uint8_t   tmpData[RFAL_ANALOG_CONFIG_TBL_SIZE];
    uint16_t  tmpDataLen;    
    
    /* Checking param. No NULL pointer allowed */
    if((NULL == hashOriginal) || (NULL == hashFlash) || (NULL == hashRam)){
        return ERR_REQUEST;
    }

    /* Get Hashes */
    switch(hashId){        
        #if RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG
        case PC_ID_AC: 
            *hashOriginal = cfg->acHash_orig;
            *hashFlash = cfg->acHash;
            tmpDataLen = 0;
            rfalAnalogConfigListReadRaw( tmpData, sizeof(tmpData), &tmpDataLen );
            *hashRam = persistentConfig_CalcHash( tmpData, tmpDataLen );
        break;
        #endif

        #if RFAL_FEATURE_DPO
        case PC_ID_DPO : 
            *hashOriginal = cfg->dpoHash_orig;
            *hashFlash = cfg->dpoHash;
            tmpDataLen = 0;
            rfalDpoTableRead( (rfalDpoEntry*)tmpData, RFAL_DPO_TABLE_MAX_ENTRIES, (uint8_t*)&tmpDataLen);
            tmpData[(tmpDataLen * sizeof(rfalDpoEntry))] = rfalDpoIsEnabled() ? 1 : 0;
            *hashRam = persistentConfig_CalcHash( tmpData, 1 /* sizeof(isEnabledFlag) */ + (tmpDataLen * sizeof(rfalDpoEntry)));
            break;
        #endif
        
        #if RFAL_FEATURE_DLMA
        case PC_ID_DLMA: 
            *hashOriginal = cfg->dlmaHash_orig;
            *hashFlash = cfg->dlmaHash;
            tmpDataLen = 0;
            rfalDlmaTableRead( (rfalDlmaEntry*)tmpData, RFAL_DLMA_TABLE_MAX_ENTRIES, (uint8_t*)&tmpDataLen);
            tmpData[(tmpDataLen * sizeof(rfalDlmaEntry))] = rfalDlmaIsEnabled() ? 1 : 0;
            *hashRam = persistentConfig_CalcHash( tmpData, 1 /* sizeof(isEnabledFlag) */ + (tmpDataLen * sizeof(rfalDlmaEntry)));
            break;
        #endif
        
        default:
            return ERR_REQUEST;
    }
    
    return ERR_NONE;
}

/*!
 *****************************************************************************
 * \brief Program Persistent Analog Config page
 *
 * Retrieve actual Analog Configs in RAM and write it permanently to dedicated flash page
 *
 *****************************************************************************
 */

#if ((RFAL_PERSISTENT_CONFIG_TBL_SIZE) % 16) != 0
#error "RFAL Analog Config table size must be multiple of quad words due to HAL_FLASH_Program() pace"
#endif

static uint64_t cfgData[PERSISTENT_CONFIG_DATA_MAX_SIZE / sizeof(uint64_t)];  

stError persistentConfig_Program (pcID pcId)
{
    stError err = ERR_NONE;

    t_PersistentConfigHeader  pcHeader;
    uint32_t acHash = PERSISTENT_CONFIG_HASH_MARKER_BAD;
    uint32_t dpoHash = PERSISTENT_CONFIG_HASH_MARKER_BAD;
    uint32_t dlmaHash = PERSISTENT_CONFIG_HASH_MARKER_BAD;
    
    uint16_t cfgDataOffset = 0;
    uint8_t *cfgData8 = (uint8_t *)cfgData;
    
    uint8_t tmpAc[RFAL_ANALOG_CONFIG_TBL_SIZE];
#if RFAL_FEATURE_DPO 
    uint8_t tmpDpo[(RFAL_DPO_TABLE_MAX_ENTRIES * sizeof(rfalDpoEntry)) + sizeof(uint8_t)/*reserve space for IS_ENABLED info*/];
#endif /* RFAL_FEATURE_DPO */
#if RFAL_FEATURE_DLMA 
    uint8_t tmpDlma[(RFAL_DLMA_TABLE_MAX_ENTRIES * sizeof(rfalDlmaEntry)) + sizeof(uint8_t)/*reserve space for IS_ENABLED info*/];
#endif /* RFAL_FEATURE_DLMA */
    uint16_t tmpAcSize = 0;
#if RFAL_FEATURE_DPO
    uint16_t tmpDpoSize = 0;
    uint8_t  dpoEntries = 0;
#endif /* RFAL_FEATURE_DPO */
#if RFAL_FEATURE_DLMA
    uint16_t tmpDlmaSize = 0;
    uint8_t  dlmaEntries = 0;
#endif /* RFAL_FEATURE_DLMA */
    uint16_t tlvDataOffset = 0;
    uint16_t tlvLength = 0;
    
    /* clear buffer to avoid padding */
    memset(cfgData, 0, sizeof(cfgData));
    
        
    /* Get header info */
    memcpy(& pcHeader, (const void*)& PersistentConfig, sizeof(pcHeader));
    /* In case not proper set config table address update address */
    if(pcHeader.data != PersistentConfigTable){
        pcHeader.data = PersistentConfigTable; /* point to persistent page */
    }
    
    /* iterate over list of configurations to make persistent */
    for(int i = 0; i < NUM_PERSISTENT_CONFIG; i++){
        /* build up TLV data */
        switch(i){
            
        #if RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG            
            case PC_ID_AC:{                

                if(i == pcId){
                    /* AC: get current AC from RAM, TLV encode it and store it to be programmed to FLASH */                    
                    if(RFAL_ERR_NONE == rfalAnalogConfigListReadRaw( (uint8_t*)tmpAc, sizeof(tmpAc), &tmpAcSize)){
                        /* Check to ensure in bound array access */
                        if((cfgDataOffset + tmpAcSize) > PERSISTENT_CONFIG_DATA_MAX_SIZE) {
                            return ERR_IO;
                        }  
                        cfgDataOffset += addTlvExt(&cfgData8[cfgDataOffset], TLV_TAG_AC, tmpAcSize, tmpAc);
                        /* calculate new hash */
                        acHash = persistentConfig_CalcHash( (uint8_t*)tmpAc, tmpAcSize);
                    }
                }
                else{
                    /* otherwise backup existing flash data */                    
                    if(findTlvExt ((uint8_t *)pcHeader.data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_AC, &tlvDataOffset)){
                        tlvLength = getTlvLen((uint8_t*)&pcHeader.data[tlvDataOffset]);
                        tmpAcSize = (sizeof(TLV_TAG) + getTlvLenSizeof(&pcHeader.data[tlvDataOffset]) + tlvLength);
                        /* Check to ensure in bound array access */
                        if((cfgDataOffset + tmpAcSize) > PERSISTENT_CONFIG_DATA_MAX_SIZE) {
                            return ERR_IO;
                        }                         
                        memcpy(&cfgData8[cfgDataOffset], &pcHeader.data[tlvDataOffset], tmpAcSize);
                        cfgDataOffset += tmpAcSize;                        
                        acHash = pcHeader.acHash;
                    }                          
                }
                break;
            }
        #endif /* RFAL_FEATURE_DYNAMIC_ANALOG_CONFIG */
            
            
        #if RFAL_FEATURE_DPO
            case PC_ID_DPO:{               

                if(i == pcId){
                    /* DPO: get current configuration, TLV encode it and store it to be programmed to FLASH */
                    if (RFAL_ERR_NONE == rfalDpoTableRead( (rfalDpoEntry*)tmpDpo, RFAL_DPO_TABLE_MAX_ENTRIES, &dpoEntries)){

                        /* calculate needed size of data to be inserted for parent tlv */
                        int dpoTlvRecordsLen = 0;
                        /* add sizeof(T+L+V) from TLV_TAG_DPO_DATA*/
                        dpoTlvRecordsLen += sizeof(TLV_TAG);
                        dpoTlvRecordsLen += (dpoEntries * sizeof(rfalDpoEntry)) > 0x7F ? sizeof(uint16_t) : sizeof(uint8_t);
                        dpoTlvRecordsLen += (dpoEntries * sizeof(rfalDpoEntry));
                        /* add sizeof(T+L+V) from TLV_TAG_DPO_IS_ENABLED */
                        dpoTlvRecordsLen += sizeof(TLV_TAG);
                        dpoTlvRecordsLen += sizeof(uint8_t);
                        dpoTlvRecordsLen += sizeof(uint8_t);                                                   
                          
                        /* keep placeholder area and move write pos (offset) forward */
                        int parentDpoTlvPos = cfgDataOffset;
                        cfgDataOffset += (sizeof(TLV_TAG) + ((dpoTlvRecordsLen > 0x7F) ? sizeof(uint16_t) : sizeof(uint8_t)));
                        /* Check to ensure in bound array access */
                        if((cfgDataOffset + (dpoEntries * sizeof(rfalDpoEntry))) > PERSISTENT_CONFIG_DATA_MAX_SIZE) {
                            return ERR_IO;
                        }    
                        /* TLV_TAG_DPO_DATA append TLV */    
                        cfgDataOffset += addTlvExt(&cfgData8[cfgDataOffset], TLV_TAG_DPO_DATA, (dpoEntries * sizeof(rfalDpoEntry)), tmpDpo);    
                        /* TLV_TAG_DPO_IS_ENABLED append TLV */    
                        uint8_t isEnabledDpo = rfalDpoIsEnabled();
                        cfgDataOffset += addTlv8(&cfgData8[cfgDataOffset], TLV_TAG_DPO_IS_ENABLED, isEnabledDpo);    
                                
                        /* Generate Master TLV to bind table data and isEnabled info -> TLV_TAG_DPO = TLV_TAG_DPO_DATA + TLV_TAG_DPO_IS_ENABLED */
                        cfgData8[parentDpoTlvPos] = TLV_TAG_DPO;
                        if (dpoTlvRecordsLen > 0x7F) {
                            cfgData8[parentDpoTlvPos + 1] = (uint8_t)(0x80U | (dpoTlvRecordsLen >> 8));
                            cfgData8[parentDpoTlvPos + 2] = (uint8_t)(dpoTlvRecordsLen & 0x00FF);
                        } else {
                            cfgData8[parentDpoTlvPos + 1] = (uint8_t)dpoTlvRecordsLen;
                        }  
                        
                        /* Append IS_ENABLED for subsequent hash calculation */
                        tmpDpo[(dpoEntries * sizeof(rfalDpoEntry))] = isEnabledDpo;
                        tmpDpoSize = (dpoEntries * sizeof(rfalDpoEntry)) + sizeof(uint8_t);
                        
                        /* calculate new hash */
                        dpoHash = persistentConfig_CalcHash( (uint8_t*)tmpDpo, tmpDpoSize);
                    }
                }
                else{
                    /* otherwise backup existing flash data */                    
                    if(findTlvExt ((uint8_t *)pcHeader.data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DPO, &tlvDataOffset)){
                        tlvLength = getTlvLen((uint8_t*)&pcHeader.data[tlvDataOffset]);
                        tmpDpoSize = (sizeof(TLV_TAG) + getTlvLenSizeof(&pcHeader.data[tlvDataOffset]) + tlvLength);
                        /* Check to ensure in bound array access */
                        if((cfgDataOffset + tmpDpoSize) > PERSISTENT_CONFIG_DATA_MAX_SIZE) {
                            return ERR_IO;
                        }                                                
                        memcpy(&cfgData8[cfgDataOffset], &pcHeader.data[tlvDataOffset], tmpDpoSize);                        
                        cfgDataOffset += tmpDpoSize;
                        dpoHash = pcHeader.dpoHash;
                    }                     
                }
                break;
            }
        #endif /* RFAL_FEATURE_DPO */
            
        #if RFAL_FEATURE_DLMA
            case PC_ID_DLMA:{

                if(i == pcId){
                    /* DLMA: get current configuration, TLV encode it and store it to be programmed to FLASH */
                    if (RFAL_ERR_NONE == rfalDlmaTableRead( (rfalDlmaEntry*)tmpDlma, RFAL_DLMA_TABLE_MAX_ENTRIES, &dlmaEntries)){
                        
                        /* calculate needed size of data to be inserted for parent tlv */
                        int dlmaTlvRecordsLen = 0;
                        /* add sizeof(T+L+V) from TLV_TAG_DLMA_DATA*/
                        dlmaTlvRecordsLen += sizeof(TLV_TAG);
                        dlmaTlvRecordsLen += (dlmaEntries * sizeof(rfalDlmaEntry)) > 0x7F ? sizeof(uint16_t) : sizeof(uint8_t);
                        dlmaTlvRecordsLen += (dlmaEntries * sizeof(rfalDlmaEntry));
                        /* add sizeof(T+L+V) from TLV_TAG_DLMA_IS_ENABLED */
                        dlmaTlvRecordsLen += sizeof(TLV_TAG);
                        dlmaTlvRecordsLen += sizeof(uint8_t);
                        dlmaTlvRecordsLen += sizeof(uint8_t);                                                    
                          
                        /* keep placeholder area and move write pos (offset) forward */
                        int parentDlmaTlvPos = cfgDataOffset;
                        cfgDataOffset += (sizeof(TLV_TAG) + ((dlmaTlvRecordsLen > 0x7F) ? sizeof(uint16_t) : sizeof(uint8_t)));
                        /* Check to ensure in bound array access */
                        if((cfgDataOffset + (dlmaEntries * sizeof(rfalDlmaEntry))) > PERSISTENT_CONFIG_DATA_MAX_SIZE) {
                            return ERR_IO;
                        }
                        /* TLV_TAG_DLMA_DATA append TLV */    
                        cfgDataOffset += addTlvExt(&cfgData8[cfgDataOffset], TLV_TAG_DLMA_DATA, (dlmaEntries * sizeof(rfalDlmaEntry)), tmpDlma);    
                        /* TLV_TAG_DLMA_IS_ENABLED append TLV */    
                        uint8_t isEnabledDlma = rfalDlmaIsEnabled();
                        cfgDataOffset += addTlv8(&cfgData8[cfgDataOffset], TLV_TAG_DLMA_IS_ENABLED, isEnabledDlma);    
                                
                        /* Generate Master TLV to bind table data and isEnabled info -> TLV_TAG_DLMA = TLV_TAG_DLMA_DATA + TLV_TAG_DLMA_IS_ENABLED */
                        cfgData8[parentDlmaTlvPos] = TLV_TAG_DLMA;
                        if (dlmaTlvRecordsLen > 0x7F) {
                            cfgData8[parentDlmaTlvPos + 1] = (uint8_t)(0x80U | (dlmaTlvRecordsLen >> 8));
                            cfgData8[parentDlmaTlvPos + 2] = (uint8_t)(dlmaTlvRecordsLen & 0x00FF);
                        } else {
                            cfgData8[parentDlmaTlvPos + 1] = (uint8_t)dlmaTlvRecordsLen;
                        }  

                        /* Append IS_ENABLED for subsequent hash calculation */
                        tmpDlma[(dlmaEntries * sizeof(rfalDlmaEntry))] = isEnabledDlma;
                        tmpDlmaSize = (dlmaEntries * sizeof(rfalDlmaEntry)) + sizeof(uint8_t);

                        /* calculate new hash */
                        dlmaHash = persistentConfig_CalcHash( (uint8_t*)tmpDlma, tmpDlmaSize);                        
                    }
                }
                else{
                    /* otherwise backup existing flash data */                    
                    if(findTlvExt ((uint8_t *)pcHeader.data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DLMA, &tlvDataOffset)){
                        tlvLength = getTlvLen((uint8_t*)&pcHeader.data[tlvDataOffset]);
                        tmpDlmaSize = (sizeof(TLV_TAG) + getTlvLenSizeof(&pcHeader.data[tlvDataOffset]) + tlvLength);
                        /* Check to ensure in bound array access */
                        if((cfgDataOffset + tmpDlmaSize) > PERSISTENT_CONFIG_DATA_MAX_SIZE) {
                            return ERR_IO;
                        }                           
                        memcpy(&cfgData8[cfgDataOffset], &pcHeader.data[tlvDataOffset], tmpDlmaSize);
                        cfgDataOffset += tmpDlmaSize;                          
                        dlmaHash = pcHeader.dlmaHash;
                    }                                            
                }
                break;
            }
            
        #endif /* RFAL_FEATURE_DLMA */
            
            default:
                break;
        }
    }

    /* Update Header CRCs */
    if(PC_ID_AC == pcId){
        pcHeader.acHash = (uint32_t )acHash;   /* store computed hash */
        if (pcHeader.acHash_orig == PERSISTENT_CONFIG_HASH_MARKER_DEAD){
            pcHeader.acHash_orig = acHash;    /* Set original value */
        }    
    }
    if(PC_ID_DPO == pcId){
        pcHeader.dpoHash        = (uint32_t )dpoHash; /* store computed hash */
        if (pcHeader.dpoHash_orig == PERSISTENT_CONFIG_HASH_MARKER_DEAD){
            pcHeader.dpoHash_orig = dpoHash;  /* Set original value */
        }    
    }
    if(PC_ID_DLMA == pcId){
        pcHeader.dlmaHash        = (uint32_t )dlmaHash;   /* store computed hash */
        if (pcHeader.dlmaHash_orig == PERSISTENT_CONFIG_HASH_MARKER_DEAD){
            pcHeader.dlmaHash_orig = dlmaHash;    /* Set original value */
        }    
    }
    /* Update flash storage */
        
    /* Erase page(s) and program data and header */
    err = flash_PageErase(PERSISTENT_CONFIG_FLASH_PAGE_NUM, PERSISTENT_CONFIG_FLASH_PAGE_COUNT, PERSISTENT_CONFIG_FLASH_BANK_NUM);
    if (err != ERR_NONE)
    {
        return ERR_IO;
    }
    /* Program data */
    err = flash_Program((uint32_t) & PersistentConfigTable, cfgData, cfgDataOffset);
    if (err != ERR_NONE){
        err = ERR_WRITE;
    }
    /* Program the header now */
    if (flash_Program((uint32_t)&PersistentConfig, (uint64_t*) &pcHeader, sizeof(t_PersistentConfigHeader)) != ERR_NONE){
        /* Don't overwrite previous error */
        if (err == ERR_NONE){
            err = ERR_WRITE;
        }
    }

    return err;
}


stError persistentConfig_Get(const pcTAG persistentConfig, uint8_t* data, const uint16_t dataSize, uint16_t* dataLength)
{
    stError err = ERR_PARAM;
    uint16_t offset = 0;

    if(NULL == data){
        /* no memory to store reply available  */
        return err;
    }
       
    switch(persistentConfig){
#if PERSISTENTCONFIG_GET_PC_TAG_AC_RAW_DATA
        case PC_TAG_AC_RAW_DATA:
            break;
#endif        
        case PC_TAG_IS_DPO_ENABLED:{
            if(dataSize < sizeof(uint8_t)){
                /* not enough memory to store reply available  */
                return err;
            }
            if(findTlvExt ((uint8_t *)(&PersistentConfig)->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DPO, &offset)){
                /* if parent TAG found look for child */
                offset += sizeof(TLV_TAG) + getTlvLenSizeof(&(&PersistentConfig)->data[offset]);
                if(findTlvExt ((uint8_t *)(&PersistentConfig)->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DPO_IS_ENABLED, &offset)){
                    uint8_t tmpT, tmpV;
                    offset += getTlv8(&(&PersistentConfig)->data[offset], &tmpT, &tmpV);                
                    *dataLength = sizeof(uint8_t);
                    data[0] = tmpV;
                    err = ERR_NONE;
                }
            }        
            break;
        }
#if PERSISTENTCONFIG_GET_PC_TAG_DPO_RAW_DATA
        case PC_TAG_DPO_RAW_DATA   :
            break;
#endif        
        case PC_TAG_IS_DLMA_ENABLED:{            
            if(dataSize < sizeof(uint8_t)){
                /* not enough memory to store reply available  */
                return err;
            }
            if(findTlvExt ((uint8_t *)(&PersistentConfig)->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DLMA, &offset)){
                /* if parent TAG found look for child */
                offset += sizeof(TLV_TAG) + getTlvLenSizeof(&(&PersistentConfig)->data[offset]);
                if(findTlvExt ((uint8_t *)(&PersistentConfig)->data, PERSISTENT_CONFIG_DATA_MAX_SIZE, TLV_TAG_DLMA_IS_ENABLED, &offset)){
                    uint8_t tmpT, tmpV;
                    offset += getTlv8(&(&PersistentConfig)->data[offset], &tmpT, &tmpV);
                    *dataLength = sizeof(uint8_t);
                    data[0] = tmpV;
                    err = ERR_NONE;
                }
            }        
            break;
        }
#if PERSISTENTCONFIG_GET_PC_TAG_DLMA_RAW_DATA  
        case PC_TAG_DLMA_RAW_DATA  :
            break;
#endif        
        default:
            break;
            
    }   
    return err;
}
