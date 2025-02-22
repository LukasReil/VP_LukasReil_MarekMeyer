/******************************************************************************
 * @file Scheduler.c
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com
 * @date   08.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Implementation of the cooperative scheduler with a  pre-defined set
 * of cyclic "task slots"
 *
 *
 *****************************************************************************/


/***** INCLUDES **************************************************************/
#include "Scheduler.h"


/***** PRIVATE CONSTANTS *****************************************************/


/***** PRIVATE MACROS ********************************************************/

#define FUNC_VALID                  0           //!< Function pointer is in text section
#define FUNC_NOT_VALID              1           //!< Function pointer is not in text section

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/

/**
 * @brief Checks if a function pointer lies inside of the program FLASH 
 * 
 * @param toRegisterFunction The function, which gets tested
 * 
 * @return FUNC_VALID if the function pointer lies inside of the program FLASH 
 *         FUNC_NOT_VALID if the function pointer lies outside of the program FLASH 
 */
static uint8_t isCyclicFunctionValid(CyclicFunction toCheckFunction);

/***** PRIVATE VARIABLES *****************************************************/


/***** PUBLIC FUNCTIONS ******************************************************/


int32_t schedInitialize(Scheduler* pScheduler)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(pScheduler->pGetHALTick == 0){
        return SCHED_ERR_INVALID_PTR;
    }

    uint32_t beginTickTime      = pScheduler->pGetHALTick();
    for(uint32_t i = 0; i < pScheduler->registeredTaskCount; i++){
        pScheduler->tasks[i].lastExecution = beginTickTime;
    }

    return SCHED_ERR_OK;
}


int32_t schedCycle(Scheduler* pScheduler)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(pScheduler->pGetHALTick == 0){
        return SCHED_ERR_INVALID_PTR;
    }

    for(uint32_t i = 0; i < pScheduler->registeredTaskCount; i++){
        uint32_t nowTickTime = pScheduler->pGetHALTick();
        if(nowTickTime - pScheduler->tasks[i].lastExecution >= pScheduler->tasks[i].period){
            pScheduler->tasks[i].lastExecution += pScheduler->tasks[i].period;
            if(pScheduler->tasks[i].pTask != 0){
                pScheduler->tasks[i].pTask();
            }
        }
    }

    return SCHED_ERR_OK;
}

int32_t registerHALTickFunction(Scheduler* pScheduler, GetHALTick halTickFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(halTickFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }

    pScheduler->pGetHALTick = halTickFunction;
}

int32_t registerTask(Scheduler *pScheduler, uint32_t period, CyclicFunction toRegisterFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(toRegisterFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }
    if(pScheduler->registeredTaskCount >= MAX_SCHEDULER_TASKS){
        return SCHED_ERR_MAX_TASKS_REACHED;
    }

    pScheduler->tasks[pScheduler->registeredTaskCount].period = period;
    pScheduler->tasks[pScheduler->registeredTaskCount].pTask = toRegisterFunction;
    pScheduler->tasks[pScheduler->registeredTaskCount].lastExecution = 0;

    pScheduler->registeredTaskCount++;
    return SCHED_ERR_OK;
}

/***** PRIVATE FUNCTIONS *****************************************************/

static uint8_t isCyclicFunctionValid(CyclicFunction toCheckFunction)
{
    if(&_stext <= toCheckFunction && toCheckFunction <= &_etext){
        return FUNC_VALID;
    }

    return FUNC_NOT_VALID;
}