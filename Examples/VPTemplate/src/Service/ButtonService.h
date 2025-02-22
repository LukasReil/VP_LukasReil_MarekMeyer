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

#define BUTTON_FILTER_WINDOW_SIZE 5

/***** MACROS ****************************************************************/


/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

/**
 * @return 1 if the button B1 was pressed and this is the first time the function is called
 *         0 otherwise
 */
uint8_t wasButtonB1Pressed();

/**
 * @return 1 if the button SW1 was pressed and this is the first time the function is called
 *         0 otherwise
 */
uint8_t wasButtonSW1Pressed();

/**
 * @return 1 if the button SW2 was pressed and this is the first time the function is called
 *         0 otherwise
 */
uint8_t wasButtonSW2Pressed();

/**
 * @return The value of the button B1
 */
uint8_t getButtonB1Value();

/**
 * @return The value of the button SW1
 */
uint8_t getButtonSW1Value();

/**
 * @return The value of the button SW2
 */
uint8_t getButtonSW2Value();

/**
 * @brief   Reads the value of B1 and debounces it using a two-point controller
 *          The filtered value is stored in the global variable g_buttonB1Value
 */
void readButtonB1();

/**
 * @brief   Reads the value of SW1 and debounces it using a two-point controller
 *          The filtered value is stored in the global variable g_buttonSW1Value
 */
void readButtonSW1();

/**
 * @brief   Reads the value of SW2 and debounces it using a two-point controller
 *          The filtered value is stored in the global variable g_buttonSW2Value
 */
void readButtonSW2();



#endif // _BUTTON_SERVICE_H
