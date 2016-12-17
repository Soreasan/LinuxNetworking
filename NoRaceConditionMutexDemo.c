/* 
Kenneth Adair
This program is almost identical to 
"RaceConditionNoMutex.c" except that in the 
*func function there is a mutex which locks the variable
while we're updating it.  Therefore this program
consistently counts to 2 million rather than getting
random numbers.
Taken from Linux Networking course.
Program to demonstrate why we need mutexes 
To compile do this:
gcc [filename] -lpthread
gcc [filename] -lpthread -o [output filename]
*/

#include <pthread.h>
#include <stdio.h>

static int global = 0;
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;

void *func(void *arg)
{
    int local, i;

    for (i=0; i<1000000; i++) {
    //This lock prevents other threads from doing anything to the data till we're done.
    pthread_mutex_lock(&mylock);
    local = global;
    local++;
    global = local;
    //Unlock the mutex so other threads can interact with the data
    pthread_mutex_unlock(&mylock);
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
