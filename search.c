#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>

void searchCurrent(char *pattern)
{
    DIR *dir = opendir(".");
    struct dirent *entry;
    struct timeval rawTime;
    struct timezone timeZone;

    int found = -1;
    while ((entry = readdir(dir)) != NULL)
    {

        if (entry->d_type == DT_REG && strstr(entry->d_name, pattern) != NULL)
        {

            found = 0;
            break;
        }
    }

    if (found == 0)
    {
        gettimeofday(&rawTime, &timeZone);
        printf("\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02ld:%02ld:%02ld:%02ld\033[0m\n", entry->d_name, getcwd(NULL, 0), rawTime.tv_sec / 3600 % 24, rawTime.tv_sec / 60 % 60, rawTime.tv_sec % 60, rawTime.tv_usec / 10000);
    }
    else
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
    }

    closedir(dir);
}

/* while (i < numChild)
{
    waitpid(pid[i], &status, WNOHANG);
    i++;
} */
