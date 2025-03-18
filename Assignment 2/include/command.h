/**
 * @file command.h
 * @brief This file contains the structs and function declarations for the command and command chain structs.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef COMMAND_H
#define COMMAND_H

// Includes
#include "utils.h"
#include <stdbool.h>
#include <unistd.h>

/**
 * @brief This struct represents a simple command.
 * 
 * A simple command is a command/process with its args and its set of file descriptors. Different simple commands can be combined together by pipes to form a pipeline. For example, `ls -l` is a simple command, while `ls -l | grep a` is not a simple command.
 * 
 * IO redirection is handled by the shell, not by the command itself. So, the command will just have the file descriptors, and the shell will handle the redirection.
 * 
 */
typedef struct SimpleCommand {
    char* commandName; //< cmd name, e.g. ls etc.

    char** args;       //< args array, including the command name
    int argc;          //< args count, including the command name, so it's equal to the length of the args array

    int inputFD;       //< input file descriptor, default value is 0 (stdin)
    int outputFD;      //< output file descriptor, default value is 1 (stdout)
    int stderrFD;      //< stderr file descriptor, default value is 2 (stderr)
    int pid;           //< represents the processID of the child process, in case of external. Default is -1.

    int noWait;        //< specifies that whether dont need to wait for this simple command to finish. default is 0, in case of a background job, it is 1

    int (*execute)(struct SimpleCommand*); //< function pointer to the function that will execute the simple command.
} SimpleCommand;

/**
 * @brief This struct represents a command, or more precisely a pipeline.
 * 
 * A command is a set of simple commands, and it can be a pipeline of simple commands. For example, `ls -l | grep a` is a command.
 * A command's grammar can be like:
 * ```cmd [args]* [< file] [| cmd [args]*]* [(> OR >> OR 2>) file]```
 * 
 */
typedef struct Command {
    struct SimpleCommand** simpleCommands;  //< array to hold simple commands
    int nSimpleCommands;                    //< number of commands

    bool background;                        //< flag for background execution

    char* chainingOperator;                 //< what chaining operator is used to chain with the next command. can be ';'/'&' but you can add more
    struct Command* next;                   //< pointer to the next command in the chain
} Command;

/**
 * @brief This struct represents a command chain (represented via a linked list). 
 * 
 * A command chain is a set of commands, separated by ; or &.
 * 
 * A command chain's grammar can be like:
 * Command [(; OR &) Command]*
 */
typedef struct CommandChain {
    struct Command* head;   //< pointer to the head of the command chain
    struct Command* tail;   //< pointer to the tail of the command chain
} CommandChain;


// Function declarations


// ------------------------- Initializers --------------------------------

/**
 * @brief This function creates an empty simple command, and returns a pointer to it. It returns NULL on failure. The caller is responsible for freeing the memory allocated by this function.
 * 
 * @return SimpleCommand* Pointer to the simple command
 */
SimpleCommand* initSimpleCommand();

/**
 * @brief This function creates an empty command, and returns a pointer to it. It returns NULL on failure. The caller is responsible for freeing the memory allocated by this function.
 * 
 * @return Command* Pointer to the command
 */
Command* initCommand();

/**
 * @brief This function creates an empty command chain, and returns a pointer to it. It returns NULL on failure. The caller is responsible for freeing the memory allocated by this function.
 * 
 * @return CommandChain* Pointer to the command chain
 */
CommandChain* initCommandChain();


// ------------------------- Pushers --------------------------------

/**
 * @brief This function pushes an argument to the args array of a simple command. It returns 0 on success, -1 on failure.
 * 
 * If the simpleCommand's name is not set, then it also sets the name of the simple command to the argument. Then it pushes to the args array, and increments the argc.
 * 
 * @param arg Name or argument to push
 * @param simpleCommand The simple command to push the argument to
 * @return int Status code (0 on success, -1 on failure)
 */
int pushArgs(char* arg, SimpleCommand* simpleCommand);

/**
 * @brief This function adds a command to the command chain. It returns 0 on success, -1 on failure.
 * 
 * @param chain The command chain to add the command to
 * @param command The command to add to the chain
 * @return int Status code (0 on success, -1 on failure)
 */
int addCommandToChain(CommandChain* chain, Command* command);

/**
 * @brief This function adds a simpleCommand to a command.
 * 
 * Uses realloc to increase the size of the simple commands array in the command, and adds the simple command to the array. Returns 0 on success, -1 on failure.
 * 
 * @param command The command to add the simple command to
 * @param simpleCommand The simple command to add to the command
 * @return int Status code (0 on success, -1 on failure)
 */
int addSimpleCommand(Command* command, SimpleCommand* simpleCommand);

// ------------------------- Cleaners --------------------------------

/**
 * @brief The function is responsible for freeing up the memory allocated by a simple command. All the internal arrays and strings are freed, and the pointer is set to NULL.
 * 
 * @param simpleCommand Pointer to the simpleCommand to be freed
 */
void cleanUpSimpleCommand(SimpleCommand* simpleCommand);

/**
 * @brief The function is responsible for freeing up the memory allocated by a command. All the internal arrays and strings are freed, and the pointer is set to NULL.
 * 
 * @param command Pointer to the command to be freed
 */
void cleanUpCommand(Command* command);

/**
 * @brief The function is responsible for freeing up the memory allocated by a command chain. All the linked list nodes are freed, and the pointer is set to NULL.
 * 
 * @param chain Pointer to the command chain to be freed
 */
void cleanUpCommandChain(CommandChain* chain);

// ------------------------- Execute --------------------------------

/**
 * @brief This function executes a chain of commands.
 * 
 * Function traverses the linked list, and calls executeCommand on each command. The rules for executing a command chain are:
 * 1. If the chaining operator is ';', then execute all commands in the chain, and return the exit status of the last command.
 * 2. If the chaining operator is '&', then the setup was such that the pipeline was set to run in the background.
 * 
 * @param chain The command chain to execute
 * @return int Status code (exit status of the last command according to the rules above)
 */
int executeCommandChain(CommandChain* chain);

/**
 * @brief This function executes a command. The function traverses the simple commands in the command, and executes them one by one.
 * 
 * @param command The command to execute
 * @return int Status code (exit status of the last command)
 */
int executeCommand(Command* command);

// ------------------------- Debug --------------------------------

/**
 * @brief This function prints the command chain in a readable format. Purely a debug utility.
 * 
 * @param chain The command chain to print
 * @return void
 */
void printCommandChain(CommandChain* chain);

/**
 * @brief This function prints the a simpleCommand in a readable format for debugging. Purely a debug utility.
 * 
 * @param simpleCommand The simple command to print
 * @return void
 */
void printSimpleCommand(SimpleCommand* simpleCommand);

#endif // COMMAND_H