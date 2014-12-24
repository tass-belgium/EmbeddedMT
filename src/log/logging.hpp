/*
 * logging.hpp
 *
 *  Created on: Apr 11, 2014
 *      Author: cv
 */

#ifndef LOGGING_HPP_
#define LOGGING_HPP_

#include <stdio.h>
#include <iostream>

#define LEVEL_NONE 0
#define LEVEL_ERROR 1
#define LEVEL_WARNING 2
#define LEVEL_DEBUG 3


#ifdef LOG_LEVEL_NONE
#	define LOG_SEVERITY LEVEL_NONE
#else
#	ifdef LOG_LEVEL_ERROR
#		define LOG_SEVERITY LEVEL_ERROR
#	else
#		ifdef LOG_LEVEL_WARNING
#			define LOG_SEVERITY LEVEL_WARNING
#		else
			// LOG_LEVEL_DEBUG
#			define LOG_SEVERITY LEVEL_DEBUG
#		endif
#	endif
#endif

#define LOG_HELP(msg) do { \
	std::cout << "[HELP] " << msg << std::endl; \
	} while(false);

#define LOG(outputChannel, logSeverity, message...) do { \
	fprintf(outputChannel, "[%s]\t%s - %s | %s:%d | %s | ", logSeverity, __DATE__, __TIME__, __FILE__, __LINE__, __func__); \
	fprintf(outputChannel, message); \
	fprintf(outputChannel, "\n"); \
	} while(false);

#if LOG_SEVERITY >= LEVEL_DEBUG
#	define LOG_ENTER(args...) do { \
		LOG(stdout, "ENTER", args) \
		} while (false);
#	define LOG_EXIT(args...) do { \
		LOG(stdout, "EXIT", args) \
		} while (false);
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

// Measurement log macro's
#	define LOG_MEASUREMENT_START(args...) do { \
		LOG(stdout, "MEASUREMENT_START", args) \
		} while (false);

#	define LOG_MEASUREMENT_DESCRIPTION(args...) do { \
		LOG(stdout, "MEASUREMENT_DESCRIPTION", args) \
		} while (false);

#	define LOG_MEASUREMENT_RESULT(args...) do { \
		LOG(stdout, "MEASUREMENT_RESULT", args) \
		} while (false);

#endif /* LOGGING_HPP_ */
