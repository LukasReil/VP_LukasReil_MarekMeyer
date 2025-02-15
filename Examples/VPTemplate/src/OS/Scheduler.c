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
#define HAL_TICK_VALUE_1MS      1       //!< Number of HAL Ticks used for 1ms Tasks
#define HAL_TICK_VALUE_10MS     10      //!< Number of HAL Ticks used for 10ms Tasks
#define HAL_TICK_VALUE_100MS    100     //!< Number of HAL Ticks used for 100ms Tasks
#define HAL_TICK_VALUE_250MS    250     //!< Number of HAL Ticks used for 250ms Tasks
#define HAL_TICK_VALUE_1000MS   1000    //!< Number of HAL Ticks used for 1000ms Tasks

#define FUNC_VALID                  0           //!< Function pointer is in text section
#define FUNC_NOT_VALID              1           //!< Function pointer is not in text section

/***** PRIVATE TYPES *********************************************************/


/***** PRIVATE PROTOTYPES ****************************************************/


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
    pScheduler->halTick_1ms     = beginTickTime;
    pScheduler->halTick_10ms    = beginTickTime;
    pScheduler->halTick_100ms   = beginTickTime;
    pScheduler->halTick_250ms   = beginTickTime;
    pScheduler->halTick_1000ms  = beginTickTime;    

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

    uint32_t nowTickTime = pScheduler->pGetHALTick();
    if(nowTickTime - pScheduler->halTick_1ms >= HAL_TICK_VALUE_1MS){
        pScheduler->halTick_1ms += HAL_TICK_VALUE_1MS;
        if(pScheduler->pTask_1ms != 0){
            pScheduler->pTask_1ms();
        }
    }


    nowTickTime = pScheduler->pGetHALTick();
    if(nowTickTime - pScheduler->halTick_10ms >= HAL_TICK_VALUE_10MS){
        pScheduler->halTick_10ms += HAL_TICK_VALUE_10MS;
        if(pScheduler->pTask_10ms != 0){
            pScheduler->pTask_10ms();
        }
    }


    nowTickTime = pScheduler->pGetHALTick();
    if(nowTickTime - pScheduler->halTick_10ms >= HAL_TICK_VALUE_100MS){
        pScheduler->halTick_100ms += HAL_TICK_VALUE_100MS;
        if(pScheduler->pTask_100ms != 0){
            pScheduler->pTask_100ms();
        }
    }


    nowTickTime = pScheduler->pGetHALTick();
    if(nowTickTime - pScheduler->halTick_250ms >= HAL_TICK_VALUE_250MS){
        pScheduler->halTick_250ms += HAL_TICK_VALUE_250MS;
        if(pScheduler->pTask_250ms != 0){
            pScheduler->pTask_250ms();
        }
    }


    nowTickTime = pScheduler->pGetHALTick();
    if(nowTickTime - pScheduler->halTick_1000ms >= HAL_TICK_VALUE_1000MS){
        pScheduler->halTick_1000ms += HAL_TICK_VALUE_1000MS;
        if(pScheduler->pTask_1000ms != 0){
            pScheduler->pTask_1000ms();
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

int32_t register1msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(toRegisterFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }

    pScheduler->pTask_1ms = toRegisterFunction;
}

int32_t register10msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(toRegisterFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }

    pScheduler->pTask_10ms = toRegisterFunction;
}

int32_t register100msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(toRegisterFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }

    pScheduler->pTask_100ms = toRegisterFunction;
}

int32_t register250msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(toRegisterFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }

    pScheduler->pTask_250ms = toRegisterFunction;
}

int32_t register1000msTask(Scheduler* pScheduler, CyclicFunction toRegisterFunction)
{
    if(pScheduler == 0){
        return SCHED_ERR_INVALID_PTR;
    }
    if(isCyclicFunctionValid(toRegisterFunction) != FUNC_VALID){
        return SCHED_ERR_INVALID_FUNC_PTR;
    }

    pScheduler->pTask_1000ms = toRegisterFunction;
}

/***** PRIVATE FUNCTIONS *****************************************************/

static uint8_t isCyclicFunctionValid(CyclicFunction toCheckFunction)
{
    if(&_stext <= toCheckFunction && toCheckFunction <= &_etext){
        return FUNC_VALID;
    }

    return FUNC_NOT_VALID;
}