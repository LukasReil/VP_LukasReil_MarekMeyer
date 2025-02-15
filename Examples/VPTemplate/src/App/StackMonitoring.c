/*
 * StackMonitoring.c
 *
 *  Created on: Feb 15, 2025
 *      Author: Marek Meyer
 */

/***** INCLUDES **************************************************************/

#include "StackMonitoring.h"

/***** PUBLIC FUNCTIONS ******************************************************/

void initMemoryChecker()
{

}
void cyclic250ms_StackMonitoring()
{
	getFreeBytes();
	getStackValidity();
}
uint32_t getFreeBytes()
{
	uint32_t *pCurrentStackPosition = &_top_of_stack + 4;
	uint32_t *pEndOfStack = &_bottom_of_stack;
	uint16_t freeBytes = 0;

	if (pCurrentStackPosition == 0 || pEndOfStack == 0)
	{
		return -1;
	}

	for (; pCurrentStackPosition != pEndOfStack;)
	{
		if (*pCurrentStackPosition != 0xCDCDCDCD)
		{
			break;
		}
		freeBytes = freeBytes + 4;
		pCurrentStackPosition++;
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
