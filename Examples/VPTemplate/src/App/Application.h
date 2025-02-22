/******************************************************************************
 * @file Application.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com
 * @author Lukas Reil
 * @date   08.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Header file for main application (state machine)
 *
 *
 *****************************************************************************/
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

/***** INCLUDES **************************************************************/
#include <stdint.h>

/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/
#define STATE_ID_BOOTUP             1       //!< Initial State
#define STATE_ID_FAILURE            2       //!< Failure State, can be reached by sensor failure or stack overflow
#define STATE_ID_MAINTENANCE        3       //!< Maintenance State, can be toggled by user by Button B1
#define STATE_ID_OPERATIONAL        4       //!< Operational State, reached after successful bootup

#define EVT_ID_SYSTEM_OK            1       //!< Event ID for Successful Bootup
#define EVT_ID_SENSOR_FAILURE       2       //!< Event ID for Sensor Failure
#define EVT_ID_STACK_OVERFLOW       3       //!< Event ID for Stack Overflow
#define EVT_ID_EVENT_MAINTENANCE    4       //!< Event ID for Maintenance Mode

/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/

int32_t appInitialize();

int32_t appRunCyclic();

int32_t appSendEvent(int32_t eventID);

#endif
