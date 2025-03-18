/**
 * @file log.h
 * @brief A collection of macros and functions to facilitate logging in a helpful manner, instead of using plain printf statements.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// Log coloring
#define LOG_RESET   "\033[0m"
#define LOG_RED     "\033[1;31m"
#define LOG_GREEN   "\033[1;32m"
#define LOG_YELLOW  "\033[1;33m"
#define LOG_BLUE    "\033[1;34m"
#define LOG_CYAN    "\033[1;36m"
#define LOG_WHITE   "\033[1;37m"

// defines for logging modes
#define LOG_ERR     0  /* For printing critical errors, always get printed */
#define LOG_DBG     1  /* For printing debug print statements, only in debug mode */
#define LOG_PRI     2  /* For normal printing of messages, always get printed without annotations */

// Default colors for different log types
#define LOG_COLOR_ERR   LOG_RED
#define LOG_COLOR_DBG   LOG_CYAN
#define LOG_COLOR_PRI   LOG_WHITE

// debug mode, debug messages are printed only if this is set to 1 (debug is provided as a compiler flag)
#ifdef DEBUG
#define ANNOTATIONS 1      /* Change this to zero to disable all annotations even in debug mode */
#else
#define DEBUG 0
#define ANNOTATIONS 0
#endif

// Macros to change the behavior of annotations
#define ANNOTATIONS_INFO 1  /* Change this to zero to disable annotations info */

#define ANNOTATIONS_FILE 0
#define ANNOTATIONS_FUNC 1
#define ANNOTATIONS_LINE 1

// output function for printing, default is printf
#define LOG_OUT(...) printf(__VA_ARGS__)

// defines the annotation string.
#define ANNOTATION_INFO_STRING do {\
    if (ANNOTATIONS_INFO) {\
        LOG_OUT(" ("); \
        if (ANNOTATIONS_FILE) printf("%s", __FILE__); \
        if (ANNOTATIONS_FILE) printf(","); \
        if (ANNOTATIONS_FUNC) printf("%s", __func__); \
        if (ANNOTATIONS_FUNC) printf(","); \
        if (ANNOTATIONS_LINE) printf("%d", __LINE__); \
        LOG_OUT(") ");\
    }\
} while (0)

// macro to log a message.
#define LOG(type, prefix, color, ...) \
    do { \
        if (DEBUG) {\
            LOG_OUT("%s%s%s: ", color, prefix, LOG_RESET); \
            ANNOTATION_INFO_STRING; \
            if (type == LOG_DBG) { \
                LOG_OUT(__VA_ARGS__); \
                break; \
            } \
        }\
        if (type == LOG_ERR || type == LOG_PRI) { LOG_OUT(__VA_ARGS__); } \
    } while (0)

#endif // LOG_H