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
typedef struct _DisplayValues
{
	int8_t LeftDisplay;
	int8_t RightDisplay;
} DisplayValues;

/***** PROTOTYPES ************************************************************/

void setDisplayValue(DisplayValues DispValues);
void showDisplayValue();


#endif /* SRC_SERVICE_UTIL_DISPLAYSERVICE_H_ */




