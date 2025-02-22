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
#define SCHED_ERR_OK                 0          //!< No error occured (Scheduler)
#define SCHED_ERR_INVALID_PTR       -1          //!< Invalid pointer (Scheduler)
#define SCHED_ERR_INVALID_FUNC_PTR  -2          //!< Invalid function pointer
#define SCHED_ERR_MAX_TASKS_REACHED -3          //!< Maximum number of tasks reached

#define MAX_SCHEDULER_TASKS 6                   //!< Maximum number of tasks in the scheduler


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
 * @brief Struct definition for a task in the scheduler
 *
 */
typedef struct _SchedulerTask
{
    uint32_t period;            //!< Period of the task in milliseconds
    CyclicFunction pTask;       //!< Function pointer to cyclic task function
    uint32_t lastExecution;     //!< Timestamp for last execution of task
} SchedulerTask;

/**
 * @brief Struct definition which holds the HAL tick
 * time stamps for the different tasks
 *
 */
typedef struct _Scheduler
{
    GetHALTick pGetHALTick;             //!< Function pointer for callback to read current HAL tick counter

    SchedulerTask tasks[MAX_SCHEDULER_TASKS];    //!< Array of tasks
    uint32_t registeredTaskCount;               //!< Number of registered tasks
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
 * @brief Registers a task in the scheduler
 * 
 * @param pScheduler Pointer to scheduler struct
 * @param period Period of the task in milliseconds
 * @param toRegisterFunction The function, which gets registered
 * 
 * @return SCHED_ERR_OK if not error eccured
 */
int32_t registerTask(Scheduler* pScheduler, uint32_t period, CyclicFunction toRegisterFunction);


#endif
