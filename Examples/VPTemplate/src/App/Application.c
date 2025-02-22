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
#include <stdint.h>

#include "Application.h"
#include "Util/Global.h"
#include "Util/printf.h"

#include "UARTModule.h"
#include "ButtonModule.h"
#include "LEDModule.h"
#include "DisplayModule.h"
#include "ButtonService.h"
#include "ADCService.h"

#include "Util/StateTable/StateTable.h"


/***** PRIVATE CONSTANTS *****************************************************/

static const int32_t TICKS_UNTIL_MOTOR_START = 5 * 20; // 5 seconds * 20 Hz
static const int32_t TICKS_UNTIL_VIOLATION_DISPLAY = 3 * 20; // 3 seconds * 20 Hz

/***** PRIVATE MACROS ********************************************************/


/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
// State realte functions (on-Entry, on-State and on-Exit)
static int32_t onEntryBootup(State_t* pState, int32_t eventID);
static int32_t onEntryFailure(State_t* pState, int32_t eventID);
static int32_t onEntryOperational(State_t* pState, int32_t eventID);
static int32_t onStateOperational(State_t* pState, int32_t eventID);
static int32_t onStateMaintenance(State_t* pState, int32_t eventID);

static int32_t getMotorSpeed();
static int32_t getFlowRate();

/***** PRIVATE VARIABLES *****************************************************/
static int8_t s_setFlowRate = -1;
static uint8_t s_displayCycle = 0;
static uint8_t s_manualMotorOverride = 0;
static int32_t s_ticksSinceOperationModeEntered = 0;
static int32_t s_ticksSinceViolation = 0;
//0: off, 1: on, 2: flashing
static uint8_t s_motorState = 0;

//0: off, 1: on
static uint8_t s_sensorLedStatus = 0;

static uint8_t s_motorWarningState = 0;


/**
 * @brief List of State for the State Machine
 *
 * This list only constructs the state objects for each possible state
 * in the state machine. There are no transistions or events defined
 *
 */
static State_t gStateList[] =
{
    {STATE_ID_BOOTUP,       onEntryBootup,  	0,                  0,  false},
    {STATE_ID_FAILURE,      onEntryFailure, 	0,                  0,  false},
    {STATE_ID_MAINTENANCE,  0,              	onStateMaintenance, 0,  false},
    {STATE_ID_OPERATIONAL,  onEntryOperational, onStateOperational, 0,  false},
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
	if(getMotorSpeed() < 0 || getFlowRate() < 0)
	{
		return appSendEvent(EVT_ID_SENSOR_FAILURE);
	}

    return appSendEvent(EVT_ID_SYSTEM_OK);
}

static int32_t onEntryFailure(State_t* pState, int32_t eventID)
{
    if(eventID == EVT_ID_SENSOR_FAILURE)
    {
        ledSetLED(LED0, LED_OFF);
        ledSetLED(LED2, LED_ON);
		ledSetLED(LED3, LED_OFF);
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
		ledSetLED(LED3, LED_OFF);
        ledSetLED(LED4, LED_OFF);
    }
    return STATETBL_ERR_OK;
}

int32_t onEntryOperational(State_t *pState, int32_t eventID)
{
	s_ticksSinceOperationModeEntered = 0;
	s_manualMotorOverride = 0;
	ledSetLED(LED0, LED_ON);
	ledSetLED(LED1, LED_OFF);
	ledSetLED(LED2, LED_OFF);
	ledSetLED(LED3, LED_OFF);
	ledSetLED(LED4, LED_OFF);
	return STATETBL_ERR_OK;
}

static int32_t onStateOperational(State_t* pState, int32_t eventID)
{
	s_ticksSinceOperationModeEntered++;
	int32_t motorSpeed = getMotorSpeed();
	int32_t flowRate = getFlowRate();
	if(motorSpeed < 0 || flowRate < 0)
	{
		ledSetLED(LED4, LED_ON);
	} else {
		ledSetLED(LED4, LED_OFF);
	}

	if(wasButtonB1Pressed())
	{
		appSendEvent(EVT_ID_EVENT_MAINTENANCE);
		return STATETBL_ERR_OK;
	}

	if(wasButtonSW1Pressed())
	{
		s_manualMotorOverride = 1;
	}

	if(wasButtonSW2Pressed())
	{
		s_manualMotorOverride = 0;
	}

	if(s_setFlowRate < 0)
	{
		if(s_displayCycle)
		{
			displayShowDigit(LEFT_DISPLAY, DIGIT_LOWER_O);
		}
		else
		{
			dislpayShowDigit(RIGHT_DISPLAY, DIGIT_LOWER_O);
		}
		s_displayCycle ^= 1;
		// Turn off the motor
		s_motorState = 0;
		ledSetLED(LED3, LED_OFF);
		return STATETBL_ERR_OK;
	}

	if(s_ticksSinceOperationModeEntered >= TICKS_UNTIL_MOTOR_START)
	{
		s_motorState = 2;
	}
	
	if(s_motorState > 0){
		int8_t motorSpeed_10_2 = (motorSpeed / 100) % 10;
		int8_t motorSpeed_10_1 = (motorSpeed / 10) % 10;
		if(s_displayCycle)
		{
			displayShowDigit(LEFT_DISPLAY, motorSpeed_10_2);
		}
		else
		{
			displayShowDigit(RIGHT_DISPLAY, motorSpeed_10_1);
		}
		s_displayCycle ^= 1;
	}


	if(0 < motorSpeed && motorSpeed < 200 && 0 < flowRate && flowRate <= 20){
		s_ticksSinceViolation = 0;
	} else if(200 < motorSpeed && motorSpeed < 400 && 20 < flowRate && flowRate <= 50){
		s_ticksSinceViolation = 0;
	} else if(400 < motorSpeed && motorSpeed < 600 && 50 < flowRate && flowRate <= 75){
		s_ticksSinceViolation = 0;
	} else if(600 < motorSpeed && flowRate <= 80){
		s_ticksSinceViolation = 0;	
	} else {
		s_ticksSinceViolation++;
	}

	static int32_t motorSpeedViolationCounter900 = 0;
	static int32_t motorSpeedViolationCounter700 = 0;
	if(motorSpeed > 900){
		motorSpeedViolationCounter900++;
		motorSpeedViolationCounter700++;
	} else if(motorSpeed > 700){
		motorSpeedViolationCounter900 = 0;
		motorSpeedViolationCounter700++;
		if(s_motorWarningState == 0) {
			s_motorWarningState = 1;
		}
	} else {
		motorSpeedViolationCounter900 = 0;
		motorSpeedViolationCounter700 = 0;
	}

	if(motorSpeedViolationCounter900 > 3 * 20){
		s_motorWarningState = 2;
	}
	
	if(motorSpeed < 650){
		s_motorWarningState = 0;
	} else if(motorSpeed < 800){
		if(s_motorWarningState == 2) {
			s_motorWarningState = 1;
		}
	}

    return STATETBL_ERR_OK;
}

static int32_t onStateMaintenance(State_t* pState, int32_t eventID)
{
	static uint8_t flowRateTensDigit = 0;
	static uint8_t flowRateOneDigit = 0;

	if(s_setFlowRate < 0)
	{
		if(s_displayCycle)
		{
			displayShowDigit(LEFT_DISPLAY, DIGIT_DASH);
		}
		else
		{
			dislpayShowDigit(RIGHT_DISPLAY, DIGIT_DASH);
		}
		s_displayCycle ^= 1;
	}
	else
	{
		flowRateOneDigit = s_setFlowRate % 10;
		flowRateTensDigit = s_setFlowRate / 10;
		flowRateTensDigit = flowRateTensDigit % 10;
		if(s_displayCycle)
		{
			displayShowDigit(LEFT_DISPLAY, flowRateTensDigit);
		}
		else
		{
			displayShowDigit(RIGHT_DISPLAY, flowRateOneDigit);
		}
		s_displayCycle ^= 1;
	}

	if (g_buttonSW1Value == 1 && g_buttonSW2Value == 1)
	{

	}
	else if (g_buttonSW1Value == 1)
	{
		if (s_setFlowRate <= 75)
		{
			s_setFlowRate = s_setFlowRate + 5;
		}
	}
	else if (g_buttonSW2Value == 1)
	{
		if (s_setFlowRate >= 5)
		{
			s_setFlowRate = s_setFlowRate - 5;
		}

	}

	if (g_buttonB1Value == 1)

    return STATETBL_ERR_OK;
}

int32_t getMotorSpeed()
{
	int32_t voltage = getPot1Value();
	if(voltage < 500 || voltage > 2500)
	{
		return -1;
	}
    return (voltage - 500) / 2;
}

int32_t getFlowRate()
{
	int32_t voltage = getPot2Value();
	if(voltage < 500 || voltage > 2500)
	{
		return -1;
	}
    return (voltage - 500) / 25;
}
