/******************************************************************************
 * @file ButtonService.h
 *
 * @author Lukas Reil
 * @date   22.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Service Layer Module for the Buttons
 *
 * @details Contains functions to read the values of the buttons and filter them
 *
 *
 *****************************************************************************/
#ifndef _BUTTON_SERVICE_H
#define _BUTTON_SERVICE_H


/***** INCLUDES **************************************************************/

#include <stdint.h>

/***** CONSTANTS *************************************************************/

const uint32_t BUTTON_FILTER_WINDOW_SIZE = 5;

/***** MACROS ****************************************************************/


/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

uint8_t g_buttonB1Value;
uint8_t g_buttonSW1Value;
uint8_t g_buttonSW2Value;

/**
 * @brief   Reads the value of B1 and debounces it using a two-point controller
 *          The filtered value is stored in the global variable g_buttonB1Value
 * @return  uint8_t Filtered value of the B1
 */
uint8_t readButtonB1();

/**
 * @brief   Reads the value of SW1 and debounces it using a two-point controller
 *          The filtered value is stored in the global variable g_buttonSW1Value
 * @return  uint8_t Filtered value of the SW1
 */
uint8_t readButtonSW1();

/**
 * @brief   Reads the value of SW2 and debounces it using a two-point controller
 *          The filtered value is stored in the global variable g_buttonSW2Value
 * @return  uint8_t Filtered value of the SW2
 */
uint8_t readButtonSW2();



#endif // _BUTTON_SERVICE_H
