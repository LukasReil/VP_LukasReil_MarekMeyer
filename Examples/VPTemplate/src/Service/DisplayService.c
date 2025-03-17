/*
 * DisplayService.c
 *
 *  Created on: Feb 26, 2025
 *      Author: marek
 */
/******************************************************************************
 *
 * @brief <Some short descrition>
 *
 * @details <A more detailed description>
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "DisplayService.h"

/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/
static DisplayValues s_dispValues;

/***** PUBLIC FUNCTIONS ******************************************************/
void setDisplayValue(DisplayValues DispValues)
{
	s_dispValues.LeftDisplay = DispValues.LeftDisplay;
	s_dispValues.RightDisplay = DispValues.RightDisplay;
}

void showDisplayValue()
{
	static uint8_t s_displayCycle = 0b1;
	if(s_displayCycle)
	{
		displayShowDigit(LEFT_DISPLAY, s_dispValues.LeftDisplay);
	}
	else
	{
		displayShowDigit(RIGHT_DISPLAY, s_dispValues.RightDisplay);
	}
	s_displayCycle ^= 1;
}
/***** PRIVATE FUNCTIONS *****************************************************/




