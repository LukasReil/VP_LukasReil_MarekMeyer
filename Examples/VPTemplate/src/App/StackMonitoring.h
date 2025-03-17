/*
 * StackMonitoring.h
 *
 *  Created on: Feb 15, 2025
 *      Author: Marek Meyer
 */



#ifndef SRC_APP_STACKMONITORING_H_
#define SRC_APP_STACKMONITORING_H_

/***** INCLUDES **************************************************************/

#include "stdint.h"


/***** CONSTANTS *************************************************************/

// Constants from the linker script
extern uint32_t _top_of_stack;
extern uint32_t _bottom_of_stack;
extern uint32_t _size_of_stack;

const extern int32_t STACK_CHECK_FAILED;


/***** PROTOTYPES ************************************************************/

/**
 * @brief Cyclic function to monitor the stack usage
 */
void cyclic250ms_StackMonitoring();

/**
 * @brief Function to get the free bytes on the stack
 * 
 * @return 	the number of free bytes on the stack
 * 			STACK_CHECK_FAILED if the stack pointers are invalid
 */
int32_t getFreeBytes();

/**
 * @brief Function to check the validity of the stack
 * 
 * @return 	1 if the stack is valid
 * 			0 if the stack is invalid
 */
uint8_t getStackValidity();



#endif /* SRC_APP_STACKMONITORING_H_ */
