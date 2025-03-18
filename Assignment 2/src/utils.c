/**
 * @file utils.c
 * @brief Function definitions for the utility functions.
 * @version 0.1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils.h"

#include <string.h>
#include <stdlib.h>


// tokenizes the string based on the delimiter
char **tokenizeString(const char *input, char delimiter)
{
    int input_length = strlen(input);
    char **tokens = (char **)malloc(sizeof(char *) * input_length);
    int token_count = 0;

    int i = 0;
    int token_start = 0;
    int inside_quotes = 0;

    while (input[i] != '\0')
    {
        if (input[i] == delimiter && !inside_quotes)
        {
            int token_length = i - token_start;
            tokens[token_count] = (char *)malloc(sizeof(char) * (token_length + 1));
            strncpy(tokens[token_count], input + token_start, token_length);
            tokens[token_count][token_length] = '\0';
            token_count++;
            token_start = i + 1;
        }
        else if (input[i] == '"' || input[i] == '\'')
        {
            inside_quotes = !inside_quotes;
        }
        i++;
    }

    int token_length = i - token_start;
    tokens[token_count] = (char *)malloc(sizeof(char) * (token_length + 1));
    strncpy(tokens[token_count], input + token_start, token_length);
    tokens[token_count][token_length] = '\0';
    token_count++;

    char** temp = (char **)realloc(tokens, sizeof(char *) * (token_count + 1));
    if (!temp)
        return NULL;
    
    tokens = temp;
    temp = NULL;
    
    tokens[token_count] = NULL;

    return tokens;
}

// counts the number of tokens in the token array
int getTokenCount(char **tokens)
{
    int token_count = 0;
    while (tokens[token_count] != NULL)
    {
        token_count++;
    }
    return token_count;
}

// frees the tokens
void freeTokens(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

// removes quotes from the string
char *removeQuotes(char *inputString)
{
    int inputLength = strlen(inputString);

    // Check if the string is long enough to contain quotes
    if (inputLength < 2)
    {
        // String is too short to be enclosed in quotes
        return inputString;
    }

    // Check if the string is enclosed in quotes
    if ((inputString[0] == '"' && inputString[inputLength - 1] == '"') || (inputString[0] == '\'' && inputString[inputLength - 1] == '\''))
    {
        // Create a modified string without the quotes
        size_t modifiedLength = inputLength - 2;
        char *modifiedString = malloc((modifiedLength + 1) * sizeof(char));
        strncpy(modifiedString, inputString + 1, modifiedLength);
        modifiedString[modifiedLength] = '\0';
        free(inputString);
        return modifiedString;
    }
    else
    {
        // String is not enclosed in quotes
        return inputString; // Return a copy of the input string
    }
}