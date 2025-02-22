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

/***** PUBLIC FUNCTIONS ******************************************************/

void initMemoryChecker()
{

}

void cyclic250ms_StackMonitoring()
{
	uint32_t freeBytes = getFreeBytes();
	outputLogf("Free bytes: %d\n", freeBytes);

	if(getStackValidity() != 1){
		appSendEvent(EVT_ID_STACK_OVERFLOW);
	}
}

uint32_t getFreeBytes()
{
	uint32_t *pCurrentStackPosition = &_top_of_stack + 4;
	uint32_t *pEndOfStack = &_bottom_of_stack;
	uint32_t freeBytes = 0;

	if (pCurrentStackPosition == 0 || pEndOfStack == 0)
	{
		return -1;
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
