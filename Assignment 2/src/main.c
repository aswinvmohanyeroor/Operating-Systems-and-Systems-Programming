/**
 * @file main.c
 * @brief This is the main file for the shell. It contains the main function and the loop that runs the shell.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "utils.h"
#include "command.h"
#include "parser.h"
#include "shell_builtins.h"

#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

// Global variables
int lastExitStatus = 0;

ShellState* globalShellState;

// shell can also support scripting, useful for testing
FILE* scriptFile;

char* getInput(int interactive)
{
    char* input = malloc(MAX_STRING_LENGTH);

    if (interactive)
    {
        int again = 1;
        char *linept;        // pointer to the line buffer

        while (again) {
            again = 0;
            printf("%s ", globalShellState->prompt_buffer);
            linept = fgets(input, MAX_STRING_LENGTH, stdin);
            if (linept == NULL) 
                if (errno == EINTR)
                    again = 1;        // signal interruption, read again
        }

        // remove the trailing newline
        size_t ln = strlen(input) - 1;
        if (input[ln] == '\n')
            input[ln] = '\0';
    }
    else
    {
        size_t len = 0;
        ssize_t read;
        read = getline(&input, &len, scriptFile);
        if (read == -1)
        {
            free(input);
            return NULL;
        }
        // Remove trailing newline
        if (input[read - 1] == '\n')
            input[read - 1] = '\0';
    }

    return input;
}

void sigint_handler(int signo) {
    // Handle SIGINT (CTRL-C)
    LOG_DEBUG("\nCTRL-C pressed. signo: %d\n", signo);
}

void sigtstp_handler(int signo) {
    // Handle SIGTSTP (CTRL-Z)
    LOG_DEBUG("\nCTRL-Z pressed. signo: %d\n", signo);
}

void sigquit_handler(int signo) {
    // Handle SIGQUIT (CTRL-\)
    LOG_DEBUG("\nCTRL-\\ pressed. signo: %d\n", signo);
}

void sigchld_handler(int signo) {
    (void) signo;
    int more = 1;        // more zombies to claim
    pid_t pid;           // pid of the zombie
    int status;          // termination status of the zombie

    while (more) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0) 
            more = 0;
    }
}

/**
 * @brief This is the main function for the shell. It contains the main loop that runs the shell.
 * 
 * @return int 
 */
int main(int argc, char** argv)
{
    // by default we are in interactive
    int interactive = 1;
    scriptFile = NULL;

    if (argc > 2)
    {
        LOG_ERROR("Usage: %s [script]\n", argv[0]);
        exit(1);
    }

    // If a script is provided, run it and exit
    if (argc == 2)
    {
        interactive = 0;
        LOG_DEBUG("Running script %s\n", argv[1]);
        scriptFile = fopen(argv[1], "r");
        if (!scriptFile)
        {
            LOG_ERROR("Error opening script %s: %s\n", argv[1], strerror(errno));
            exit(1);
        }
    }
    globalShellState = init_shell_state();

    LOG_DEBUG("Starting shell\n");

    char delimiter = ' ';

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        LOG_ERROR("Unable to register SIGINT handler");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR) {
        LOG_ERROR("Unable to register SIGTSTP handler");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGQUIT, sigquit_handler) == SIG_ERR) {
        LOG_ERROR("Unable to register SIGQUIT handler");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
        LOG_ERROR("Unable to register SIGCHLD handler");
        exit(EXIT_FAILURE);
    }

    while (1) 
    {
        // read input
        char* input = getInput(interactive);

        // Check for EOF.
        if (!input)
            break;
        if (strcmp(input, "") == 0) 
        {
            free(input);
            continue;
        }
        if (strcmp(input, "exit") == 0)
        {
            free(input);
            break;
        }

        // Add input to readline history.
        add_to_history(&globalShellState->history, input);

        // simple whitespace tokenizer
        char** tokens = tokenizeString(input, delimiter);

        for (int i = 0; tokens[i] != NULL; i++) {
            LOG_DEBUG("Token %d: [%s]\n", i, tokens[i]);
        }

        // generate the command from tokens
        CommandChain* commandChain = parseTokens(tokens);

        // display the command chain
        printCommandChain(commandChain);
        
        // execute the command
        int status = executeCommandChain(commandChain);
        LOG_DEBUG("Command executed with status %d\n", status);

        // Free tokens
        freeTokens(tokens);

        // free the command chain
        cleanUpCommandChain(commandChain);

        // Free buffer that was allocated for input
        free(input);
    }

    // clean up history before we leave
    clean_history(&globalShellState->history);

    return 0;
}