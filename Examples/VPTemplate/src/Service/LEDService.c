/******************************************************************************
 * @file LEDService.c
 *
 * @author Lukas Reil
 * @date   08.03.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Service Layer Module for the LEDs
 *
 *****************************************************************************/

/***** INCLUDES **************************************************************/

#include "LEDService.h"





/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/

static LED_VALUE_t s_ledValues[LED4+1] = {0};


/***** PUBLIC FUNCTIONS ******************************************************/

void updateLEDs()
{
    static uint8_t s_blinkCounter = 0;
    s_blinkCounter++;
    if(s_blinkCounter >= LED_BLINK_HALF_PERIOD_CYCLES)
    {
        s_blinkCounter = 0;
        for(uint8_t i = 0; i <= LED4; i++)
        {
            if(s_ledValues[i] == LED_BLINK)
            {
                ledToggleLED(i);
            }
        }
    }
}

void setLEDValue(LED_t led, uint8_t value){
    if(led <= LED4)
    {
        s_ledValues[led] = value;
        if(value == LED_ON)
        {
            ledSetLED(led, LED_ON);
        }
        else if(value == LED_OFF)
        {
            ledSetLED(led, LED_OFF);
        }
    }
}

/***** PRIVATE FUNCTIONS *****************************************************/
