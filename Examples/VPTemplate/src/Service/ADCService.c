/******************************************************************************
 * @file ADCService.c
 *
 * @author Lukas Reil
 * @date   22.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************/

/***** INCLUDES **************************************************************/

#include "ADCService.h"
#include "../HAL/ADCModule.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/

/**
 * @brief   Inverse of the alpha value for the exponential moving average filter of the first potentiometer
 *          Inverse is used to avoid floating point arithmetic
 */
#define POT1_EMA_ALPHA_INV 5

/**
 * @brief   Number of iterations to initialize the first potentiometer
 *          At a = 0.2 this guarantees that the filter output is within 5% of the input after the initialization
 */
#define POT1_INIT_ITERATIONS 32

/**
 * @brief Size of the moving average filter for the second potentiometer
 */
#define POT2_WINDOW_SIZE    5

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

static int32_t s_pot1Value = 0;
static int32_t s_pot2Value = 0;

void initADCService()
{
    for(uint8_t i = 0; i < POT2_WINDOW_SIZE; i++)
    {
        readPot1();
    }

    for(uint8_t i = 0; i < POT1_INIT_ITERATIONS; i++)
    {
        readPot2();
    }

}

int32_t getPot1Value()
{
    return s_pot1Value;
}

int32_t getPot2Value()
{
    return s_pot2Value;
}

void readPot1()
{
    static int32_t lastOutput = 0;
    int32_t adcValue = adcReadChannel(ADC_INPUT0);
    // filteredValue = adcValue * alpha + (1 - alpha) * lastFilteredValue = adcValue / (1 / alpha) + (1 - 1 / (1 / alpha)) * lastFilteredValue = adcValue / (1 / alpha) + (lastFilteredValue - lastFilteredValue / (1 / alpha))
    // With 1 / alpha = POT1_EMA_ALPHA_INV => filteredValue = adcValue / POT1_EMA_ALPHA_INV + (lastFilteredValue - lastFilteredValue / POT1_EMA_ALPHA_INV)
    lastOutput = adcValue / POT1_EMA_ALPHA_INV + (lastOutput - lastOutput / POT1_EMA_ALPHA_INV);
    s_pot1Value = lastOutput;
}

void readPot2()
{
    static int32_t lastInputs[POT2_WINDOW_SIZE];
    int32_t adcValue = adcReadChannel(ADC_INPUT1);
    int32_t sum = adcValue;
    for(int i = POT2_WINDOW_SIZE - 1; i > 0; i--)
    {
        lastInputs[i] = lastInputs[i-1];
        sum += lastInputs[i];
    }
    lastInputs[0] = adcValue;
    s_pot2Value = sum / POT2_WINDOW_SIZE;
}

/***** PRIVATE FUNCTIONS *****************************************************/

