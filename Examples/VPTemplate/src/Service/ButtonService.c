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
void hysteresis(uint8_t input, uint8_t* lastInputs, uint8_t* output, uint8_t *outputChange);

/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

uint8_t g_buttonB1Value;
uint8_t g_buttonB1ChangeRead;
uint8_t g_buttonSW1Value;
uint8_t g_buttonSW1ChangeRead;
uint8_t g_buttonSW2Value;
uint8_t g_buttonSW2ChangeRead;

uint8_t wasButtonB1Pressed()
{
    if(g_buttonB1ChangeRead)
    {
        return 0;
    }
    g_buttonB1ChangeRead = 1;
    return g_buttonB1Value;
}

uint8_t wasButtonSW1Pressed()
{
    if(g_buttonSW1ChangeRead)
    {
        return 0;
    }
    g_buttonSW1ChangeRead = 1;
    return g_buttonSW1Value;
}

uint8_t wasButtonSW2Pressed()
{
    if(g_buttonSW2ChangeRead)
    {
        return 0;
    }
    g_buttonSW2ChangeRead = 1;
    return g_buttonSW2Value;
}

uint8_t getButtonB1Value()
{
    return g_buttonB1Value;
}

uint8_t getButtonSW1Value()
{
    return g_buttonSW1Value;
}

uint8_t getButtonSW2Value()
{
    return g_buttonSW2Value;
}

void readButtonB1()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_B1) == BUTTON_PRESSED;
    hysteresis(buttonValue, lastInputs, &g_buttonB1Value, &g_buttonB1ChangeRead);
}

void readButtonSW1()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_SW1) == BUTTON_PRESSED;
    hysteresis(buttonValue, lastInputs, &g_buttonSW1Value, &g_buttonSW1ChangeRead);
}

void readButtonSW2()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_SW2) == BUTTON_PRESSED;
    hysteresis(buttonValue, lastInputs, &g_buttonSW2Value, &g_buttonSW2ChangeRead);
}

/***** PRIVATE FUNCTIONS *****************************************************/

void hysteresis(uint8_t input, uint8_t *lastInputs, uint8_t *output, uint8_t *outputChange)
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
