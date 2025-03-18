/**
 * @file builtitns.c
 * @brief Contains the function definitions for the builtin shell functions.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "shell_builtins.h"
#include "parser.h"
#include "command.h"

#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

// stores the original stdin and stdout fds
extern ShellState* globalShellState;

// adds a command to the history list
int add_to_history(HistoryList* list, char* command)
{
    if (!command)
        return -1;

    HistoryNode* node = malloc(sizeof(HistoryNode));
    node->command = COPY(command);
    node->next = NULL;

    if (!list->head)
    {
        list->head = node;
    }
    else if (!list->head->next)
    {
        list->tail = node;
        list->head->next = list->tail;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
    return 0;
}

// retrieves a command at a particular index from the list
char* get_command(HistoryList* list, unsigned int index)
{
    if (index > list->size || !list->head)
        return NULL;

    HistoryNode* curr = list->head;
    unsigned int ctr = 1;

    for (; curr && ctr != index; curr = curr->next, ctr++);

    return curr->command;    
}

void clean_history(HistoryList* list) {
    HistoryNode* current = list->head;
    HistoryNode* next;

    while (current != NULL) {
        next = current->next;
        free(current->command);
        free(current);
        current = next;
    }

    // After cleaning up all nodes, reset the list
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

char* find_last_command_with_prefix(HistoryList* list, const char* prefix)
{
    if (list == NULL || list->head == NULL || prefix == NULL) {
        return NULL; // Invalid input
    }

    HistoryNode* current = list->head;
    char* lastCommand = NULL;

    while (current != NULL) 
    {
        // Check if the current command starts with the given prefix
        if (strncmp(current->command, prefix, strlen(prefix)) == 0) {
            // Update the lastCommand whenever a match is found
            
            lastCommand = current->command; // Duplicate the string
        }

        current = current->next;
    }

    return lastCommand;
}

// initializes the shell state
ShellState* init_shell_state()
{
    ShellState* stateObj = malloc(sizeof(ShellState));
    if (!stateObj)
    {
        LOG_ERROR("malloc failure. Exiting.\n");
        exit(-1);
    }

    stateObj->originalStdinFD = STDIN_FD;
    stateObj->originalStdoutFD = STDOUT_FD;
    stateObj->originalStderrFD = STDERR_FD;

    // default prompt
    strncpy(stateObj->prompt_buffer, "\%", MAX_STRING_LENGTH);

    stateObj->history.head = NULL;
    stateObj->history.tail = NULL;
    stateObj->history.size = 0;

    return stateObj;
}

// cleans things up and frees memory
int clear_shell_state(ShellState* stateObj)
{
    if (!stateObj)
    {
        LOG_DEBUG("Can't clear NULL shell state object.\n");
        return -1;
    }

    free(stateObj);
    return 0;
}

/*-------------------------------File Desc Manipulators----------------------------------*/

/**
 * @brief Sets up the file descriptors for a command. Duplicates the file descriptors to stdin, and stdout, and if we are in the parent process, we also save the original stdin and stdout file descriptors.
 * 
 * Uses dup2 system call to set up the file descriptors. Returns 0 on success, -1 on failure. Only dups if the file descriptors are not the default ones.
 * 
 * @param inputFD The input file descriptor
 * @param outputFD The output file descriptor
 * @return int Status code (0 on success, -1 on failure)
 */
static int setUpFD(int inputFD, int outputFD, int stderrFD)
{
    if (inputFD != STDIN_FD)
    {
        globalShellState->originalStdinFD = dup(STDIN_FD);

        if (dup2(inputFD, STDIN_FD) == -1)
        {
            LOG_DEBUG("dup2: %s\n", strerror(errno));
            return -1;
        }

        close(inputFD);
    }

    if (outputFD != STDOUT_FD)
    {
        globalShellState->originalStdoutFD = dup(STDOUT_FD);
        
        if (dup2(outputFD, STDOUT_FD) == -1)
        {
            LOG_DEBUG("dup2: %s\n", strerror(errno));
            return -1;
        }

        close(outputFD);
    }

    if (stderrFD != STDERR_FD)
    {
        globalShellState->originalStderrFD = dup(STDERR_FD);
        
        if (dup2(stderrFD, STDERR_FD) == -1)
        {
            LOG_DEBUG("dup2: %s\n", strerror(errno));
            return -1;
        }

        close(stderrFD);
    }

    return 0;
}

/**
 * @brief Resets the file descriptors to the default ones (stdin and stdout).
 * 
 * @return void
 */
static void resetFD()
{
    if (globalShellState->originalStdinFD != STDIN_FD)
    {
        if (dup2(globalShellState->originalStdinFD, STDIN_FD) == -1)
        {
            LOG_ERROR("dup2: %s\n", strerror(errno));
            exit(1);
        }
    }

    if (globalShellState->originalStdoutFD != STDOUT_FD)
    {
        if (dup2(globalShellState->originalStdoutFD, STDOUT_FD) == -1)
        {
            LOG_ERROR("dup2: %s\n", strerror(errno));
            exit(1);
        }
    }

    if (globalShellState->originalStderrFD != STDERR_FD)
    {
        if (dup2(globalShellState->originalStderrFD, STDERR_FD) == -1)
        {
            LOG_ERROR("dup2: %s\n", strerror(errno));
            exit(1);
        }
    }
}

/*-------------------------------Builtins-----------------------------------------------*/

int cd(SimpleCommand* simpleCommand)
{
    if (simpleCommand->argc > 2)
    {
        LOG_ERROR("cd: Too many arguments\n");
        return -1;
    }

    // Don't think cd ever needs any input from stdin, neither it puts anything to stdout, so dont need to modify file descriptors
    const char* path = NULL;
    if (simpleCommand->argc == 1)
    {
        // No path specified, go to home directory
        path = HOME_DIR;
    }
    else
    {
        path = simpleCommand->args[1];
    }

    if (chdir(path) == -1)
    {
        LOG_ERROR("cd: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int pwd(SimpleCommand* simpleCommand)
{
    if (simpleCommand->argc > 1)
    {
        LOG_ERROR("pwd: Too many arguments\n");
        return -1;
    }

    char cwd[MAX_PATH_LENGTH];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        LOG_ERROR("pwd: %s\n", strerror(errno));
        return -1;
    }

    if (setUpFD(simpleCommand->inputFD, simpleCommand->outputFD, simpleCommand->stderrFD))
    {
        return -1;
    }

    LOG_PRINT("%s\n", cwd);

    resetFD();
    return 0;
}

int exitShell(SimpleCommand* simpleCommand)
{
    if (simpleCommand->argc > 2)
    {
        printf("exit: Too many arguments\n");
        return -1;
    }
    LOG_OUT("exit\n");

    if (simpleCommand->argc == 1)
        exit(0);
    
    // check if each char in the second arg is a number or not. that was the only standard compliant way I could think of to figure whether the argument is a numebr or not
    if(strspn(simpleCommand->args[1], "0123456789") != strlen(simpleCommand->args[1]))
    {
        LOG_ERROR("exit: Expects a numerical argument\n");
        return -1;
    }

    int exit_status = atoi(simpleCommand->args[1]);
    exit(exit_status);
}

int history(SimpleCommand* simpleCommand)
{
    if (simpleCommand->argc > 2)
    {
        LOG_ERROR("history: Too many arguments\n");
        return -1;
    }

    if (setUpFD(simpleCommand->inputFD, simpleCommand->outputFD, simpleCommand->stderrFD))
    {
        return -1;
    }

    if (simpleCommand->argc == 1)
    {
        HistoryNode* curr = globalShellState->history.head;
        int i = 1;
        while (curr)
        {
            LOG_PRINT("%d %s\n", i, curr->command);
            curr = curr->next;
            i++;
        }

        resetFD();
    }
    else
    {
        char* input = NULL;
        if(strspn(simpleCommand->args[1], "0123456789") == strlen(simpleCommand->args[1]))
        {
            // execute the commmand at that index
            unsigned int idx = (unsigned int)atoi(simpleCommand->args[1]);
            input = COPY(get_command(&globalShellState->history, idx));

            if (!input)
            {
                LOG_ERROR("history: invalid index\n");
                return -1;
            }
        }
        else
        {
            char* last = find_last_command_with_prefix(&globalShellState->history, simpleCommand->args[1]);
            input = COPY(last);

            if (!input)
            {
                LOG_ERROR("history: no matching command found\n");
                return -1;
            }
        }

        // simple whitespace tokenizer
        char** tokens = tokenizeString(input, ' ');

        // generate the command from tokens
        CommandChain* commandChain = parseTokens(tokens);
        
        // execute the command
        int status = executeCommandChain(commandChain);
        (void)status;
        // Free tokens
        freeTokens(tokens);

        // free the command chain
        cleanUpCommandChain(commandChain);

        // Free buffer that was allocated for input
        free(input);
    }

    return 0;
}

int executeProcess(SimpleCommand* simpleCommand)
{
    int pid = fork();

    if (pid == -1)
    {
        LOG_DEBUG("fork: %s\n", strerror(errno));
        return -1;
    }
    else if (pid == 0)
    {
        // Duplicate the FDs. Default FDs are STDIN AND STDOUT but, if pipes or  < > are used, the FDs are updated in the parsing step, by opening the relevant file or creating relevant pipes
        setUpFD(simpleCommand->inputFD, simpleCommand->outputFD, simpleCommand->stderrFD);

        // Execute the command
        if (execvp(simpleCommand->commandName, simpleCommand->args) == -1)
        {
            LOG_ERROR("%s: %s\n", simpleCommand->commandName, strerror(errno));
            exit(1);
        }

        // This should never be reached
        LOG_ERROR("This should never be reached\n");
        exit(0);
    }
    else
    {
        // Parent process
        simpleCommand->pid = pid;

        if (!simpleCommand->noWait) {
            // waiting for the child process to finish
            int status;
            LOG_DEBUG("Waiting for child process, with command name %s\n", simpleCommand->commandName);
            if (waitpid(pid, &status, 0) == -1)
            {
                LOG_ERROR("waitpid: %s\n", strerror(errno));
                return -1;
            }

            // print the error (if any) from errno
            if (WEXITSTATUS(status) != 0)
            {
                // LOG_ERROR("%s: %s\n", simpleCommand->commandName, strerror(errno));
                LOG_DEBUG("Non zero exit status : %d\n", WEXITSTATUS(status));
                return WEXITSTATUS(status);
            }
        }
    }

    LOG_DEBUG("Finished executing command %s\n", simpleCommand->commandName);
    return 0;
}

// Changes the current prompt of the shell
int prompt(SimpleCommand* simpleCommand) 
{
    if (simpleCommand->argc == 1)
    {
        LOG_ERROR("prompt: Too few arguments\n");
        return -1;
    }

    if (simpleCommand->argc > 2)
    {
        LOG_ERROR("prompt: Too many arguments\n");
        return -1;
    }

    strcpy(globalShellState->prompt_buffer, simpleCommand->args[1]);

    return 0;
}

/**
 * @brief This struct represents the builtin commands of the shell, and their corresponding execution functions.
 * 
 */
typedef struct commandRegistry
{
    char* commandName;
    ExecutionFunction executionFunction;
} CommandRegistry;

/**
 * @brief Registry of all the commands supported by the shell, and their corresponding execution functions. If a command is not found in the registry, it is assumed to be a process to be executed and the executeProcess function is called. Add new commands here, with their appropriate functions.
 * 
 */
static const CommandRegistry commandRegistry[] = {
    {"cd", cd},
    {"pwd", pwd},
    {"exit", exitShell},
    {"history", history},
    {"prompt", prompt},
    {NULL, NULL}
};

ExecutionFunction getExecutionFunction(char* commandName)
{
    for (int i = 0; commandRegistry[i].commandName != NULL; i++)
    {
        if (strcmp(commandRegistry[i].commandName, commandName) == 0)
        {
            return commandRegistry[i].executionFunction;
        }
    }

    return executeProcess;
}