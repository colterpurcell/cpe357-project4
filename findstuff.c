#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include "header.h"
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

/**
 * @brief Find all files in a directory (or all subdirectories) that match a given pattern,
 * use fork to create a child process to search each subdirectory.
 * @result Print the occurrence of the pattern in the file, in directory, or in all subdirectories.
 */

child children[10];
int pipes[10][2];
int d;

int main()
{
    char input[200];
    char *token;
    char *flags[10] = {0};
    char type[10] = {0};
    char text[1000] = {0};
    int concat = 0, success = 0;
    int i = 0, j;

    /*
     * Process type index 0 indicates subdirectory search or not, index 1 indicates whether it will be a
     * file search or a text search, index 2 indicates whether it will be restricted to a certain ending.
     */
    int processType[3] = {0};
    d = dup(STDIN_FILENO);

    for (i = 0; i < 10; i++)
    {
        children[i].pid = 0;
        children[i].task[0] = '\0';
    }

    while (1)
    {
        i = 0;
        /* Prompt loop */
        printf("\033[1;32mfindstuff\033[0m$ ");
        fgets(input, 200, stdin);
        token = strtok(input, " -\n");
        while (token != NULL)
        {
            flags[i] = token;
            token = strtok(NULL, " -\n");
            i++;
        }
        for (j = 0; j < i; j++)
        {
            if (concat == 1)
            {
                if (flags[j][strlen(flags[j]) - 1] == '"')
                {
                    concat = 0;
                    flags[j][strlen(flags[j]) - 1] = '\0';
                }
                strcat(text, " ");
                strcat(text, flags[j]);
            }
            else if (flags[j][0] == '"')
            {
                if (flags[j][strlen(flags[j]) - 1] == '"')
                {
                    flags[j][strlen(flags[j]) - 1] = '\0';
                    memmove(flags[j], flags[j] + 1, strlen(flags[j]));
                }
                else
                {
                    concat = 1;
                    memmove(flags[j], flags[j] + 1, strlen(flags[j]));
                }
                strcpy(text, flags[j]);
                processType[1] = 1;
            }
            else if (flags[j][0] == 's' && j > 1)
            {
                processType[0] = 1;
            }
            else if (flags[j][0] == 'f' && j != 0)
            {
                strcpy(type, ".");
                memmove(flags[j], flags[j] + 2, strlen(flags[j]));
                strcat(type, flags[j]);
                processType[2] = 1;
            }
            else if (flags[j][0] == 'q' || strcmp(flags[j], "quit") == 0)
            {
                exit(0);
            }
        }
        for (j = 0; j < i; j++)
        {
            printf("FLAGS %d: %s\n", j, flags[j]);
        }
        if (strcmp(flags[0], "list") == 0)
        {
            list();
        }
        else
        {
            if (processType[0] == 0)
            {
                i = 0;
                while (i < 10)
                {
                    if (children[i].task[0] == '\0')
                    {
                        success = 1;
                        pipe(pipes[i]);
                        strcpy(children[i].task, input);
                        children[i].pid = fork();
                        if (children[i].pid == 0)
                        {
                            if (processType[1] == 0)
                            {
                                searchCurrent(flags[1], 0, NULL, &children[i], pipes[i]);
                            }
                            else
                            {
                                if (processType[2] == 0)
                                {
                                    searchCurrent(flags[1], 1, NULL, &children[i], pipes[i]);
                                }
                                else
                                {
                                    searchCurrent(flags[1], 1, type, &children[i], pipes[i]);
                                }
                            }
                            exit(0);
                        }
                        break;
                    }
                    i++;
                }
            }
            else
            {
                i = 0;
                while (i < 10)
                {
                    if (children[i].task[0] == '\0')
                    {
                        success = 1;
                        pipe(pipes[i]);
                        strcpy(children[i].task, input);
                        children[i].pid = fork();
                        if (children[i].pid == 0)
                        {
                            if (processType[1] == 0)
                            {
                                searchR(flags[1], 0, NULL, &children[i], pipes[i], getcwd(NULL, 0));
                            }
                            else
                            {
                                if (processType[2] == 0)
                                {
                                    searchR(flags[1], 1, NULL, &children[i], pipes[i], getcwd(NULL, 0));
                                }
                                else
                                {
                                    searchR(flags[1], 1, type, &children[i], pipes[i], getcwd(NULL, 0));
                                }
                            }
                            exit(0);
                        }
                        break;
                    }
                    i++;
                }
            }
        }
        if (success == 0)
        {
            printf("Too many processes running, please wait for one to finish.\n");
        }
        else
        {
            success = 0;
        }
        i = 0;
        for (j = 0; j < 3; j++)
        {
            printf("%d", processType[j]);
            processType[j] = 0;
        }
        printf("%s\n", text);
    }
}

void redirect(int sig)
{
    int i = 0;
    if (sig == 10)
    {
        while (i < 10)
        {
            if (children[i].pid == -1)
            {
                children[i].pid = 0;
                break;
            }
            i++;
        }
        dup2(pipes[i][0], STDIN_FILENO);
    }
}

void list()
{
    int i;
    for (i = 0; i < 10; i++)
    {
        if (children[i].pid > 0)
        {
            printf("%d\n", children[i].pid);
            printf("\033[1;33mProcess %d\033[0m: %s\n", i + 1, children[i].task);
        }
    }
}
