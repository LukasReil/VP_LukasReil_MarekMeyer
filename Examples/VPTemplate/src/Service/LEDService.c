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

static const uint8_t LED_SERVICE_UPDATE_PERIOD_MS = 10;
static const uint8_t LED_BLINK_HALF_PERIOD_CYCLES = LED_SERVICE_UPDATE_PERIOD_MS * 10 / 2;

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/

static LED_Value_t s_ledValues[LED4+1] = {0};


/***** PUBLIC FUNCTIONS ******************************************************/


void updateLEDs()
{
    static uint8_t s_blinkCounter = 0;
    s_blinkCounter++;
    if(s_blinkCounter >= LED_BLINK_HALF_PERIOD_CYCLES)
    {
        s_blinkCounter -= LED_BLINK_HALF_PERIOD_CYCLES;
        for(uint8_t i = 0; i <= LED4; i++)
        {
            if(s_ledValues[i] == LED_BLINKING)
            {
                ledToggleLED(i);
            }
        }
    }
}

void setLEDValue(LED_t led, LED_Value_t value){
    if(led <= LED4)
    {
        s_ledValues[led] = value;
        if(value == LED_TURNED_ON)
        {
            ledSetLED(led, LED_ON);
        }
        else if(value == LED_TURNED_OFF)
        {
            ledSetLED(led, LED_OFF);
        }
    }
}

/***** PRIVATE FUNCTIONS *****************************************************/
