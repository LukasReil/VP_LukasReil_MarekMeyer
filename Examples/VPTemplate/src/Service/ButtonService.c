/******************************************************************************
 * @file ButtonService.c
 *
 * @author Lukas Reil
 * @date   22.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************/

/***** INCLUDES **************************************************************/

#include "ButtonService.h"
#include "../HAL/ButtonModule.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/

/**
 * @brief   Filters the input value using a hysterese filter
 *          The filter is defined by the constant BUTTON_FILTER_WINDOW_SIZE
 * 
 * @param   input       Value to filter
 * @param   lastInputs  Array of last inputs
 * @param   output      Pointer to the output value 
 */
void hysterese(uint8_t input, uint8_t* lastInputs, uint8_t* output);

/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

uint8_t g_buttonB1Value;
uint8_t g_buttonSW1Value;
uint8_t g_buttonSW2Value;

uint8_t readButtonB1()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_B1) == BUTTON_PRESSED;
    hysterese(buttonValue, lastInputs, &g_buttonB1Value);
    return g_buttonB1Value;
}

uint8_t readButtonSW1()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_SW1) == BUTTON_PRESSED;
    hysterese(buttonValue, lastInputs, &g_buttonSW1Value);
    return g_buttonSW1Value;    
}

uint8_t readButtonSW2()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_SW2) == BUTTON_PRESSED;
    hysterese(buttonValue, lastInputs, &g_buttonSW2Value);
    return g_buttonSW2Value;
}

/***** PRIVATE FUNCTIONS *****************************************************/

void hysterese(uint8_t input, uint8_t *lastInputs, uint8_t *output)
{
    uint8_t sum = input;
    for(int i = BUTTON_FILTER_WINDOW_SIZE - 1; i > 0; i--)
    {
        lastInputs[i] = lastInputs[i-1];
        sum += lastInputs[i];
    }
    lastInputs[0] = input;

    // Only change the output if the last BUTTON_FILTER_WINDOW_SIZE inputs are the same
    if(sum == 0)
    {
        *output = 0;
    }
    else if(sum == BUTTON_FILTER_WINDOW_SIZE)
    {
        *output = 1;
    }
}
