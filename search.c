#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

void searchCurrent(char *pattern)
{
    DIR *dir = opendir(".");
    struct dirent *entry;
    time_t rawTime;
    struct tm *timeInfo;

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
        time(&rawTime);
        timeInfo = localtime(&rawTime);
        printf("\033[1;33m%s\033[0m found in \033[1;34m%s\033[0m at \033[1;34m%02d:%02d:%02d\033[0m\n", entry->d_name, getcwd(NULL, 0), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
    }
    else
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
    }

    closedir(dir);
}
