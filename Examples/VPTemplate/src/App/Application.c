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
 * @brief Implementation file for main application (state machine)
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include <string.h>

#include "Application.h"
#include "Util/Global.h"
#include "Util/printf.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
//#include "DisplayModule.h"
#include "ButtonService.h"
#include "DisplayService.h"

#include "Util/StateTable/StateTable.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/
#define MAX_FLOW_RATE 80u
#define MIN_FLOW_RATE 0u
#define FLOW_RATE_STEP_SIZE 5u

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
// State realte functions (on-Entry, on-State and on-Exit)
static int32_t onEntryBootup(State_t* pState, int32_t eventID);
static int32_t onEntryFailure(State_t* pState, int32_t eventID);
static int32_t onStateOperational(State_t* pState, int32_t eventID);
static int32_t onStateMaintenance(State_t* pState, int32_t eventID);

/***** PRIVATE VARIABLES *****************************************************/
static int8_t s_setFlowRate = -1;
static uint8_t s_displayCycle = 0b1;




/**
 * @brief List of State for the State Machine
 *
 * This list only constructs the state objects for each possible state
 * in the state machine. There are no transistions or events defined
 *
 */
static State_t gStateList[] =
{
    {STATE_ID_BOOTUP,       onEntryBootup,  0,                  0,  false},
    {STATE_ID_FAILURE,      onEntryFailure, 0,                  0,  false},
    {STATE_ID_MAINTENANCE,  0,              onStateMaintenance, 0,  false},
    {STATE_ID_OPERATIONAL,  0,              onStateOperational, 0,  false},
};

/**
 * @brief Definition of the transistion table of the state machine. Each row
 * contains FROM_STATE_ID, TO_STATE_ID, EVENT_ID, Function Pointer Guard Function
 *
 * The last two members of a transistion row are only the initialization of dynamic
 * members used durin runtim
 */
static StateTableEntry_t gStateTableEntries[] =
{
    {STATE_ID_BOOTUP,          STATE_ID_OPERATIONAL,           EVT_ID_SYSTEM_OK,          0,      0,      0},
    {STATE_ID_BOOTUP,          STATE_ID_FAILURE,               EVT_ID_SENSOR_FAILURE,     0,      0,      0},
    {STATE_ID_OPERATIONAL,     STATE_ID_MAINTENANCE,           EVT_ID_EVENT_MAINTENANCE,  0,      0,      0},
    {STATE_ID_OPERATIONAL,     STATE_ID_FAILURE,               EVT_ID_STACK_OVERFLOW,     0,      0,      0},
    {STATE_ID_OPERATIONAL,     STATE_ID_FAILURE,               EVT_ID_SENSOR_FAILURE,     0,      0,      0},
    {STATE_ID_MAINTENANCE,     STATE_ID_OPERATIONAL,           EVT_ID_EVENT_MAINTENANCE,  0,      0,      0},
    {STATE_ID_MAINTENANCE,     STATE_ID_FAILURE,               EVT_ID_STACK_OVERFLOW,     0,      0,      0},
};

/**
 * @brief Global State Table instance
 *
 */
static StateTable_t gStateTable;


/***** PUBLIC FUNCTIONS ******************************************************/

int32_t appInitialize()
{
    gStateTable.pStateList = gStateList;
    gStateTable.stateCount = sizeof(gStateList) / sizeof(State_t);
    int32_t result = stateTableInitialize(&gStateTable, gStateTableEntries, sizeof(gStateTableEntries) / sizeof(StateTableEntry_t), STATE_ID_BOOTUP);

    return result;
}

int32_t appRunCyclic()
{
    int32_t result = stateTableRunCyclic(&gStateTable);
    return result;
}

int32_t appSendEvent(int32_t eventID)
{
    int32_t result = stateTableSendEvent(&gStateTable, eventID);
    return result;
}


/***** PRIVATE FUNCTIONS *****************************************************/

static int32_t onEntryBootup(State_t* pState, int32_t eventID)
{
    // TODO: Implement the startup sequence

    return appSendEvent(EVT_ID_SYSTEM_OK);
}

static int32_t onEntryFailure(State_t* pState, int32_t eventID)
{
    if(eventID == EVT_ID_SENSOR_FAILURE)
    {
        ledSetLED(LED0, LED_OFF);
        ledSetLED(LED2, LED_ON);
        ledSetLED(LED4, LED_ON);
    }
    else if(eventID == EVT_ID_STACK_OVERFLOW)
    {
        ledSetLED(LED0, LED_ON);
        ledSetLED(LED1, LED_ON);
        ledSetLED(LED2, LED_ON);
        ledSetLED(LED3, LED_ON);
        ledSetLED(LED4, LED_ON);
    }
    else
    {
        ledSetLED(LED0, LED_OFF);
        ledSetLED(LED2, LED_ON);
        ledSetLED(LED4, LED_OFF);
    }
    return STATETBL_ERR_OK;
}

static int32_t onStateOperational(State_t* pState, int32_t eventID)
{

    

    return STATETBL_ERR_OK;
}

/**
 * @brief function to set and show the flow rate
 * @details This function can set the flow rate using the SW1 and SW2 buttons
 * It can also show the selected value and initiate the switch to the Operation Mode
 * @param pState: Pointer to pass on the current state of the State machine
 * @param enventID: variable to notify the function from which state it was called.
 * **/

static int32_t onStateMaintenance(State_t* pState, int32_t eventID)
{
	/* Display aus hinzufügen nach integration */
	DisplayValues DispValues;
	DispValues.RightDisplay = -1;
	DispValues.LeftDisplay = -1;
	static uint8_t flowRateTensDigit = 0;
	static uint8_t flowRateOneDigit = 0;
	/* calculating the Digits according to the flow rate */
	uint8_t flowRateTensDigit = (s_setFlowRate / 10) % 10;
	uint8_t flowRateOneDigit  = s_setFlowRate % 10;

	/* checking and saving whether the SW1 or/and Sw2 were pressed or not */
	uint8_t buttonState_SW1 = wasButtonSW1Pressed();
	uint8_t buttonState_SW2 = wasButtonSW2Pressed();

	/* check whether the flow rate is set or not. */
	if(s_setFlowRate < 0)
	{
		DispValues.RightDisplay = DIGIT_DASH;
		DispValues.LeftDisplay = DIGIT_DASH;
		setDisplayValue(DispValues);
	}
	else
	{
		flowRateOneDigit = s_setFlowRate % 10;
		flowRateTensDigit = s_setFlowRate / 10;
		flowRateTensDigit = flowRateTensDigit % 10;
		DispValues.RightDisplay = flowRateOneDigit;
		DispValues.LeftDisplay = flowRateTensDigit;
		setDisplayValue(DispValues);
	}

	/* check for which button was pressed and increase/decrease the flow rate according to the pressed buttons */
	if (buttonState_SW1)
	{
		/* Increasing the set flow rate if there is enough space from the upper Limit */
		if (s_setFlowRate <= (MAX_FLOW_RATE - FLOW_RATE_STEP_SIZE))
		{
			s_setFlowRate = s_setFlowRate + FLOW_RATE_STEP_SIZE;
		}
	}
	if (buttonState_SW2)
	{
		/* Decreasing the set flow rate if there is enough space from the lower Limit */
		if (s_setFlowRate >= (MIN_FLOW_RATE + FLOW_RATE_STEP_SIZE))
		{
			s_setFlowRate = s_setFlowRate - FLOW_RATE_STEP_SIZE;
		}

	}

	/* check whether the Button 1 was pressed and the system shall switch to the Operation state */
	if (wasButtonB1Pressed())
	{
		appSendEvent(EVT_ID_EVENT_MAINTENANCE);
	}

    return STATETBL_ERR_OK;
}
