/**
 * @file builtins.h
 * @brief Inlcudes structures and utilities to manage the builtins and the internal state of the shell.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "command.h"

#define HOME_DIR getenv("HOME")
#define MAX_PATH_LENGTH 1024

// a linked list to represent shell history
typedef struct HistoryNode {
    char* command;
    struct HistoryNode* next;
} HistoryNode;

typedef struct HistoryList {
    HistoryNode* head;
    HistoryNode* tail;
    size_t size;
} HistoryList;

// adds a command to the history list
int add_to_history(HistoryList* list, char* command);

// retrieves a command at a particular index from the list
char* get_command(HistoryList* list, unsigned int index);

// clean up history
void clean_history(HistoryList* list);

// finds the last command that starts with the prefix
char* find_last_command_with_prefix(HistoryList* list, const char* prefix);

// To represent the state of the shell.
typedef struct ShellState {

    // these holds the original shell stdin, stdout and stderr
    int originalStdoutFD;
    int originalStdinFD;
    int originalStderrFD;

    // shell variable that holds the current prompt
    char prompt_buffer[MAX_STRING_LENGTH];

    // represents the history node list, storing tail for quick insertions
    HistoryList history;
} ShellState;

// initializes the shell state
ShellState* init_shell_state();
// cleans things up and frees memory
int clear_shell_state(ShellState* stateObj);

typedef int (*ExecutionFunction)(SimpleCommand*);

/**
 * @brief Returns the execution function for the given command.
 * 
 * @param commandName The name of the command.
 * @return ExecutionFunction The execution function for the given command.
*/
ExecutionFunction getExecutionFunction(char* commandName);

/**
 * @brief This function is the builtin for the cd command.
 * 
 * @param command The command to be executed.
 * @return int Returns 0 on success, -1 on failure.
 */
int cd(SimpleCommand* command);

/**
 * @brief This function is the builtin for the exit command.
 * 
 * @param command The command to be executed.
 * @return int Returns 0 on success, -1 on failure.
 */
int exitShell(SimpleCommand* command);

/**
 * @brief This function is the builtin for the pwd command.
 * 
 * @param command The command to be executed.
 * @return int Returns 0 on success, -1 on failure.
 */
int pwd(SimpleCommand* command);

/**
 * @brief This function is the builtin for the history command.
 * 
 * @param command The command to be executed.
 * @return int Returns 0 on success, -1 on failure.
 */
int history(SimpleCommand* command);

/**
 * @brief This function executes a process.
 * 
 * The process is executed by forking a child process, and then executing the command in the child process.
 * 
 * @param command The command to be executed.
 * @return int Returns non-zero status on failue. else returns 0 on success
 */
int executeProcess(SimpleCommand* command);

#endif // BUILTINS_H