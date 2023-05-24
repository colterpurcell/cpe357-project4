#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include "header.h"

/**
 * @brief Find all files in a directory (or all subdirectories) that match a given pattern,
 * use fork to create a child process to search each subdirectory.
 * @result Print the occurrence of the pattern in the file, in directory, or in all subdirectories.
 */

int main()
{
    char input[1000];
    char *token;
    char *flags[10] = {0};
    child *children[10] = {NULL};
    int i = 0, j;

    /*
     * Process type index 0 indicates subdirectory search or not, index 1 indicates whether it will be a
     * file search or a text search, index 2 indicates whether it will be restricted to a certain ending.
     */
    int processType[3] = {0};

    while (1)
    {
        /* Prompt loop */
        printf("\033[1;32mfindstuff\033[0m$ ");
        fgets(input, 1000, stdin);
        token = strtok(input, " -\n");
        while (token != NULL)
        {
            flags[i] = token;
            token = strtok(NULL, " -\n");
            i++;
        }
        for (j = 0; j < i; j++)
        {
            if (flags[j][0] == 's')
            {
                processType[0] = 1;
            }
            else if (strchr((const char *)flags[j], '"') != NULL)
            {
                processType[1] = 1;
            }
            else if (flags[j][0] == 'f' && j != 0)
            {
                processType[2] = 1;
            }
            else if (flags[j][0] == 'q' || strcmp(flags[j], "quit") == 0)
            {
                exit(0);
            }
        }
        for (j = 0; j < i; j++)
        {
            printf("%s ", flags[j]);
        }

        i = 0;
        for (j = 0; j < 3; j++)
        {
            printf("%d", processType[j]);
            processType[j] = 0;
        }
        printf("\n");
        /* searchCurrent(flags[1]); */
    }
}
