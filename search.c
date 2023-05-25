#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>
#include "header.h"

/**
 * @param pattern The pattern to be searched for, whether it be a whole file or a substring from a file(type).
 * @param type The type of search to be performed, 0 indicates a file search, 1 indicates a text search.
 * @param ending The ending of the file to be searched for, if any.
 */

/* if (found)
{
    gettimeofday(&rawTime, &timeZone);
    printf("\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", entry->d_name, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
}
else
{
    printf("\033[1;31mFile Not Found\033[0m\n");
} */

int searchCurrent(char *pattern, int type, char *ending, int *pipe)
{
    DIR *dir = opendir(".");
    FILE *fptr;
    struct dirent *entry;
    struct timeval rawTime;
    struct timezone timeZone;

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
                gettimeofday(&rawTime, &timeZone);
                sprintf(current, "\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", entry->d_name, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
                write(pipe[1], current, strlen(current));
                found++;
            }
        }
        if (found == 0)
        {
            current[0] = '\0';
            sprintf(current, "\033[1;31mFile Not Found\033[0m\n");
            write(pipe[1], current, strlen(current));
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
                            gettimeofday(&rawTime, &timeZone);
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
                                gettimeofday(&rawTime, &timeZone);
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
    return found;
}

/* while (i < numChild)
{
    waitpid(pid[i], &status, WNOHANG);
    i++;
} */
