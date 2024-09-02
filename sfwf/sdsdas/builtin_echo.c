/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   becho.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skreik <skreik@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 10:25:01 by skreik            #+#    #+#             */
/*   Updated: 2024/08/30 10:25:01 by skreik           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "mini_shell.h"
// Function prototypes


// int main() {
//     // Test cases for builtin_echo

//     // Case 1: Expanded variable $HOME
//     char *test1[] = {"Home", "directory:", "$HOME", NULL};
//     printf("Test 1:\n");
//     builtin_echo(test1);

//     // Case 2: Expanded variable $USER
//     char *test2[] = {"Current", "user:", "$USER", NULL};
//     printf("Test 2:\n");
//     builtin_echo(test2);

//     // Case 3: Expanded variable $PATH
//     char *test3[] = {"System", "path:", "$PATH", NULL};
//     printf("Test 3:\n");
//     builtin_echo(test3);

//     // Case 4: Expanded variable $SHELL
//     char *test4[] = {"Current", "shell:", "$SHELL", NULL};
//     printf("Test 4:\n");
//     builtin_echo(test4);

//     // Case 5: Expanded variable $PWD
//     char *test5[] = {"Current", "directory:", "$PWD", NULL};
//     printf("Test 5:\n");
//     builtin_echo(test5);

//     // Case 6: Expanded variable $LANG
//     char *test6[] = {"Language is 'hi'", "setting:", "$LANG", NULL};
//     printf("Test 6:\n");
//     builtin_echo(test6);

//     // Case 7: Combined test
//     char *test7[] = {"Info:", "$USER", "in", "$HOME", "with", "PATH='$PATH'", NULL};
//     printf("Test 7:\n");
//     builtin_echo(test7);

//     // Case 8: Echo with -n flag
//     char *test8[] = {"-n", "Echo", "without", "newline", NULL};
//     printf("Test 8:\n");
//     builtin_echo(test8);

//     return 0;
// }// Function implementations
int check(char **input, char c) 
{
    (*input)++;
    while (**input)
    {
        if (**input == c)
            return 1;
        (*input)++;
    }
    return 0;
}

bool check_balanced_quotes(char *input)
{
    while (*input)
    {
        if (*input == '\'' || *input == '\"')
        {
            if (!check(&input, *input))
                return 0;
        }
        input++;
    }
    return 1;
}

void print_expanded_input(char *input, bool inside_single_quotes)
{
    while (*input)
    {
        if (*input == '$' && !inside_single_quotes)
        {
            input++;
            char *var_name_start = input;
            while (*input && (*input == '_' || (*input >= 'a' && *input <= 'z') || (*input >= 'A' && *input <= 'Z') || (*input >= '0' && *input <= '9')))
                input++;
            size_t var_name_length = input - var_name_start;
            char var_name[var_name_length + 1];
            strncpy(var_name, var_name_start, var_name_length);
            var_name[var_name_length] = '\0';

            char *value = getenv(var_name);
            if (value)
                printf("%s", value);
            else
                printf("$%s", var_name); // Print the variable name if not found
        }
        else
        {
            printf("%c", *input);
            input++;
        }
    }
}

void builtin_echo_helper(char **input, char quote)
{
    bool inside_single_quotes = (quote == '\'');
    while (**input && **input != quote)
    {
        if (**input == '\\' && (*(*input + 1) == 'n' || *(*input + 1) == 't'))
        {
            // Handle escape sequences
            if (*(*input + 1) == 'n')
                printf("\n");
            else if (*(*input + 1) == 't')
                printf("\t");
            (*input) += 2;
        }
        else if (**input == '$' && !inside_single_quotes)
        {
            print_expanded_input(*input, inside_single_quotes);
            while (**input && **input != ' ' && **input != quote)
                (*input)++;
        }
        else
        {
            printf("%c", **input);
            (*input)++;
        }
    }
}

void builtin_echo(t_parser *list)
{
    int i = 0;

    while (list->input[i])
    {
        if (!check_balanced_quotes(list->input[i]))
        {
            printf("Error: Unbalanced quotes in argument %d.\n", i + 1);
            return;
        }
        char *arg = list->input[i];
        char quote;

        while (*arg)
        {
            if (*arg == '\'' || *arg == '\"')
            {
                quote = *arg;
                arg++;
                builtin_echo_helper(&arg, quote);
                if (*arg == quote)
                    arg++;
            }
            else
            {
                if (*arg == '$')
                {
                    print_expanded_input(arg, false);
                    while (*arg && *arg != ' ' && *arg != '\'' && *arg != '\"')
                        arg++;
                }
                else
                {
                    printf("%c", *arg);
                    arg++;
                }
            }
        }
        if (list->input[i + 1])  // Print space between arguments
            printf(" ");
        i++;
    }
    if (list->operations == NULL || strcmp(list->operations, "-n") != 0)
        printf("\n");  // Print a newline at the end if not suppressed
}
