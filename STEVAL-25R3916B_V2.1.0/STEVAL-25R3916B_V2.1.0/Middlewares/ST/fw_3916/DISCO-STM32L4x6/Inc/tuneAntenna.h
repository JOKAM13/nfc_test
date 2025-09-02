/*
 *****************************************************************************
 * Copyright by ams AG                                                       *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
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
 *  \brief ST25R3916 Antenna Tuning 
 *
 */
/*!
 * 
 */

#ifndef TUNE_ANTENNA_H
#define TUNE_ANTENNA_H

#include <stdint.h>
#include "rfal_utils.h"

/*! 
 *****************************************************************************
 * Antenna Tuning
 *
 * The antenna tuning algorithm try to find the a optimal settings for 
 * the AAT_A and AAT_B registers, which are connected to variable capacitors 
 * to tune the antenna matching.
 * 
 * The algorithm is divided into two parts to find the optimal settings. First 
 * the algorithm try to find the area where the first criteria match and
 * then the search is continued to find the second criteria. 
 *
 *  
 *
 */



/*
******************************************************************************
* GLOBAL DATATYPES
******************************************************************************
*/


/*! 
 * Tuning algorithm parameters.
 */
typedef enum
{
	// tuning parameters
	MODE_MEASURE_SER_AND_PAR_CAP_VARIABLE = 0x00,	/*!< two-dimensional search */
	MODE_MEASURE_SER_CAP_FIXED = 0x01,	/*!< one-dimensional search with fixed serial cap */
	MODE_MEASURE_PAR_CAP_FIXED = 0x02,	/*!< one-dimensional search with fixed parallel cap */
	
  // tuning algorithms
	MODE_ALGO_SER_SWEEP_AND_PAR_BINARY_SEARCH = 0x10, /*!< Default algorithm: Sweep and search binary */
	MODE_ALGO_PAR_SWEEP_AND_SER_BINARY_SEARCH = 0x20, /*!< Default algorithm: Sweep and search binary */
	MODE_ALGO_2DIM_BINARY_SEARCH = 0x30,  /*!< 2 Dimensional binary search */
	MODE_ALGO_HILL_CLIMB = 0x40,          /*!< Hill climbing */
}tuningAlgorithmMode_t;



/*!< 
 * struct representing input parameters for the antenna tuning
 */
typedef struct{
    tuningAlgorithmMode_t mode; /*<! Algorithm modes */

		uint8_t AAT_A_first;			  /*<! starting value of serial cap */
		uint8_t AAT_A_last;				  /*<! last value of serial cap */
		uint8_t AAT_A_stepWidth;    /*<! increment stepWidth for serial sweep */
		uint8_t AAT_B_first;			  /*<! starting value of parallel cap */
		uint8_t AAT_B_last;				  /*<! last value of parallel cap */
		uint8_t AAT_B_stepWidth;    /*<! increment stepWidth for parallel sweep */

    uint8_t phaTarget;				  /*<! target phase to trim */
    uint8_t phaWeight;				  /*<! delta of target phase */
		uint8_t ampTarget;				  /*<! target amplitude to trim */
		uint8_t ampWeight;				  /*<! delta of target amplitude */
}tuningParams_t;


/*!< 
 * struct representing out parameters for the antenna tuning
 */
typedef struct{

    uint8_t AAT_A_tuned;        /*<! serial cap after tuning */
    uint8_t AAT_B_tuned;        /*<! parallel cap after tuning */
    uint8_t phaTuned;				    /*<! phase after tuning */
		uint8_t ampTuned;				    /*<! amplitude after tuning */
		uint16_t measureCnt;        /*<! number of measures performed */
}tuningStatus_t;



/*! 
 *****************************************************************************
 *  \brief  Perform antenna tuning
 *
 *  This function start a antenna tuning procedure by modifying the serial 
 *  and parallel capacitors of the antenna matching circuit via the AAT_A
 *  and AAT_B registers. 
 *   
 *  \param[in] tuningParams : Input parameters for the tuning algorithm  
 *  \param[out] tuningStatus : Result information of performed tuning.
 *
 *  \return GP_ERR_PARAM : invalid input parameters.
 *  \return GP_ERR_NOTFOUND : Chosen criteria not found, using nearest values
 *  \return GP_ERR_IO : Error during communication.
 *  \return GP_ERR_NONE : No error.
 *
 *****************************************************************************
 */
extern ReturnCode tuneAntenna(tuningParams_t *tuningParams, tuningStatus_t *tuningStatus);



#endif // TUNE_ANTENNA_H
