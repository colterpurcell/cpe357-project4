#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

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
    int i = 0, j;

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
            printf("%s\n", flags[j]);
        }
        if (strcmp(input, "quit\n") == 0 || strcmp(input, "q\n") == 0)
        {
            printf("Quitting...\n");
            break;
        }
        i = 0;
        /*
        input[strlen(input) - 1] = '\0';
        searchCurrent(input);
        */
    }
}
