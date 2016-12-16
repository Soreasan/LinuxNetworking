/*
Kenneth Adair
Zombie processes occur when a process exits but
its parent isn't waiting for it.  
The process wants to pass its exit status but
no one wants it.
If we create too many zombies they will fill
up the process table and the system will grind 
to a halt.
Taken from Linux Networking course on Pluralsight.
This program creates a zombie process.
*/

#include <unistd.h>
#include <stdlib.h>

int main()
{
    if(fork() == 0)
        exit(0);
    sleep(1000);
}
