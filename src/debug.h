#pragma once

/** \file debug.h */

#ifndef NDEBUG
	#ifdef PLATFORM_N64
		#include "usb_debugging/debug.h"
		#define INIT_LOG() do { if(DEBUG_MODE) debug_initialize(); } while(0)
		#define UPDATE_LOG() do { if(DEBUG_MODE) debug_pollcommands(); } while(0)
		#define LOG_TRACE(fmt, ...) do { if(DEBUG_MODE)  debug_printf("[TRACE] "fmt, __VA_ARGS__); } while(0)
		#define LOG_DEBUG(fmt, ...) do { if(DEBUG_MODE)  debug_printf("[DEBUG] "fmt, __VA_ARGS__); } while(0)
		#define LOG_INFO(fmt, ...)  do { if(DEBUG_MODE)  debug_printf("[INFO]  "fmt, __VA_ARGS__); } while(0)
		#define LOG_WARN(fmt, ...)  do { if(DEBUG_MODE)  debug_printf("[WARN]  "fmt, __VA_ARGS__); } while(0)
		#define LOG_ERROR(fmt, ...) do { if(DEBUG_MODE)  debug_printf("[ERROR] "fmt, __VA_ARGS__); } while(0)
		#define LOG_FATAL(fmt, ...) do { if(DEBUG_MODE)  debug_printf("[FATAL] "fmt, __VA_ARGS__); } while(0)
	#else
		#include <stdio.h>
		#define DEBUG_MODE 1
		#define INIT_LOG()
		#define UPDATE_LOG()
		#define LOG_TRACE(fmt, ...) do { if(DEBUG_MODE) printf("[TRACE] "fmt, __VA_ARGS__); } while(0)
		#define LOG_DEBUG(fmt, ...) do { if(DEBUG_MODE) printf("[DEBUG] "fmt, __VA_ARGS__); } while(0)
		#define LOG_INFO(fmt, ...)  do { if(DEBUG_MODE) printf("[INFO]  "fmt, __VA_ARGS__); } while(0)
		#define LOG_WARN(fmt, ...)  do { if(DEBUG_MODE) printf("[WARN]  "fmt, __VA_ARGS__); } while(0)
		#define LOG_ERROR(fmt, ...) do { if(DEBUG_MODE) printf("[ERROR] "fmt, __VA_ARGS__); } while(0)
		#define LOG_FATAL(fmt, ...) do { if(DEBUG_MODE) printf("[FATAL] "fmt, __VA_ARGS__); } while(0)
	#endif

#else
#define INIT_LOG()
#define UPDATE_LOG()
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)
#endif // ifndef NDEBUG