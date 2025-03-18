/**
 * @file utils.h
 * @brief Contains useful macros and utilities to be used by the shell.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UTILS_H
#define UTILS_H

#include "log.h"

#include <string.h>
#include <stdlib.h>

// Exposed macros for logging
#define LOG_ERROR(...) LOG(LOG_ERR, "[ERROR]", LOG_COLOR_ERR, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_DBG, "[DEBUG]", LOG_COLOR_DBG, __VA_ARGS__)
#define LOG_PRINT(...) LOG(LOG_PRI, "[PRINT]", LOG_COLOR_PRI, __VA_ARGS__)

// we specify that the strings in our program won't exceed length of 100 characters
#define MAX_STRING_LENGTH 1024

// in order to be consistent, let's just define a macro for copying strings
#define COPY(str) (str ? strndup(str, MAX_STRING_LENGTH) : NULL)

// Useful macros for file descriptors to make the code more readable
#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

/**
 * @brief This function tokenizes a string, given a delimiter.
 * 
 * It returns an array of tokens, and the number of tokens in the array. The function ignores any delimiter encountered inside quotes.
 * It is the responsibility of the caller to free the memory via the freeTokens() function.
 * 
 * @param str String to tokenize
 * @param delimiter Delimiter to use for tokenization
 * @return char** Array of tokens (NULL terminated)
 */
char** tokenizeString(const char* str, const char delimiter);

/**
 * @brief This function frees the memory allocated by tokenizeString()
 * 
 * @param tokens Array of tokens
 */
void freeTokens(char** tokens);

/**
 * @brief This function removes the quotes from a string. If the string is not quoted, it returns the same string. Otherwise, the current string is freed, and a pointer to the new string is returned.
 * 
 * @param str String to remove quotes from
 * @return char* Pointer to the new string
 */
char* removeQuotes(char* str);

/**
 * @brief Get the Tokens count
 * 
 * @param tokens The NULL terminated array of tokens
 * @return int Number of tokens in the array
 */
int getTokenCount(char** tokens);

#endif // UTILS_H