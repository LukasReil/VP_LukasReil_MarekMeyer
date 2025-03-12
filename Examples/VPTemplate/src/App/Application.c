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

#include "LEDService.h"
#include "ButtonService.h"
#include "ADCService.h"
#include "DisplayService.h"

#include "Util/StateTable/StateTable.h"

#if LOG_OUTPUT_ENABLED
#include "LogOutput.h"
#endif // LOG_OUTPUT_ENABLED


/***** PRIVATE CONSTANTS *****************************************************/

static const int32_t TICKS_UNTIL_MOTOR_START = 5 * 20; // 5 seconds * 20 Hz
static const int32_t TICKS_UNTIL_VIOLATION_DISPLAY = 3 * 20; // 3 seconds * 20 Hz

/***** PRIVATE MACROS ********************************************************/
#define MAX_FLOW_RATE 80u
#define MIN_FLOW_RATE 0u
#define FLOW_RATE_STEP_SIZE 5u

#define GET_HUNDREDS_DIGIT(x) (((x) / 100) % 10)
#define GET_TENS_DIGIT(x) (((x) / 10) % 10)

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/
// State realte functions (on-Entry, on-State and on-Exit)
static int32_t onEntryBootup(State_t* pState, int32_t eventID);
static int32_t onEntryFailure(State_t* pState, int32_t eventID);
static int32_t onEntryOperational(State_t* pState, int32_t eventID);
static int32_t onStateOperational(State_t* pState, int32_t eventID);
static int32_t onEntryMaintenance(State_t* pState, int32_t eventID);
static int32_t onStateMaintenance(State_t* pState, int32_t eventID);

static int32_t getMotorSpeed();
static int32_t getFlowRate();

/***** PRIVATE VARIABLES *****************************************************/
static int8_t s_setFlowRate = -1;
static uint8_t s_manualMotorOverride = 0;
static int32_t s_ticksSinceOperationModeEntered = 0;
static int32_t s_ticksSinceViolation = 0;
//0: off, 1: on, 2: flashing
static uint8_t s_motorState = 0;

//0: off, 1: on
static uint8_t s_sensorLedStatus = 0;

static uint8_t s_motorWarningState = 0;


//static uint8_t s_displayCycle = 0b1;




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
    {STATE_ID_MAINTENANCE,  onEntryMaintenance, onStateMaintenance, 0,  false},
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

	initADCService();
	initButtonService();

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
		setLEDValue(LED0, LED_TURNED_OFF);
        setLEDValue(LED2, LED_TURNED_ON);
		setLEDValue(LED3, LED_TURNED_OFF);
		setLEDValue(LED4, LED_TURNED_ON);
    }
    else if(eventID == EVT_ID_STACK_OVERFLOW)
    {
		setLEDValue(LED0, LED_TURNED_ON);
		setLEDValue(LED1, LED_TURNED_ON);
		setLEDValue(LED2, LED_TURNED_ON);
		setLEDValue(LED3, LED_TURNED_ON);
		setLEDValue(LED4, LED_TURNED_ON);
    }
    else
    {
		setLEDValue(LED0, LED_TURNED_OFF);
		setLEDValue(LED2, LED_TURNED_ON);
		setLEDValue(LED3, LED_TURNED_OFF);
		setLEDValue(LED4, LED_TURNED_OFF);
    }

    return STATETBL_ERR_OK;
}

int32_t onEntryOperational(State_t *pState, int32_t eventID)
{
	s_ticksSinceOperationModeEntered = 0;
	s_manualMotorOverride = 0;
	s_motorState = 0;
	setLEDValue(LED0, LED_TURNED_ON);
	setLEDValue(LED1, LED_TURNED_OFF);
	setLEDValue(LED2, LED_TURNED_OFF);
	setLEDValue(LED3, LED_TURNED_OFF);
	setLEDValue(LED4, LED_TURNED_OFF);
	return STATETBL_ERR_OK;
}

static int32_t onStateOperational(State_t* pState, int32_t eventID)
{
	s_ticksSinceOperationModeEntered++;
	int32_t motorSpeed = getMotorSpeed();
	int32_t flowRate = getFlowRate();
	if(motorSpeed < 0 || flowRate < 0)
	{
		return appSendEvent(EVT_ID_SENSOR_FAILURE);
	}

	if(wasButtonB1Pressed())
	{
		return appSendEvent(EVT_ID_EVENT_MAINTENANCE);
	}

	if(s_setFlowRate < 0)
	{
		DisplayValues dispValues;
		dispValues.RightDisplay = DIGIT_LOWER_O;
		dispValues.LeftDisplay = DIGIT_LOWER_O;
		setDisplayValue(dispValues);
		// Turn off the motor
		s_motorState = 0;
		setLEDValue(LED3, LED_TURNED_OFF);
		return STATETBL_ERR_OK;
	}

	DisplayValues dispValues;
	dispValues.RightDisplay = GET_TENS_DIGIT(motorSpeed);
	dispValues.LeftDisplay = GET_HUNDREDS_DIGIT(motorSpeed);
	setDisplayValue(dispValues);

	if(wasButtonSW1Pressed())
	{
		s_manualMotorOverride = 1;
	}

	if(wasButtonSW2Pressed())
	{
		s_manualMotorOverride = 0;
	}

	if(s_ticksSinceOperationModeEntered >= TICKS_UNTIL_MOTOR_START)
	{
		s_motorState = 2;
	}

	static int32_t motorSpeedViolationCounter900 = 0;
	static int32_t motorSpeedViolationCounter800 = 0;
	static int32_t motorSpeedViolationCounter700 = 0;
	static int32_t motorSpeedViolationCounter650 = 0;

	if(s_motorState > 0 && s_manualMotorOverride == 0)
	{
		if(0 < motorSpeed && motorSpeed < 200){
			if(0 < flowRate && flowRate <= 20){
				s_ticksSinceViolation = 0;
			} else {
				s_ticksSinceViolation++;
			}
			s_ticksSinceViolation = 0;
		} else if(200 < motorSpeed && motorSpeed < 400){
			if(20 < flowRate && flowRate <= 50){
				s_ticksSinceViolation = 0;
			} else {
				s_ticksSinceViolation++;
			}
		} else if(400 < motorSpeed && motorSpeed < 600){
			if(50 < flowRate && flowRate <= 75){
				s_ticksSinceViolation = 0;
			} else {
				s_ticksSinceViolation++;
			}
		} else if(600 < motorSpeed){
			if(flowRate <= 80){
				s_ticksSinceViolation = 0;
			} else {
				s_ticksSinceViolation++;
			}
		} else {
			s_ticksSinceViolation = 0;
		}

		uint8_t monitoringViolation = 0;
		if(s_ticksSinceViolation >= TICKS_UNTIL_VIOLATION_DISPLAY){

			monitoringViolation = 1;
		}

		if(motorSpeed > 900){
			motorSpeedViolationCounter900++;
			motorSpeedViolationCounter700++;
		} else if(motorSpeed > 700){
			motorSpeedViolationCounter900 = 0;
			motorSpeedViolationCounter700++;
		} else {
			motorSpeedViolationCounter900 = 0;
			motorSpeedViolationCounter700 = 0;
		}
		if(motorSpeed < 650){
			motorSpeedViolationCounter650++;
		} else if(motorSpeed < 800){
			motorSpeedViolationCounter800++;
			motorSpeedViolationCounter650 = 0;
		} else {
			motorSpeedViolationCounter650 = 0;
			motorSpeedViolationCounter800 = 0;
		}

		if (motorSpeedViolationCounter700 > 5 * 20 && s_motorWarningState == 0)
		{
			s_motorWarningState = 1;
		}
		if(motorSpeedViolationCounter900 > 3 * 20)
		{
			s_motorWarningState = 2;
		}
		if(motorSpeedViolationCounter800 > 3 * 20 && s_motorWarningState == 2)
		{
			s_motorWarningState = 1;
		}
		if(motorSpeedViolationCounter650 > 3 * 20 && s_motorWarningState == 1)
		{
			s_motorWarningState = 0;
		}
		
		uint8_t worstViolation = s_motorWarningState > monitoringViolation ? s_motorWarningState : monitoringViolation;
		switch(worstViolation)
		{
			case 0:
				setLEDValue(LED1, LED_TURNED_OFF);
				break;
			case 1:
				setLEDValue(LED1, LED_TURNED_ON);
				break;
			case 2:
				setLEDValue(LED1, LED_BLINKING);
				break;
		}

		if(flowRate >= s_setFlowRate && worstViolation == 0){
			setLEDValue(LED3, LED_TURNED_ON);
		} else {
			setLEDValue(LED3, LED_BLINKING);
		}
	} else {
		setLEDValue(LED3, LED_TURNED_OFF);
	}

	


    return STATETBL_ERR_OK;
}

static void clutterStack()
{
	int32_t stack[8000];
	for(int i = 0; i < 8000; i++)
	{
		stack[i] = i;
	}
}

static int32_t onEntryMaintenance(State_t* pState, int32_t eventID)
{
	setLEDValue(LED0, LED_BLINKING);
	setLEDValue(LED3, LED_TURNED_OFF);
	return STATETBL_ERR_OK;
}

/**
 * @brief function to set and show the flow rate
 * @details This function can set the flow rate using the SW1 and SW2 buttons
 * It can also show the selected value and initiate the switch to the Operation Mode
 * @param pState: Pointer to pass on the current state of the State machine
 * @param enventID: variable to notify the function from which state it was called.
 * 
**/
static int32_t onStateMaintenance(State_t* pState, int32_t eventID)
{
	DisplayValues DispValues;
	DispValues.RightDisplay = DIGIT_OFF;
	DispValues.LeftDisplay = DIGIT_OFF;
	static uint8_t flowRateTensDigit = 0;
	static uint8_t flowRateOneDigit = 0;
	/* calculating the Digits according to the flow rate */
	flowRateTensDigit = (s_setFlowRate / 10) % 10;
	flowRateOneDigit  = s_setFlowRate % 10;

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
	else if (s_setFlowRate % 5 != 0)
	{
		s_setFlowRate = 0;
	}
	else
	{
		flowRateOneDigit = s_setFlowRate % 10;
		flowRateTensDigit = (s_setFlowRate / 10) % 10;
		DispValues.RightDisplay = flowRateOneDigit;
		DispValues.LeftDisplay = flowRateTensDigit;
		setDisplayValue(DispValues);
	}

	/* check for which button was pressed and increase/decrease the flow rate according to the pressed buttons */
	if (buttonState_SW1)
	{
		/* Increasing the set flow rate if there is enough space from the upper Limit */
		if (s_setFlowRate <= (int8_t) (MAX_FLOW_RATE - FLOW_RATE_STEP_SIZE))
		{
			s_setFlowRate = s_setFlowRate + FLOW_RATE_STEP_SIZE;
		}
	}
	if (buttonState_SW2)
	{
		/* Decreasing the set flow rate if there is enough space from the lower Limit */
		if (s_setFlowRate >= (int8_t) (MIN_FLOW_RATE + FLOW_RATE_STEP_SIZE))
		{
			s_setFlowRate = s_setFlowRate - FLOW_RATE_STEP_SIZE;
		}

	}

	/* check whether the Button 1 was pressed and the system shall switch to the Operation state */
	if (wasButtonB1Pressed())
	{
		return appSendEvent(EVT_ID_EVENT_MAINTENANCE);
	}

	if(getButtonSW1Value() && getButtonSW2Value())
	{
		clutterStack();
	}

	if (getMotorSpeed() < 0 || getFlowRate() < 0)
	{
		setLEDValue(LED4, LED_TURNED_ON);
	}
	else
	{
		setLEDValue(LED4, LED_TURNED_OFF);
	}

    return STATETBL_ERR_OK;
}

int32_t getMotorSpeed()
{
	int32_t voltage = getPot1Value();
	if(voltage < 500000 || voltage > 2500000)
	{
		return -1;
	}
    return (voltage - 500000) / 2000;
}

int32_t getFlowRate()
{
	int32_t voltage = getPot2Value();
	if(voltage < 500000 || voltage > 2500000)
	{
		return -1;
	}
    return (voltage - 500000) / 25000;
}
