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


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t g_pot1Value = 0;
int32_t g_pot2Value = 0;

int32_t readPot1()
{
    static int32_t lastOutput = 0;
    int32_t adcValue = adcReadChannel(ADC_INPUT0);
    lastOutput = adcValue / POT1_EMA_ALPHA_INV + (lastOutput - lastOutput / POT1_EMA_ALPHA_INV);
    g_pot1Value = lastOutput;
    return lastOutput;
}

int32_t readPot2()
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
    g_pot2Value = sum / POT2_WINDOW_SIZE;
    return g_pot2Value;
}

/***** PRIVATE FUNCTIONS *****************************************************/

