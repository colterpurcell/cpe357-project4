#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

/**
 * @brief Find all files in a directory that match a given pattern,
 * use fork to create a child process to search each subdirectory.
 */