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

const extern uint32_t _top_of_stack;
const extern uint32_t _bottom_of_stack;
const extern uint32_t _size_of_stack;


/***** PROTOTYPES ************************************************************/

void initMemoryChecker();
void cyclic250ms_StackMonitoring();
uint32_t getFreeBytes();
uint8_t getStackValidity();



#endif /* SRC_APP_STACKMONITORING_H_ */
