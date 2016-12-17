/* 
    Kenneth Adair
    Multi-threaded client to count prime
    numbers using multiple servers.
    Taken from Linux Networking course.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include "primes.h"

#define MAX_SERVERS 16

//Global variables
pthread_t handles[MAX_SERVERS]; //An array of thread handles
int nprimes= 0;     // count of primes found
int lower =  0;     // start of next range
int biggest = 10000000; // largest number to test
int chunk_size = 1000;  // size of each range

//Mutexes to protect our global variables
pthread_mutex_t prime_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lower_lock = PTHREAD_MUTEX_INITIALIZER;

// This is the function the threads run
void *func(void *s)
{
    struct subrange range;
    int fd, n;

    // Get our thread-specific file descriptor
    fd = *(int *)s;

    // Send ranges to our server until there are none left
    while(1) {
    // Grab the next range
    pthread_mutex_lock(&lower_lock);
    range.min = lower;
    range.max = lower + chunk_size;
    lower += chunk_size;
    pthread_mutex_unlock(&lower_lock);
    if (range.min >= biggest)
        pthread_exit(NULL); // No more ranges

    // Send the range to our primes server
    write(fd, &range, sizeof range);
    read(fd, &n, sizeof(int));
    pthread_mutex_lock(&prime_lock);
    nprimes += n;
    pthread_mutex_unlock(&prime_lock);
    }
}

int main(int argc, char *argv[])
{
    int i, sock, nsrv = 0;
    struct hostent *host_info;
    struct sockaddr_in server;
    int opt;

    // process -n and -c command options
    // unrecognised options are silently ignored
    while ((opt = getopt(argc, argv, "n:c:")) != -1) {
    switch(opt) {
    case 'n': biggest = atoi(optarg); break;
    case 'c': chunk_size = atoi(optarg); break;
    }
    }
    argc -= optind;
    argv += optind;
    printf("biggest = %d, chunk_size = %d\n", biggest, chunk_size);

    // Loop over remaining command line arguments (hosts)
    // Attempt to resolve each and connect to its  server
    // Successful  connections  placed in 'active'  table
    // Unsuccessful ones are reported and ignored.

    if (argc == 0) {
    printf("no servers specified -- giving up!\n");
    exit(1);
    }

    // Loop over all potential servers on command line
    for (i = 0; i < argc; i ++) {
    host_info = gethostbyname(argv[i]);
    if (host_info == NULL) {
        printf("cannot resolve %s -- ignoring\n", argv[i]);
        continue;    // Try next one
    }

    // Connect to prime server on this host
    // Create the socket 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("creating stream socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, host_info->h_addr, host_info->h_length);
    server.sin_port = htons(PRIME_PORT);
    if (connect(sock, (struct sockaddr*)&server, sizeof server) < 0) {
        printf("Cannot connect to server %s -- ignoring\n", argv[i]);
        continue;
    }
    printf("connected to %s\n", argv[i]);

    // Spawn a thread to send packets to this server
    pthread_create(&handles[nsrv], NULL, func, &sock);
    nsrv++;
    }

    if (nsrv == 0) {
        printf("no servers found -- giving up!\n");
    exit(3);
    }
    printf("using %d servers\n", nsrv);

    // Wait until all the threads have run out of work
    for (i=0; i < nsrv; i++) {
    pthread_join(handles[i], NULL);
    }

    // Print the answer
    printf("found %d primes\n", nprimes);
}

