/*
 * DisplayService.h
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





#ifndef SRC_SERVICE_UTIL_DISPLAYSERVICE_H_
#define SRC_SERVICE_UTIL_DISPLAYSERVICE_H_

/***** INCLUDES **************************************************************/
#include <stdint.h>
#include "DisplayModule.h"

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/


/***** TYPES *****************************************************************/
/**
 * @brief struct to hold the Display Values for both displays
 */
typedef struct _DisplayValues
{
	int8_t LeftDisplay;
	int8_t RightDisplay;
} DisplayValues;

/***** PROTOTYPES ************************************************************/

/**
 * @brief function to set the internal Display Values
 * @param DispValues struct to pass on the Display Values for both displays.
 */
void setDisplayValue(DisplayValues DispValues);

/**
 *  @brief function to show the set Display Values
 */
void showDisplayValue();


#endif /* SRC_SERVICE_UTIL_DISPLAYSERVICE_H_ */




