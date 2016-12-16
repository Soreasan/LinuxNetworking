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

This program would create a zombie process but
signal(SIGCHLD, SIGN_IGN);
prevents the zombies from being created by telling
the program to ignore SIGCHLD signals.  
*/

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main()
{
    //This one lines prevents zombies
    signal(SIGCHLD, SIG_IGN);
    //This tries to create zombies.
    if(fork() == 0)
        exit(0);
    sleep(1000);
}
