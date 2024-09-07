#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "header.h"

int contains(const char *str1, const char *str2)
{
    return strstr(str1, str2) != NULL;
}

char *tokenize(char *str, const char *delim)
{
    static char *s = NULL;
    if (str)
    {
        s = str;
    }
    if (s == NULL || *s == '\0')
    {
        return NULL;
    }
    char *start = s;
    while (*s != '\0' && strchr(delim, *s) == NULL)
    {
        s++;
    }
    if (*s != '\0')
    {
        *s = '\0';
        s++;
    }
    else
    {
        s = NULL;
    }
    return start;
}

char *read_input()
{
    char *array = malloc(100);

    if (!array)
    {
        printf("Initial Memory Allocation Failure\n");
        exit(1);
    }

    int position = 0;
    int c;

    while (1)
    {
        c = getchar();

        if (c == (-1) || c == '\n')
        {
            array[position] = '\0';
            return array;
        }
        else
        {
            array[position] = c;
        }
        position++;

        if (position >= 100)
        {
            array = realloc(array, position + 100);
            if (!array)
            {
                printf("Error: Array size increased. Reallocation of memory Failed\n");
                exit(1);
            }
        }
    }
}

void run_commands(char *commands[])
{
    if (strcmp(commands[0], "cd") == 0) // cd command will only change child directory, so to prevent it. I am changing the directory in parent process.
    {
        if (commands[1] == NULL)
        {
            if (chdir(getenv("HOME")) != 0)
            {
                printf("Error: cd command failed");
            }
            // printf("Error: cd command requires an argument\n");
        }
        else
        {
            if (chdir(commands[1]) != 0)
            {
                printf("Error: cd command failed");
            }
        }
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Error: Fork Failed\n");
        exit(1);
    }

    if (pid == 0) // child process
    {
        int error = execvp(commands[0], commands);
        printf("Error: Command Failed with code %d\n", error);
        exit(1);
    }
    else // parent process
    {
        pid_t id = wait(NULL);

        if (id == -1)
        {
            printf("Error: Wait Failed\n");
        }

        return;
    }
}