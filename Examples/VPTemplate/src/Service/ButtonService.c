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

// Size of the filter window = 50ms Debounce time / 10ms Task cycle time = 5
//static const uint16_t BUTTON_FILTER_WINDOW_SIZE = 5;
#define BUTTON_FILTER_WINDOW_SIZE 5

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
static void hysteresis(uint8_t input, uint8_t* lastInputs, uint8_t* output, uint8_t *outputChangeRead);

/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/

static uint8_t s_buttonB1Value;
static uint8_t s_buttonB1ChangeRead;
static uint8_t s_buttonSW1Value;
static uint8_t s_buttonSW1ChangeRead;
static uint8_t s_buttonSW2Value;
static uint8_t s_buttonSW2ChangeRead;

void initButtonService()
{
    for(uint8_t i = 0; i < BUTTON_FILTER_WINDOW_SIZE; i++)
    {
        readButtonB1();
        readButtonSW1();
        readButtonSW2();
    }
}

uint8_t wasButtonB1Pressed()
{
    if(s_buttonB1ChangeRead)
    {
        return 0;
    }
    s_buttonB1ChangeRead = 1;
    return s_buttonB1Value;
}

uint8_t wasButtonSW1Pressed()
{
    if(s_buttonSW1ChangeRead)
    {
        return 0;
    }
    s_buttonSW1ChangeRead = 1;
    return s_buttonSW1Value;
}

uint8_t wasButtonSW2Pressed()
{
    if(s_buttonSW2ChangeRead)
    {
        return 0;
    }
    s_buttonSW2ChangeRead = 1;
    return s_buttonSW2Value;
}

uint8_t getButtonB1Value()
{
    return s_buttonB1Value;
}

uint8_t getButtonSW1Value()
{
    return s_buttonSW1Value;
}

uint8_t getButtonSW2Value()
{
    return s_buttonSW2Value;
}

void readButtonB1()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_B1) == BUTTON_PRESSED;
    hysteresis(buttonValue, lastInputs, &s_buttonB1Value, &s_buttonB1ChangeRead);
}

void readButtonSW1()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_SW1) == BUTTON_PRESSED;
    hysteresis(buttonValue, lastInputs, &s_buttonSW1Value, &s_buttonSW1ChangeRead);
}

void readButtonSW2()
{
    static uint8_t lastInputs[BUTTON_FILTER_WINDOW_SIZE];
    uint8_t buttonValue = buttonGetButtonStatus(BTN_SW2) == BUTTON_PRESSED;
    hysteresis(buttonValue, lastInputs, &s_buttonSW2Value, &s_buttonSW2ChangeRead);
}

/***** PRIVATE FUNCTIONS *****************************************************/

static void hysteresis(uint8_t input, uint8_t *lastInputs, uint8_t *output, uint8_t *outputChangeRead)
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
        if(*output == 1)
        {
            *outputChangeRead = 0;
        }
        *output = 0;
    }
    else if(sum == BUTTON_FILTER_WINDOW_SIZE)
    {
        if(*output == 0)
        {
            *outputChangeRead = 0;
        }
        *output = 1;
    }
}
