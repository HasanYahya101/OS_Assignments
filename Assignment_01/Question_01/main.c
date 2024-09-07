#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char *commands[10];
        for (int i = 1; i < argc; i++)
        {
            commands[i - 1] = argv[i];
        }
        commands[argc - 1] = NULL;
        run_commands(commands);
    }

    while (1)
    {
        printf("shell> ");
        char *input = read_input();

        if (contains(input, "exit")) // exit the shell if "exit" is found
        {
            free(input);
            exit(0);
        }

        char *commands[10];
        int i = 0;
        char *token = tokenize(input, " ");
        while (token != NULL && i < 10 - 1)
        {
            commands[i++] = token;
            token = tokenize(NULL, " ");
        }
        commands[i] = NULL;

        if (commands[0] != NULL)
        {
            run_commands(commands);
        }

        free(input);
    }
    return 0;
}
