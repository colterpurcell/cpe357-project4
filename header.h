/* Typedefs */
#include <time.h>
typedef struct timeval timeval;
typedef struct child
{
    int pid;
    char task[200];
} child;

int searchCurrent(char *, int, char *, int *);

int searchR(char *, int, char *, int *, char *);

int searchRecursive(char *, int, char *, int *, char *, int *, struct timeval *, char *);

void redirect(int);

void list();

void killProc(int);
