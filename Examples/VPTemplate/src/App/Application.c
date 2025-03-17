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





/***** PRIVATE MACROS ********************************************************/

#define GET_HUNDREDS_DIGIT(x) (((x) / 100) % 10)
#define GET_TENS_DIGIT(x) (((x) / 10) % 10)
#define GET_ONES_DIGIT(x) ((x) % 10)


/***** PRIVATE TYPES *********************************************************/

/**
 * @brief Enumeration to track monitoring violations
 */
typedef enum {
	NO_MONITORING_VIOLATION = 0,
	MONITORING_VIOLATION = 1,
	CRITICAL_MONITORING_VIOLATION = 2,
} MonitoringViolation;

/**
 * @brief Enumeration to track the state of the motor
 */
typedef enum {
	MOTOR_OFF = 0,
	MOTOR_ON = 1,
} MotorState;

/**
 * @brief Function pointer type for the motor range check
 */
typedef uint8_t (*MotorRangeCheck)(int32_t);
/**
 * @brief Function pointer type for the motor speed violation check
 */
typedef uint8_t (*MotorSpeedViolationCheck)(int32_t);

/**
 * @brief Structure to hold the function pointers for the motor range and speed violation checks
 */
typedef struct {
	MotorRangeCheck motorRangeCheck;
	MotorSpeedViolationCheck motorSpeedViolationCheck;
} MotorRangeViolationCheck;


/***** PRIVATE CONSTANTS *****************************************************/

static const int32_t TICKS_FOR_1_SECOND = 20;
static const int32_t TICKS_FOR_3_SECONDS = 3 * TICKS_FOR_1_SECOND;
static const int32_t TICKS_FOR_5_SECONDS = 5 * TICKS_FOR_1_SECOND;

static const int32_t TICKS_UNTIL_MOTOR_START = TICKS_FOR_5_SECONDS;
static const int32_t TICKS_UNTIL_VIOLATION_DISPLAY = TICKS_FOR_3_SECONDS;

static const int32_t TICKS_UNTIL_LIMIT_1_WARNING = TICKS_FOR_5_SECONDS;
static const int32_t TICKS_UNTIL_LIMIT_2_WARNING = TICKS_FOR_3_SECONDS;
static const int32_t TICKS_UNTIL_LIMIT_WARNING_RESOLVE = TICKS_FOR_3_SECONDS;

const extern int32_t _size_of_stack;
static const size_t STACK_SIZE = (size_t) &_size_of_stack;

static const int32_t SENSOR_MIN_VOLTAGE = 500000;
static const int32_t SENSOR_MAX_VOLTAGE = 2500000;
static const int32_t RPM_PER_MICROVOLT = 2000;
static const int32_t FLOW_PER_MICROVOLT = 25000;

static const int32_t MAX_FLOW_RATE = 80;
static const int32_t MIN_FLOW_RATE = 0;
static const int32_t FLOW_RATE_STEP_SIZE = 5;

static const int32_t MIN_MOTOR_SPEED = 0;
static const int32_t MOTOR_SPEED_STEP_1 = 200;
static const int32_t MOTOR_SPEED_STEP_2 = 400;
static const int32_t MOTOR_SPEED_STEP_3 = 600;

static const int32_t FLOW_RATE_STEP_1 = 20;
static const int32_t FLOW_RATE_STEP_2 = 50;
static const int32_t FLOW_RATE_STEP_3 = 75;

static const int32_t MOTOR_SPEED_LIMIT_1 = 700;
static const int32_t MOTOR_SPEED_LIMIT_1_WARNING_RESOLVE = 650;
static const int32_t MOTOR_SPEED_LIMIT_2 = 900;
static const int32_t MOTOR_SPEED_LIMIT_2_WARNING_RESOLVE = 800;

static const int8_t INVALID_FLOW_RATE = -1;

// Function prototypes for the motor range and speed violation checks
static uint8_t motorRange0(int32_t motorSpeed);
static uint8_t motorRange1(int32_t motorSpeed);
static uint8_t motorRange2(int32_t motorSpeed);
static uint8_t motorRange3(int32_t motorSpeed);

static uint8_t motorSpeedViolation0(int32_t flowRate);
static uint8_t motorSpeedViolation1(int32_t flowRate);
static uint8_t motorSpeedViolation2(int32_t flowRate);
static uint8_t motorSpeedViolation3(int32_t flowRate);

static const MotorRangeViolationCheck motorRangeViolationCheck[] = {
	{motorRange0, motorSpeedViolation0},
	{motorRange1, motorSpeedViolation1},
	{motorRange2, motorSpeedViolation2},
	{motorRange3, motorSpeedViolation3}
};


static const size_t motorRangeViolationCheckSize = sizeof(motorRangeViolationCheck) / sizeof(MotorRangeViolationCheck);

/***** PRIVATE PROTOTYPES ****************************************************/
// State realte functions (on-Entry, on-State and on-Exit)
static int32_t onEntryBootup(State_t* pState, int32_t eventID);
static int32_t onEntryFailure(State_t* pState, int32_t eventID);
static int32_t onEntryOperational(State_t* pState, int32_t eventID);
static int32_t onStateOperational(State_t* pState, int32_t eventID);
static int32_t onEntryMaintenance(State_t* pState, int32_t eventID);
static int32_t onStateMaintenance(State_t* pState, int32_t eventID);


/**
 * @brief Checks the motor speed for violations, implements a hysteresis according to requirements on page 13
 * 
 * @param motorSpeed 	the motor speed in rpm
 * @return MonitoringViolation 	the violation state
 */
static MonitoringViolation checkMotorSpeed(int32_t motorSpeed);

/**
 * @brief Checks the relation between motor speed and flow rate for violations according to requirements on page 14
 * 
 * @param motorSpeed 	the motor speed in rpm
 * @param flowRate 		the flow rate in l/h
 * @return MonitoringViolation 	the violation state
 */
static MonitoringViolation checkMotorSpeedFlowRateRelation(int32_t motorSpeed, int32_t flowRate);

/**
 * @brief Reads the ADC value of the motor speed sensor and converts it to rpm
 * 
 * @return 	the motor speed in rpm
 * 			-1 if the sensor value is out of range
 */
static int32_t getMotorSpeed();

/**
 * @brief Reads the ADC value of the flow rate sensor and converts it to l/h
 * 
 * @return 	the flow rate in l/h
 * 			-1 if the sensor value is out of range
 */
static int32_t getFlowRate();

/**
 * @brief Clutters the stack with a local variable and thus causes a stack corruption
 */
static void clutterStack();

/***** PRIVATE VARIABLES *****************************************************/
static int8_t s_setFlowRate = INVALID_FLOW_RATE;
static uint8_t s_manualMotorOverride = 0;
static int32_t s_ticksSinceOperationModeEntered = 0;
static int32_t s_ticksSinceViolation = 0;

static MotorState s_motorState = MOTOR_OFF;






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
static StateTableEntry_t s_stateTableEntries[] =
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
    int32_t result = stateTableInitialize(&gStateTable, s_stateTableEntries, sizeof(s_stateTableEntries) / sizeof(StateTableEntry_t), STATE_ID_BOOTUP);

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



static MonitoringViolation checkMotorSpeedFlowRateRelation(int32_t motorSpeed, int32_t flowRate)
{


	static int32_t lastMotorSpeedViolation = -1;

	for (size_t i = 0; i <= motorRangeViolationCheckSize; i++)
	{
		if(i == motorRangeViolationCheckSize)
		{
			lastMotorSpeedViolation = -1;
			s_ticksSinceViolation = 0;
			break;
		}

		if(motorRangeViolationCheck[i].motorRangeCheck(motorSpeed))
		{
			if(motorRangeViolationCheck[i].motorSpeedViolationCheck(flowRate))
			{
				s_ticksSinceViolation = 0;
			} 
			else 
			{
				if(lastMotorSpeedViolation != i)
				{
					DEBUG_LOGF("Flow rate violation detected: %d rpm, %d l/h\n\r", motorSpeed, flowRate);
				}
				lastMotorSpeedViolation = i;
				s_ticksSinceViolation++;
			}
			break;
		}
	}

	if(s_ticksSinceViolation >= TICKS_UNTIL_VIOLATION_DISPLAY)
	{
		return MONITORING_VIOLATION;
	}
	return NO_MONITORING_VIOLATION;
}

static MonitoringViolation checkMotorSpeed(int32_t motorSpeed)
{
	
	static int32_t motorSpeedLimit1ViolationCounter = 0;
	static int32_t motorSpeedLimit1HysteresisThresholdCounter = 0;
	static int32_t motorSpeedLimit2ViolationCounter = 0;
	static int32_t motorSpeedLimit2HysteresisThresholdCounter = 0;

	static MonitoringViolation motorWarningState = NO_MONITORING_VIOLATION;

	if(motorSpeed > MOTOR_SPEED_LIMIT_2)
	{
		if(motorSpeedLimit2ViolationCounter == 0)
		{
			DEBUG_LOGF("Motor speed exceedes Limit 2: %d rpm\n\r", motorSpeed);
		}
		motorSpeedLimit1ViolationCounter++;
		motorSpeedLimit2ViolationCounter++;
	} 
	else if(motorSpeed > MOTOR_SPEED_LIMIT_1)
	{
		if(motorSpeedLimit1ViolationCounter == 0)
		{
			DEBUG_LOGF("Motor speed exceedes Limit 1: %d rpm\n\r", motorSpeed);
		}
		motorSpeedLimit2ViolationCounter = 0;
		motorSpeedLimit1ViolationCounter++;
	} 
	else 
	{
		motorSpeedLimit1ViolationCounter = 0;
		motorSpeedLimit2ViolationCounter = 0;
	}

	if(motorSpeed < MOTOR_SPEED_LIMIT_1_WARNING_RESOLVE)
	{
		motorSpeedLimit1HysteresisThresholdCounter++;
	} 
	else if(motorSpeed < MOTOR_SPEED_LIMIT_2_WARNING_RESOLVE)
	{
		motorSpeedLimit2HysteresisThresholdCounter++;
		motorSpeedLimit1HysteresisThresholdCounter = 0;
	} 
	else 
	{
		motorSpeedLimit1HysteresisThresholdCounter = 0;
		motorSpeedLimit2HysteresisThresholdCounter = 0;
	}

	if (motorSpeedLimit1ViolationCounter > TICKS_UNTIL_LIMIT_1_WARNING && motorWarningState == NO_MONITORING_VIOLATION)
	{
		motorWarningState = MONITORING_VIOLATION;
	}

	if(motorSpeedLimit2ViolationCounter > TICKS_UNTIL_LIMIT_2_WARNING)
	{
		motorWarningState = CRITICAL_MONITORING_VIOLATION;
	}

	if(motorSpeedLimit2HysteresisThresholdCounter > TICKS_UNTIL_LIMIT_WARNING_RESOLVE && motorWarningState == CRITICAL_MONITORING_VIOLATION)
	{
		motorWarningState = MONITORING_VIOLATION;
	}

	if(motorSpeedLimit1HysteresisThresholdCounter > TICKS_UNTIL_LIMIT_WARNING_RESOLVE && motorWarningState == MONITORING_VIOLATION)
	{
		motorWarningState = NO_MONITORING_VIOLATION;
	}

	return motorWarningState;
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

	if(s_setFlowRate == INVALID_FLOW_RATE)
	{
		DisplayValues dispValues;
		dispValues.RightDisplay = DIGIT_LOWER_O;
		dispValues.LeftDisplay = DIGIT_LOWER_O;
		setDisplayValue(dispValues);
		// Turn off the motor
		s_motorState = MOTOR_OFF;
		setLEDValue(LED3, LED_TURNED_OFF);
		return STATETBL_ERR_OK;
	}

	DisplayValues dispValues;
	dispValues.RightDisplay = GET_TENS_DIGIT(motorSpeed);
	dispValues.LeftDisplay = GET_HUNDREDS_DIGIT(motorSpeed);
	setDisplayValue(dispValues);

	if(wasButtonSW1Pressed())
	{
		s_manualMotorOverride = true;
	}

	if(wasButtonSW2Pressed())
	{
		s_manualMotorOverride = false;
	}

	if(s_ticksSinceOperationModeEntered >= TICKS_UNTIL_MOTOR_START)
	{
		s_motorState = MOTOR_ON;
	}

	if(s_motorState != MOTOR_OFF && !s_manualMotorOverride)
	{
		
		MonitoringViolation monitoringViolation1 = checkMotorSpeedFlowRateRelation(motorSpeed, flowRate);
		MonitoringViolation monitoringViolation2 = checkMotorSpeed(motorSpeed);

		// Choose the biggest violation
		MonitoringViolation worstViolation = monitoringViolation1 > monitoringViolation2 ? monitoringViolation1 : monitoringViolation2;
		switch(worstViolation)
		{
			case NO_MONITORING_VIOLATION:
				setLEDValue(LED1, LED_TURNED_OFF);
				break;
			case MONITORING_VIOLATION:
				setLEDValue(LED1, LED_TURNED_ON);
				break;
			case CRITICAL_MONITORING_VIOLATION:
				setLEDValue(LED1, LED_BLINKING);
				break;
		}

		if(flowRate >= s_setFlowRate && worstViolation == NO_MONITORING_VIOLATION)
		{
			setLEDValue(LED3, LED_TURNED_ON);
		} 
		else 
		{
			setLEDValue(LED3, LED_BLINKING);
		}
	} 
	else 
	{
		setLEDValue(LED3, LED_TURNED_OFF);
	}

	


    return STATETBL_ERR_OK;
}

static void clutterStack()
{
	int32_t stack[STACK_SIZE];
	for(int i = 0; i < STACK_SIZE; i++)
	{
		stack[i] = i;
	}
	//Suppress unused variable warning
	UNUSED(stack);
}

static int32_t onEntryMaintenance(State_t* pState, int32_t eventID)
{
	setLEDValue(LED0, LED_BLINKING);
	setLEDValue(LED1, LED_TURNED_OFF);
	setLEDValue(LED2, LED_TURNED_OFF);
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
	flowRateTensDigit = GET_TENS_DIGIT(s_setFlowRate);
	flowRateOneDigit  = GET_ONES_DIGIT(s_setFlowRate);

	/* checking and saving whether the SW1 or/and Sw2 were pressed or not */
	uint8_t buttonState_SW1 = wasButtonSW1Pressed();
	uint8_t buttonState_SW2 = wasButtonSW2Pressed();

	/* check whether the flow rate is set or not. */
	if(s_setFlowRate == INVALID_FLOW_RATE)
	{
		DispValues.RightDisplay = DIGIT_DASH;
		DispValues.LeftDisplay = DIGIT_DASH;
		setDisplayValue(DispValues);
	}
	else if (s_setFlowRate % FLOW_RATE_STEP_SIZE != 0)
	{
		s_setFlowRate = 0;
	}
	else
	{
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


static uint8_t motorRange0(int32_t motorSpeed)
{
	return MIN_MOTOR_SPEED < motorSpeed && motorSpeed <= MOTOR_SPEED_STEP_1;
}

static uint8_t motorRange1(int32_t motorSpeed)
{
	return MOTOR_SPEED_STEP_1 < motorSpeed && motorSpeed <= MOTOR_SPEED_STEP_2;
}

static uint8_t motorRange2(int32_t motorSpeed)
{
	return MOTOR_SPEED_STEP_2 < motorSpeed && motorSpeed <= MOTOR_SPEED_STEP_3;
}

static uint8_t motorRange3(int32_t motorSpeed)
{
	return MOTOR_SPEED_STEP_3 < motorSpeed;
}


static uint8_t motorSpeedViolation0(int32_t flowRate)
{
	return MIN_FLOW_RATE < flowRate && flowRate <= FLOW_RATE_STEP_1;
}

static uint8_t motorSpeedViolation1(int32_t flowRate)
{
	return FLOW_RATE_STEP_1 < flowRate && flowRate <= FLOW_RATE_STEP_2;
}

static uint8_t motorSpeedViolation2(int32_t flowRate)
{
	return FLOW_RATE_STEP_2 < flowRate && flowRate <= FLOW_RATE_STEP_3;
}

static uint8_t motorSpeedViolation3(int32_t flowRate)
{
	return flowRate <= MAX_FLOW_RATE;
}

int32_t getMotorSpeed()
{
	static uint8_t wasMotorSensorVoltageViolatedLastTime = false;
	int32_t voltage = getPot1Value();
	if(voltage < SENSOR_MIN_VOLTAGE || voltage > SENSOR_MAX_VOLTAGE)
	{
		if(!wasMotorSensorVoltageViolatedLastTime)
		{
			DEBUG_LOGF("Invalid voltage on motor speed sensor: %d\n\r", voltage);
		}
		wasMotorSensorVoltageViolatedLastTime = true;
		return -1;
	}
	wasMotorSensorVoltageViolatedLastTime = false;
    return (voltage - SENSOR_MIN_VOLTAGE) / RPM_PER_MICROVOLT;
}

int32_t getFlowRate()
{
	static uint8_t wasFlowRateSensorVoltageViolatedLastTime = false;
	int32_t voltage = getPot2Value();
	if(voltage < SENSOR_MIN_VOLTAGE || voltage > SENSOR_MAX_VOLTAGE)
	{
		if(!wasFlowRateSensorVoltageViolatedLastTime)
		{
			DEBUG_LOGF("Invalid voltage on flow rate sensor: %d\n\r", voltage);
		}
		wasFlowRateSensorVoltageViolatedLastTime = true;
		return -1;
	}
	wasFlowRateSensorVoltageViolatedLastTime = false;
    return (voltage - SENSOR_MIN_VOLTAGE) / FLOW_PER_MICROVOLT;
}
