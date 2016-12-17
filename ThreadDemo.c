/*
Kenneth Adair
Taken from Linux Networking course on Pluralsight
Demonstrates extremely basic thread stuff
To compile type this:
gcc [filename] -lpthread -o [output file name]
*/

#include <pthread.h>
#include <stdio.h>

void *func(void *arg)
{
    printf("child thread says %s\n", (char *)arg);
    //The void pointer allows us to return the 99
    pthread_exit((void *)99);
}

int main()
{
    pthread_t handle;
    int exitcode;

    pthread_create(&handle, NULL, func, "hi!");
    printf("primary thread says hello\n");
    pthread_join(handle, (void **)&exitcode);
    printf("exit code %d\n", exitcode);
}
