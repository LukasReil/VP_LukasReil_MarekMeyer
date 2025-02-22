/******************************************************************************
 * @file AppTasks.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com
 * @author Lukas Reil
 * @date   08.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Implementation File for the application tasks
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Scheduler.h"
#include "Application.h"
#include "AppTasks.h"
#include "Service/ADCService.h"
#include "Service/ButtonService.h"

#include "LEDModule.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


void taskApp10ms()
{
    readPot1();
    readPot2();
    readButtonB1();
    readButtonSW1();
    readButtonSW2();
}


void taskApp50ms()
{
    appRunCyclic();
}

void taskApp250ms()
{
    ledToggleLED(LED0);
}


/***** PRIVATE FUNCTIONS *****************************************************/




