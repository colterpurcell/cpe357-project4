/* Typedefs */
typedef struct child
{
    int pid;
    char task[200];
} child;

int searchCurrent(char *, int, char *, child *, int *);

int searchRecursive(char *, int, char *, child *, int *, char *);

void redirect(int);

void list();
