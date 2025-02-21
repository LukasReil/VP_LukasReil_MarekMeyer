/******************************************************************************
 * @file ADCService.h
 *
 * @author Lukas Reil
 * @date   22.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Service Layer Module for the ADC
 *
 * @details Contains functions to read the values of the potentiometers and filter them
 *
 *
 *****************************************************************************/

#ifndef _ADC_SERVICE_H
#define _ADC_SERVICE_H

/***** INCLUDES **************************************************************/

#include <stdint.h>


/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define POT1_EMA_ALPHA_INV 10
#define POT2_WINDOW_SIZE    5

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

/**
 * @brief   Returns the value of the first potentiometer
 * 
 * @return  The value of the first potentiometer
 */
int32_t getPot1Value();

/**
 * @brief   Returns the value of the second potentiometer
 * 
 * @return  The value of the second potentiometer
 */
int32_t getPot2Value();

/**
 * @brief   Reads the value of the first potentiometer and filters it with an exponential moving average filter.
 *          The filter is defined by the constant POT1_EMA_ALPHA_INV
 *          The filtered value is stored in the global variable g_pot1Value
 */
void readPot1();

/**
 * @brief   Reads the value of the second potentiometer and filters it with a moving average filter
 *          The filter is defined by the constant POT2_WINDOW_SIZE
 *          The filtered value is stored in the global variable g_pot2Value
 */
void readPot2();




#endif /* _ADC_SERVICE_H */