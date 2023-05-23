#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "header.h"

/**
 * @brief Find all files in a directory that match a given pattern,
 * use fork to create a child process to search each subdirectory.
 */

int main()
{
    char input[1000];
    while (1)
    {
        /* Prompt loop */
        printf("\033[1;32mfindstuff\033[0m$ ");
        fgets(input, 1000, stdin);
        if (strcmp(input, "quit\n") == 0 || strcmp(input, "q\n") == 0)
        {
            printf("Quitting...\n");
            break;
        }
        input[strlen(input) - 1] = '\0';
        searchCurrent(input);
    }
}
