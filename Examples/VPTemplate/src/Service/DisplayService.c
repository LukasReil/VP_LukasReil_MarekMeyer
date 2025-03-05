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
static DisplayValues i_DispValues;

/***** PUBLIC FUNCTIONS ******************************************************/
void setDisplayValue(DisplayValues DispValues)
{
	i_DispValues.LeftDisplay = DispValues.LeftDisplay;
	i_DispValues.RightDisplay = DispValues.RightDisplay;
}

void showDisplayValue()
{
	static uint8_t s_displayCycle = 0b1;
	if(s_displayCycle)
	{
		displayShowDigit(LEFT_DISPLAY, i_DispValues.LeftDisplay);
	}
	else
	{
		displayShowDigit(RIGHT_DISPLAY, i_DispValues.RightDisplay);
	}
	s_displayCycle ^= 1;
}
/***** PRIVATE FUNCTIONS *****************************************************/




