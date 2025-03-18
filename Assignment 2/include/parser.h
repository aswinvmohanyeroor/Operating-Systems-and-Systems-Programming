/**
 * @file parser.h
 * @brief Contains useful macros and the definition for the main parser.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PARSER_H
#define PARSER_H

#include "command.h"

// Useful macros for readability

// macro to test if a token is a chaining operator. the chaining operators are &, ;. macro resolves to 1 if the token is a chaining operator, 0 otherwise
#define IS_CHAINING_OPERATOR(token) (strcmp(token, "&") == 0 || strcmp(token, ";") == 0)
// checks if the token is a background operator
#define IS_BACKGROUND(token) (token && strcmp(token, "&") == 0)
// check if the token is a pipe
#define IS_PIPE(token) (strcmp(token, "|") == 0)
// check if the token is file output redirection operator
#define IS_FILE_OUT_REDIR(token) (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0)
// check if the token is file input redirection operator
#define IS_FILE_IN_REDIR(token) (strcmp(token, "<") == 0) 
// check if the token is stderr redirection operator
#define IS_STDERR_REDIR(token) (strcmp(token, "2>") == 0) 
// check if the token is NULL
#define IS_NULL(token) (!token)
// check if the token is ignorable
#define IGNORE(token) (token && (strcmp(token, " ") == 0 || strcmp(token, "\t") == 0 || strcmp(token, "\n") == 0 || strcmp(token, "") == 0))
// check if the token is the append operator
#define IS_APPEND(token) (strcmp(token, ">>") == 0)

/**
 * @brief Parses the tokens and returns a command chain. It is the responsibility of the caller to free the memory.
 * 
 * @param tokens The tokens to parse. Assumes that the tokens array is null terminated.
 * @return CommandChain* The command chain that was parsed.
 */
CommandChain* parseTokens(char** tokens);

#endif // PARSER_H