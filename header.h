/* Typedefs */
#include <time.h>
typedef struct timeval timeval;
typedef struct child
{
    int pid;
    char task[200];
} child;

int searchCurrent(char *, int, char *, child *, int *);

int searchR(char *, int, char *, child *, int *, char *);

int searchRecursive(char *, int, char *, child *, int *, char *, int *, struct timeval *);

void redirect(int);

void list();
