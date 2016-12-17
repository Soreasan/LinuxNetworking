/*
Kenneth Adair
Taken from Linux networking course on Pluralsight 
Program to demonstrate why we need mutexes 
This program tries to count to two million but will fail
because two threads keep interacting with the same global
variable. 
Could compile like this:
gcc [filename] -lpthread
gcc [filename] -lpthread -o [output filename] 
*/

#include <pthread.h>
#include <stdio.h>

static int global = 0;

void *func(void *arg)
{
    int local, i;

    for (i=0; i<1000000; i++) {
	local = global;
	local++;
	global = local;
    }

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, func, NULL);
    pthread_create(&t2, NULL, func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("global = %d\n", global);
}
