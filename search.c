#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "header.h"

/**
 * @param pattern The pattern to be searched for, whether it be a whole file or a substring from a file(type).
 * @param type The type of search to be performed, 0 indicates a file search, 1 indicates a text search.
 * @param ending The ending of the file to be searched for, if any.
 */

/*
TODO: Ensure that the pid is changed to notify the redirect for the parent
*/

int searchCurrent(char *pattern, int type, char *ending, child *chld, int *pipe)
{
    DIR *dir = opendir(".");
    FILE *fptr;
    struct dirent *entry;
    struct timeval rawTime;

    char current[1000];
    int found = 0;
    if (type == 0)
    {
        /*
         * Search for filename inside of all files in directory.
         * Pipe the results to the parent, interrupting if necessary.
         * If multiple files are found, separate each entry with a newline
         */
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, pattern) == 0)
            {
                current[0] = '\0';
                gettimeofday(&rawTime, NULL);
                printf("\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", entry->d_name, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
                sprintf(current, "\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", entry->d_name, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
                write(pipe[1], current, strlen(current));
                found++;
            }
        }
    }
    else
    {
        /*
         * Search for pattern inside of all files in directory.
         * Pipe the results to the parent, interrupting if necessary.
         * If multiple are found, separate each entry with a newline
         */
        if (!ending)
        {
            while ((entry = readdir(dir)) != NULL)
            {
                fptr = fopen(entry->d_name, "r");
                if (fptr != NULL)
                {
                    char line[4095 + 1];
                    while (fgets(line, sizeof(line), fptr))
                        if (strstr(line, pattern))
                        {
                            current[0] = '\0';
                            gettimeofday(&rawTime, NULL);
                            sprintf(current, "\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", pattern, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
                            write(pipe[1], current, strlen(current));
                            found++;
                            break;
                        }
                }
                fclose(fptr);
            }
        }
        else
        {
            while ((entry = readdir(dir)) != NULL)
            {
                if (strstr(entry->d_name, ending))
                {
                    fptr = fopen(entry->d_name, "r");
                    if (fptr != NULL)
                    {
                        char line[4095 + 1];
                        while (fgets(line, sizeof(line), fptr))
                            if (strstr(line, pattern))
                            {
                                current[0] = '\0';
                                gettimeofday(&rawTime, NULL);
                                sprintf(current, "\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", pattern, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
                                write(pipe[1], current, strlen(current));
                                found++;
                                break;
                            }
                    }
                    fclose(fptr);
                }
            }
        }
    }
    closedir(dir);
    if (found == 0)
    {
        current[0] = '\0';
        printf("\033[1;31mFile Not Found\033[0m\n");
        sprintf(current, "\033[1;31mNot Found\033[0m\n");
        write(pipe[1], current, strlen(current));
        kill(getppid(), SIGUSR1);
    }
    printf("Made it here\n");
    chld->pid = 0;
    return found;
}

/* while (i < numChild)
{
    waitpid(pid[i], &status, WNOHANG);
    i++;
} */
