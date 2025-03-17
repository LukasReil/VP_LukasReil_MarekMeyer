/******************************************************************************
 * @file AppTasks.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com
 * @date   08.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Header File for the application tasks
 *
 *
 *****************************************************************************/
#ifndef _APPTASKS_H_
#define _APPTASKS_H_


/***** INCLUDES **************************************************************/
#include <stdint.h>

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/


/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

/**
 * @brief Task for the 10ms cyclic event
 *        Does:           
 *           - read Potentiometer 1
 *           - read Potentiometer 2
 *           - read Button B1
 *           - read Button SW1
 *           - read Button SW2
 *           - show Display Value
 *           - update LEDs
 */
void taskApp10ms();

/**
 * @brief Task for the 50ms cyclic event
 *        Does:
 *          - main Application Task
 */
void taskApp50ms();

/**
 * @brief Task for the 100ms cyclic event
 *        Does:
 *          - Stack Monitoring
 */
void taskApp250ms();

#endif /* SRC_APP_APPTASKS_H_ */
