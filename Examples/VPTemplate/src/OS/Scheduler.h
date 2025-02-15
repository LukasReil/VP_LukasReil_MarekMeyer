/******************************************************************************
 * @file Scheduler.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com
 * @date   08.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Header File for cooperative scheduler module
 *
 *
 *****************************************************************************/
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_


/***** INCLUDES **************************************************************/
#include <stdint.h>

/***** CONSTANTS *************************************************************/
const extern uint32_t _stext;                        //!< Symbol from linker file to detect invalid function pointers
const extern uint32_t _etext;                        //!< Symbol from linker file to detect invalid function pointers

/***** MACROS ****************************************************************/
#define SCHED_ERR_OK                0           //!< No error occured (Scheduler)
#define SCHED_ERR_INVALID_PTR       -1          //!< Invalid pointer (Scheduler)
#define SCHED_ERR_INVALID_FUNC_PTR  -2          //!< Invalid function pointer


/***** TYPES *****************************************************************/

/**
 * @brief Function pointer for reading the current HAL Tick timer
 *
 * This provides the possibility to decouple the scheduler from a
 * specific HAL implementation, but still makes it possible to measure
 * the time inside the scheduler.
 *
 */
typedef uint32_t (*GetHALTick)(void);

/**
 * @brief Function pointer for cyclic function for the scheduler
 *
 */
typedef void (*CyclicFunction)(void);

/**
 * @brief Struct definition which holds the HAL tick
 * time stamps for the different tasks
 *
 */
typedef struct _Scheduler
{
    GetHALTick pGetHALTick;             //!< Function pointer for callback to read current HAL tick counter

    uint32_t halTick_1ms;               //!< Timestamp for last execution of 1ms task
    CyclicFunction pTask_1ms;           //!< Function pointer to 1ms cyclic task function

    uint32_t halTick_10ms;              //!< Timestamp for last execution of 10ms task
    CyclicFunction pTask_10ms;          //!< Function pointer to 10ms cyclic task function

    uint32_t halTick_100ms;             //!< Timestamp for last execution of 100ms task
    CyclicFunction pTask_100ms;         //!< Function pointer to 100ms cyclic task function

    uint32_t halTick_250ms;             //!< Timestamp for last execution of 250ms task
    CyclicFunction pTask_250ms;         //!< Function pointer to 250ms cyclic task function

    uint32_t halTick_1000ms;            //!< Timestamp for last execution of 1000ms task
    CyclicFunction pTask_1000ms;        //!< Function pointer to 1000ms cyclic task function
} Scheduler;


/***** PROTOTYPES ************************************************************/


/**
 * @brief Initializes the Scheduler component
 * Initializes the internal values for the timestamps.
 *
 * @remark: This function doesn't initialize the function
 * pointers in the Scheduler struct!
 *
 * @param pScheduler Pointer to scheduler struct
 *
 * @return SCHED_ERR_OK if no error occured
 */
int32_t schedInitialize(Scheduler* pScheduler);

/**
 * @brief Cyclic function for the scheduler
 * This function should be called in the super loop of the system
 * Hereby the scheduler takes care of the different time slots for
 * the tasks
 *
 * @param pScheduler Pointer to scheduler struct
 *
 * @return SCHED_ERR_OK if no error occured
 */
int32_t schedCycle(Scheduler* pScheduler);

/**
 * @brief Registers the function, which gets called by the scheduler,
 *        to determine the time
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t registerHALTickFunction(Scheduler* pScheduler, GetHALTick halTickFunction);


/**
 * @brief Registers the function, which gets called every millisecond.
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t register1msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction);

/**
 * @brief Registers the function, which gets called every 10 milliseconds.
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t register10msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction);

/**
 * @brief Registers the function, which gets called every 100 milliseconds.
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t register100msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction);

/**
 * @brief Registers the function, which gets called every 250 milliseconds.
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t register250msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction);

/**
 * @brief Registers the function, which gets called every 1000 milliseconds.
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t register1000msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction);



/***** PRIVATE FUNCTIONS *****************************************************/
/**
 * @brief Checks if a function pointer lies inside of the program FLASH 
 * 
 * @param toRegisterFunction The function, which gets tested
 * 
 * @return FUNC_VALID if the function pointer lies inside of the program FLASH 
 *         FUNC_NOT_VALID if the function pointer lies outside of the program FLASH 
 */
static uint8_t isCyclicFunctionValid(CyclicFunction toCheckFunction);

#endif
