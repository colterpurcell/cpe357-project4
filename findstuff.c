#define _POSIX_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include "header.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>

/**
 * @brief Find all files in a directory (or all subdirectories) that match a given pattern,
 * use fork to create a child process to search each subdirectory.
 * @result Print the occurrence of the pattern in the file, in directory, or in all subdirectories.
 */

int fd[2];
child *children;
int d;

int main()
{
    char input[200];
    char textOvr[200];
    char *token;
    char *flags[10] = {0};
    char type[32] = {0};
    char text[1000] = {0};
    int concat = 0, success = 0;
    int processType[3] = {0};
    int i = 0, j;
    children = mmap(NULL, sizeof(child) * 10, PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);

    /*
     * Process type index 0 indicates subdirectory search or not, index 1 indicates whether it will be a
     * file search or a text search, index 2 indicates whether it will be restricted to a certain ending.
     */
    d = dup(STDIN_FILENO);
    pipe(fd);

    for (i = 0; i < 10; i++)
    {
        children[i].pid = 0;
        children[i].task[0] = '\0';
    }

    while (1)
    {
        for (i = 0; i < 10; i++)
        {
            flags[i] = NULL;
        }
        for (i = 0; i < 3; i++)
        {
            processType[i] = 0;
        }

        i = 0;
        /* Prompt loop */
        printf("\033[1;32mfindstuff\033[0m$ ");
        fgets(input, 200, stdin);
        strcpy(textOvr, input);
        textOvr[strlen(textOvr) - 1] = '\0';
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
                munmap(children, sizeof(child) * 10);
                exit(0);
            }
        }
        if (strcmp(flags[0], "list") == 0)
        {
            success = 1;
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
                        strcpy(children[i].task, textOvr);
                        children[i].pid = fork();
                        if (children[i].pid == 0)
                        {
                            if (processType[1] == 0)
                            {
                                searchCurrent(flags[1], 0, NULL, fd);
                            }
                            else
                            {
                                if (processType[2] == 0)
                                {
                                    searchCurrent(text, 1, NULL, fd);
                                }
                                else
                                {
                                    searchCurrent(text, 1, type, fd);
                                }
                            }
                            children[i].task[0] = '\0';
                            children[i].pid = 0;
                            kill(getppid(), SIGUSR1);
                            exit(0);
                        }
                        else
                        {
                            signal(SIGUSR1, redirect);
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
                        pipe(fd);
                        strcpy(children[i].task, textOvr);
                        children[i].pid = fork();
                        if (children[i].pid == 0)
                        {
                            if (processType[1] == 0)
                            {
                                searchR(flags[1], 0, NULL, fd, getcwd(NULL, 0));
                            }
                            else
                            {
                                if (processType[2] == 0)
                                {
                                    searchR(text, 1, NULL, fd, getcwd(NULL, 0));
                                }
                                else
                                {
                                    searchR(text, 1, type, fd, getcwd(NULL, 0));
                                }
                            }
                            children[i].task[0] = '\0';
                            children[i].pid = 0;
                            kill(getppid(), SIGUSR1);
                            exit(0);
                        }
                        else
                        {
                            signal(SIGUSR1, redirect);
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
    }
}

void redirect(int sig)
{
    char buffer[4096] = {0};
    if (sig != SIGUSR1)
    {
        return;
    }
    /* When child interrupts parent, pipe end is redirected to standard
     * input, parent then reads in the content of the pipe through stdin and prints it out
     */
    dup2(fd[0], STDIN_FILENO);
    /* Read from stdin */
    read(STDIN_FILENO, buffer, sizeof(buffer));
    fwrite(buffer, sizeof(char), sizeof(buffer), stdout);
    /* Reset stdin using dup2 */
    dup2(d, STDIN_FILENO);
}

void killProc(int process)
{
    if (children[process - 1].pid > 0)
    {
        kill(children[process - 1].pid, SIGKILL);
        children[process].pid = 0;
        children[process].task[0] = '\0';
    }
    else
    {
        printf("Process %d does not exist.\n", process);
    }
}

void list()
{
    int i;
    printf("\n");
    for (i = 0; i < 10; i++)
    {
        if (children[i].pid > 0)
        {
            printf("\033[1;33mProcess %d\033[0m: %s\n", i + 1, children[i].task);
        }
    }
}
