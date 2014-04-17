/*
 * logging.hpp
 *
 *  Created on: Apr 11, 2014
 *      Author: cv
 */

#ifndef LOGGING_HPP_
#define LOGGING_HPP_

#define LEVEL_NONE 0
#define LEVEL_ERROR 1
#define LEVEL_WARNING 2
#define LEVEL_DEBUG 3
#define LEVEL_PROFILE 4


#ifdef LOG_LEVEL_NONE
#	define LOG_SEVERITY LEVEL_NONE
#else
#	ifdef LOG_LEVEL_ERROR
#		define LOG_SEVERITY LEVEL_ERROR
#	else
#		ifdef LOG_LEVEL_WARNING
#			define LOG_SEVERITY LEVEL_WARNING
#		else
#			ifdef LOG_LEVEL_PROFILE
#				define LOG_SEVERITY LEVEL_PROFILE
#			else
				// LOG_LEVEL_DEBUG
#				define LOG_SEVERITY LEVEL_DEBUG
#			endif
#		endif
#	endif
#endif

#include <stdio.h>

#define LOG(outputChannel, logSeverity, message...) do { \
	fprintf(outputChannel, "[%s]\t%s - %s | %s:%d | %s | ", logSeverity, __DATE__, __TIME__, __FILE__, __LINE__, __func__); \
	fprintf(outputChannel, message); \
	fprintf(outputChannel, "\n"); \
	} while(false);

#if LOG_SEVERITY >= LEVEL_PROFILE
#define LOG_ENTER(args...) static float_t totalTimeSpend=0; \
	static uint32_t nbEnters = 0; \
	nbEnters++; \
	clock_t tStart = clock(); \
	do { \
	LOG(stdout, "ENTER", args) \
	} while (false);
#define LOG_EXIT(args...) do { \
	float_t totalTimeElapsed = (float_t) (clock() - tStart)/ CLOCKS_PER_SEC; \
	totalTimeSpend += totalTimeElapsed; \
	LOG(stdout,"TIME", "%f s in %d execution(s) \t Avg: %f s/e\n", totalTimeSpend, nbEnters, (float_t) totalTimeSpend/(float_t) nbEnters); \
	LOG(stdout, "EXIT", args) \
	} while (false);
#endif

#if LOG_SEVERITY >= LEVEL_DEBUG
#	if LOG_SEVERITY == LEVEL_DEBUG
#		define LOG_ENTER(args...) do { \
			LOG(stdout, "ENTER", args) \
			} while (false);
#		define LOG_EXIT(args...) do { \
			LOG(stdout, "EXIT", args) \
			} while (false);
#	endif
#	define LOG_INFO(args...) do { \
		LOG(stdout, "INFO", args) \
		} while (false);
#	define LOG_DEBUG(args...) LOG_INFO(args)
#else
#	define LOG_INFO(args...)
#	define LOG_DEBUG(args...)
#	define LOG_ENTER(args...)
#	define LOG_EXIT(args...)
#endif

#if LOG_SEVERITY >= LEVEL_WARNING
#define LOG_WARNING(args...) do { \
	LOG(stderr, "WARNING", args) \
	} while (false);
#else
#define LOG_WARNING(args...)
#endif

#if LOG_SEVERITY >= LEVEL_ERROR
#define LOG_ERROR(args...) do { \
	LOG(stderr, "ERROR", args) \
	} while (false);
#else
#define LOG_ERROR(args...)
#endif


#endif /* LOGGING_HPP_ */
