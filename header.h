/* Typedefs */
typedef struct child
{
    int pid;
    char task[200];
} child;

int searchCurrent(char *, int, char *, child *, int *);

void redirect(int);

void list();
