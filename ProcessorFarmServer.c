/*
Kenneth Adair
Server code
Pretty conventional server without multithreads or concurrency.
The idea is the client will deal with that side of things.
Taken from Linux Networking course on Pluralsight.
*/

#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "primes.h"

//Our method will determine if a number is prime
int isprime(int n)
{
    int i;
    for (i=2; i*i <= n; i++)
        if (n%i == 0) return 0;
    return 1;
}

//this method calls the above method to count how many primes are in a number range.
int count_primes(int min, int max)
{
    int i, count=0;
    for (i=min; i<max; i++)
        count += isprime(i);
    printf("range %8d - %8d : count = %d\n", min, max, count);
    return count;
}

void main()
{
    //Below is the boiler plate code to establish a server side socket.
    int sock, fd, client_len, count;
    struct sockaddr_in server, client;
    struct subrange limits;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
    perror("creating socket");
    exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PRIME_PORT);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("cannot bind");
        exit(2);
    }

    listen(sock, 5);
    printf("listening on port %d ...\n", PRIME_PORT);

    //The process
    while (1) {
    client_len = sizeof(client);
    fd = accept(sock, (struct sockaddr *)&client, &client_len);
        if (fd < 0) {
            perror("accepting connection");
            exit(3);
        }

    printf("got connection\n");
        while(1) {
        if (read(fd, &limits, sizeof limits) != sizeof limits)
        break;  // Assume "end of file", drop the connection
        count = count_primes(limits.min, limits.max);
        write(fd, &count, sizeof count);
        }
    close(fd);
    }
}

