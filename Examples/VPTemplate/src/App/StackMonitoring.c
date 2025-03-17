/*
 * StackMonitoring.c
 *
 *  Created on: Feb 15, 2025
 *      Author: Marek Meyer
 */

/***** INCLUDES **************************************************************/

#include "StackMonitoring.h"
#include "LogOutput.h"
#include "Application.h"

#include "Global.h"

/***** PUBLIC FUNCTIONS ******************************************************/

// Return value for failed stack check
const int32_t STACK_CHECK_FAILED = -1; 

void cyclic250ms_StackMonitoring()
{
	int32_t freeBytes = getFreeBytes();
	if(freeBytes == STACK_CHECK_FAILED)
	{
		outputLogf("Stack check failed\n\r");
	} else {
		outputLogf("Free bytes: %d\n\r", freeBytes);
	}

	if(getStackValidity() != 1){
		DEBUG_LOG("Stack is invalid\n\r");
		appSendEvent(EVT_ID_STACK_OVERFLOW);
	}
}

int32_t getFreeBytes()
{
	uint32_t *pCurrentStackPosition = &_top_of_stack + 4;
	uint32_t *pEndOfStack = &_bottom_of_stack;
	uint32_t freeBytes = 0;

	if (pCurrentStackPosition == 0 || pEndOfStack == 0)
	{
		return STACK_CHECK_FAILED;
	}

	for (; pCurrentStackPosition != pEndOfStack; pCurrentStackPosition++)
	{
		if (*pCurrentStackPosition != 0xCDCDCDCD)
		{
			break;
		}
		freeBytes = freeBytes + 4;

	}
	return freeBytes;
}

uint8_t getStackValidity()
{
	uint32_t *pTopOfStack = &_top_of_stack;

	if (pTopOfStack == 0)
	{
		return 0;
	}

	if (*pTopOfStack == 0xABABABAB)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
