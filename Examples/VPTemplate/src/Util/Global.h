/******************************************************************************
 * @file Global.h
 *
 * @author Andreas Schmidt (a.v.schmidt81@googlemail.com
 * @date   08.02.2025
 *
 * @copyright Copyright (c) 2025
 *
 ******************************************************************************
 *
 * @brief Header File for global constants and types
 *
 *
 *****************************************************************************/
#ifndef _GLOBAL_H_
#define _GLOBAL_H_


/***** INCLUDES **************************************************************/


/***** CONSTANTS *************************************************************/


/***** MACROS ****************************************************************/


#define ERROR_OK            0       //!< Error code for "everything is ok"
 
#define ERROR_GENERAL       -1      //!< General, unspecific error

#define LOG_OUTPUT_ENABLED  1       //!< Enable log output


#if LOG_OUTPUT_ENABLED
#include "LogOutput.h"
#define DEBUG_LOG(x) outputLog((x))
#define DEBUG_LOGF(...) outputLogf(__VA_ARGS__)
#else
#define DEBUG_LOG(x)
#define DEBUG_LOGF(...)
#endif // LOG_OUTPUT_ENABLED


/***** TYPES *****************************************************************/


/***** PROTOTYPES ************************************************************/


#endif
