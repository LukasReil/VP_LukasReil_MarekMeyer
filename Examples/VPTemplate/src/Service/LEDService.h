/******************************************************************************
 * @file LEDService.h
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

#ifndef _LED_SERVICE_H_
#define _LED_SERVICE_H_

#include "LEDModule.h"





/***** INCLUDES **************************************************************/



/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/

#define LED_SERVICE_UPDATE_PERIOD_MS 10
#define LED_BLINK_HALF_PERIOD_CYCLES 50

/***** TYPES *****************************************************************/

typedef enum _LED_Value_t
{
    LED_TURNED_ON  = LED_ON,             //!< Value to turn a LED on
    LED_TURNED_OFF = LED_OFF,           //!< Value to turn a LED off
    LED_BLINKING = 2,                      //!< Value to blink a LED
} LED_Value_t;


/***** PROTOTYPES ************************************************************/

/**
 * @brief   Cyclic function to update the LEDs
 */
void updateLEDs();

/**
 * @brief   Sets the value of the LED
 * 
 * @param led   The LED to set the value for
 * @param value The value to set the LED to
 */
void setLEDValue(LED_t led, LED_Value_t value);



#endif /* _LED_SERVICE_H_ */