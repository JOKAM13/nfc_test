/**
  ******************************************************************************
  * File Name          : tuneAntenna.c
  * Description        : Automatic Antenna Tunning
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
/*
 *      PROJECT:   AS3916 firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author 
 *
 *  \brief \brief AS3916 Antenna Tuning 
 *
 */
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "tuneAntenna.h"
#include "st25r3916.h"
#include "st25r3916_com.h"
#include "rfal_chip.h"
#include "logger.h"

#define CAP_DELAY_MAX           10
#define MAX_MEASURES            6000

static ReturnCode findOptimunWithSweepLines(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus);

static ReturnCode narrowSector(tuningParams_t *tuningParams, uint32_t *midVal, tuningStatus_t *tuningStatus);
static ReturnCode findOptimunWith2DimBinSearch(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus);

static ReturnCode findOptimunWithHillClimb(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus);
static ReturnCode getHillClimbDirection(uint32_t *fmin, tuningParams_t *tuningParams, tuningStatus_t *tuningStatus);

static ReturnCode getValues(uint8_t serCap, uint8_t parCap, uint8_t *amplitude, uint8_t *phase, uint16_t *measureCnt);
static bool isFirst(tuningParams_t *tuningParams, uint8_t ampFirst, uint8_t phaFirst, uint8_t ampLast, uint8_t phaLast);
static uint32_t calcF(tuningParams_t *tuningParams, uint8_t amplitude, uint8_t phase);
static uint8_t sat_addu8b(uint8_t x, uint8_t y);
static uint8_t sat_subu8b(uint8_t x, uint8_t y);


#define TUNE_LOG(...) 
//#define TUNE_LOG logUsart

// --------------------------------------------------------------------------
static ReturnCode initialize(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus)
{
    tuningStatus->measureCnt = 0;
    return RFAL_ERR_NONE;
}


// --------------------------------------------------------------------------
ReturnCode tuneAntenna(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus)
{
    ReturnCode err = initialize(tuningParams, tuningStatus);
    if(err == RFAL_ERR_NONE){
        switch(tuningParams->mode & 0xF0){
            case MODE_ALGO_SER_SWEEP_AND_PAR_BINARY_SEARCH:
            case MODE_ALGO_PAR_SWEEP_AND_SER_BINARY_SEARCH:
                err |= findOptimunWithSweepLines(tuningParams, tuningStatus);
                break;

            case MODE_ALGO_2DIM_BINARY_SEARCH:
                err |= findOptimunWith2DimBinSearch(tuningParams, tuningStatus);
                break;

            case MODE_ALGO_HILL_CLIMB:
                err |= findOptimunWithHillClimb(tuningParams, tuningStatus);
                break;

            default:
                err = RFAL_ERR_PARAM;
        }

    }
    return err;
}

// --------------------------------------------------------------------------
static ReturnCode findOptimunWithSweepLines(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus)
{
    ReturnCode  err = RFAL_ERR_NONE;
    uint16_t yCapFirst = tuningParams->AAT_A_first, yCapLast = tuningParams->AAT_A_last;
    uint16_t xCapFirst = tuningParams->AAT_B_first, xCapLast = tuningParams->AAT_B_last;
    uint8_t yStepWidth = tuningParams->AAT_A_stepWidth;
    uint8_t algo = (uint8_t)tuningParams->mode & 0xF0;
    //uint8_t param = tuningParams->mode & 0x0F;

    if(algo & MODE_ALGO_PAR_SWEEP_AND_SER_BINARY_SEARCH){
        // in case we want to sweep over parallel instead of serial..
        yCapFirst = tuningParams->AAT_B_first, yCapLast = tuningParams->AAT_B_last;
        xCapFirst = tuningParams->AAT_A_first, xCapLast = tuningParams->AAT_A_last;
        yStepWidth = tuningParams->AAT_B_stepWidth;
    }

    uint8_t xCapWinnerLocal = 0;
    uint8_t xCapWinner = 0, yCapWinner = 0;

    uint8_t ampFirst, phaFirst;
    uint8_t ampLast, phaLast;

    uint8_t xAmpWinner = 0, xPhaWinner = 0;
    uint8_t ampWinner = 0xFF, phaWinner = 0xFF;

    for (int yCap = yCapFirst; yCap < yCapLast; yCap += yStepWidth)
    {

        getValues(yCap, xCapFirst, &ampFirst, &phaFirst, &tuningStatus->measureCnt);
        getValues(yCap, xCapLast, &ampLast, &phaLast, &tuningStatus->measureCnt);
        TUNE_LOG("%d-%d, %d-%d : ",xCapFirst, yCap, xCapLast,yCap);
        tuningStatus->measureCnt += 2;

        xCapFirst = tuningParams->AAT_B_first;
        xCapLast = tuningParams->AAT_B_last;
        while (xCapFirst+1 < xCapLast)
        {
            bool isFirstBetter = isFirst(tuningParams, ampFirst, phaFirst, ampLast, phaLast);

            if(isFirstBetter){
                // first stay first
                // mid become new last
                xCapLast -= ((xCapLast - xCapFirst) / 2);
                getValues(yCap, xCapLast, &ampLast, &phaLast, &tuningStatus->measureCnt);
                TUNE_LOG("%d-%d, %d-%d : ",xCapLast, yCap, xCapLast,yCap);
                // update winner
                xCapWinnerLocal = xCapLast;
                xAmpWinner = ampLast;
                xPhaWinner = phaLast;
            }
            else {
                // mid become new first
                // last stay last
                xCapFirst += ((xCapLast - xCapFirst) / 2);
                getValues(yCap, xCapFirst, &ampFirst, &phaFirst, &tuningStatus->measureCnt);
                TUNE_LOG("%d-%d, %d-%d : ",xCapFirst, yCap, xCapLast,yCap);
                // update winner
                xCapWinnerLocal = xCapFirst;
                xAmpWinner = ampFirst;
                xPhaWinner = phaFirst;
            }
            tuningStatus->measureCnt++;
        }

        // update the winner ?
        bool isBetter = isFirst(tuningParams, xAmpWinner, xPhaWinner, ampWinner, phaWinner);
        if (isBetter) {
            xCapWinner = xCapWinnerLocal;
            yCapWinner = yCap;

            ampWinner = xAmpWinner;
            phaWinner = xPhaWinner;
        }
    }

    tuningStatus->AAT_A_tuned = yCapWinner;
    tuningStatus->AAT_B_tuned = xCapWinner;

    tuningStatus->phaTuned = phaWinner;
    tuningStatus->ampTuned = ampWinner;
    return err;
}

// --------------------------------------------------------------------------
static ReturnCode narrowSector(tuningParams_t *tuningParams, uint32_t *midVal, tuningStatus_t *tuningStatus)
{
    uint8_t  err = RFAL_ERR_NONE;
    uint8_t yFourth = (tuningParams->AAT_A_last - tuningParams->AAT_A_first + 1) / 4;
    uint8_t xFourth = (tuningParams->AAT_B_last - tuningParams->AAT_B_first + 1) / 4;
    uint8_t best_x = tuningParams->AAT_B_first + (tuningParams->AAT_B_last - tuningParams->AAT_B_first) / 2;
    uint8_t best_y = tuningParams->AAT_A_first + (tuningParams->AAT_A_last - tuningParams->AAT_A_first) / 2;
    uint8_t   a, p;
    uint32_t  fsec;

    for (int sec = 0; sec < 4; sec++) {

        uint8_t x = 0, y = 0;
        switch (sec)
        {
            case 0:
                y = tuningParams->AAT_A_first + yFourth;
                x = tuningParams->AAT_B_first + xFourth;
                break;
            case 1:
                y = tuningParams->AAT_A_first + yFourth;
                x = tuningParams->AAT_B_last - xFourth;
                if (0 == xFourth || 0 == yFourth) continue;
                break;
            case 2:
                y = tuningParams->AAT_A_last - yFourth;
                x = tuningParams->AAT_B_first + xFourth;
                if (0 == xFourth || 0 == yFourth) continue;
                break;
            case 3:
                y = tuningParams->AAT_A_last - yFourth;
                x = tuningParams->AAT_B_last - xFourth;
                break;
            default:
                break;
        }

        getValues(y, x, &a, &p, &tuningStatus->measureCnt);
        fsec = calcF(tuningParams, a, p);
        TUNE_LOG("%d %d: %d\n",x,y,fsec);
        if (fsec < *midVal) {
            *midVal = fsec;
            best_x = x;
            best_y = y;
        } 
    }
    tuningParams->AAT_B_first = best_x - xFourth;
    tuningParams->AAT_B_last = best_x + xFourth;
    tuningParams->AAT_A_first = best_y - yFourth;
    tuningParams->AAT_A_last = best_y + yFourth;
    return err;
}

// --------------------------------------------------------------------------
static ReturnCode findOptimunWith2DimBinSearch(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus)
{
    uint8_t  err = RFAL_ERR_NONE;
    uint32_t midVal;

    getValues(tuningParams->AAT_A_first + (tuningParams->AAT_A_last - tuningParams->AAT_A_first) / 2,
            tuningParams->AAT_B_first + (tuningParams->AAT_B_last - tuningParams->AAT_B_first) / 2,
            &tuningStatus->ampTuned, &tuningStatus->phaTuned,&tuningStatus->measureCnt);

    midVal = calcF(tuningParams, tuningStatus->ampTuned, tuningStatus->phaTuned);
    TUNE_LOG("%d %d: %d\n",tuningParams->AAT_A_first + (tuningParams->AAT_A_last - tuningParams->AAT_A_first) / 2,
            tuningParams->AAT_B_first + (tuningParams->AAT_B_last - tuningParams->AAT_B_first) / 2,
            midVal);

    // We could have here an additional convergence criterion: midVal==0 (or <threshold): take midpoint
    while ((((tuningParams->AAT_A_first + 1) < tuningParams->AAT_A_last) || ((tuningParams->AAT_B_first + 1) < tuningParams->AAT_B_last)) && (midVal > 0)){
        narrowSector(tuningParams, &midVal, tuningStatus);
        if (tuningStatus->measureCnt > MAX_MEASURES) {
            err = RFAL_ERR_OVERRUN;
            break;
        }
    }

    tuningStatus->AAT_A_tuned = tuningParams->AAT_A_first;
    tuningStatus->AAT_B_tuned = tuningParams->AAT_B_first;
    getValues(tuningStatus->AAT_A_tuned, tuningStatus->AAT_B_tuned, &tuningStatus->ampTuned, &tuningStatus->phaTuned, &tuningStatus->measureCnt);

    TUNE_LOG("final res %d\n",calcF(tuningParams, tuningStatus->ampTuned, tuningStatus->phaTuned));

    return err;
}

// --------------------------------------------------------------------------
static ReturnCode findOptimunWithHillClimb(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus)
{
    uint8_t  err = RFAL_ERR_NONE;
    uint32_t fmin = 0xFFFFFFFF;

    while (getHillClimbDirection(&fmin, tuningParams, tuningStatus)) {
        if (tuningStatus->measureCnt > MAX_MEASURES) {
            err = RFAL_ERR_OVERRUN;
            break;
        }
    }

    return err;
}

#define NPOINTS 5
static ReturnCode getHillClimbDirection(uint32_t *fmin, tuningParams_t *tuningParams, tuningStatus_t *tuningStatus)
{
    
    uint8_t a[NPOINTS];
    uint8_t p[NPOINTS];

    uint8_t yCap[NPOINTS] = {
        tuningParams->AAT_A_first,
        sat_addu8b(tuningParams->AAT_A_first, tuningParams->AAT_A_stepWidth),
        tuningParams->AAT_A_first,
        sat_subu8b(tuningParams->AAT_A_first, tuningParams->AAT_A_stepWidth),
        tuningParams->AAT_A_first,
    };

    uint8_t	xCap[NPOINTS] = {
        tuningParams->AAT_B_first,
        tuningParams->AAT_B_first,
        sat_addu8b(tuningParams->AAT_B_first, tuningParams->AAT_B_stepWidth),
        tuningParams->AAT_B_first,
        sat_subu8b(tuningParams->AAT_B_first, tuningParams->AAT_B_stepWidth),
    };

    //
    uint8_t minIdx = 0;	
    uint32_t f[NPOINTS]; 
    for (int i = 0; i < NPOINTS; i++) {
        getValues(yCap[i], xCap[i], &a[i], &p[i],&tuningStatus->measureCnt);
        f[i] = calcF(tuningParams, a[i], p[i]);
        TUNE_LOG("%d %d: %d",xCap[i], yCap[i],f[i]);
        if (f[i] < *fmin) {
            TUNE_LOG("*");
            *fmin = f[i];
            minIdx = i;
        }
        TUNE_LOG("\n");
    }
    tuningParams->AAT_A_first = tuningStatus->AAT_A_tuned = yCap[minIdx];
    tuningParams->AAT_B_first = tuningStatus->AAT_B_tuned = xCap[minIdx];
    tuningStatus->ampTuned = a[minIdx];
    tuningStatus->phaTuned = p[minIdx];

    // something better found ?
    return minIdx > 0;
}

// --------------------------------------------------------------------------
static uint32_t calcF(tuningParams_t *tuningParams, uint8_t amplitude, uint8_t phase)
{
    // f(amp, pha) = (ampWeight * |amp - ampTarget|) + (phaWeight * |pha - phaTarget|)
    uint8_t ampTarget = tuningParams->ampTarget;
    uint8_t phaTarget = tuningParams->phaTarget;

    uint32_t ampWeight = tuningParams->ampWeight;
    uint32_t phaWeight = tuningParams->phaWeight;

    uint32_t ampDelta = (uint32_t)((amplitude > ampTarget)  ? (amplitude - ampTarget) : (ampTarget - amplitude));
    uint32_t phaDelta = (uint32_t)((phase > phaTarget)      ? (phase - phaTarget)     : (phaTarget - phase));

    return ((ampWeight * ampDelta) + (phaWeight * phaDelta));
}

// --------------------------------------------------------------------------
static bool isFirst(tuningParams_t *tuningParams, uint8_t ampFirst, uint8_t phaFirst, uint8_t ampLast, uint8_t phaLast)
{
    // f(amp, pha) = (ampWeight * |amp - ampTarget|) + (phaWeight * |pha - phaTarget|)
    uint32_t first =  calcF(tuningParams,  ampFirst, phaFirst);
    uint32_t last =  calcF(tuningParams,  ampLast, phaLast);
    TUNE_LOG("%d, %d\n",first, last);
    return (first < last);
}

// --------------------------------------------------------------------------
static ReturnCode getValues(uint8_t serCap, uint8_t parCap, uint8_t *amplitude, uint8_t *phase, uint16_t *measureCnt)
{
    // set AAT_A (serial) and AAT_B (parallel)
    ReturnCode err = RFAL_ERR_IO;

    st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_A, serCap);
    st25r3916WriteRegister(ST25R3916_REG_ANT_TUNE_B, parCap);

    // wait till caps have settled..
    platformDelay(CAP_DELAY_MAX);
    
    // get amplitude and phase ..
    err = rfalChipMeasureAmplitude(amplitude);
    err |= rfalChipMeasurePhase(phase);
        
    //
    (*measureCnt)++;
    return err;
}

static uint8_t sat_addu8b(uint8_t x, uint8_t y){
	uint8_t res = x + y;
	res |= -(res < x);
	return res;
}
static uint8_t sat_subu8b(uint8_t x, uint8_t y){
	uint8_t res = x - y;
	res &= -(res <= x);
	return res;
}
