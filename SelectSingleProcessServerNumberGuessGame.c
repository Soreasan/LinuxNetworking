/*  Kenneth Adair
    Taken from Linux Networking course from Pluralsight. 
    Number-guessing server. This is an example of a
    single-process, concurrent server using TCP and select()  */
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

//port our socket will use.
#define GUESS_PORT      1500    /* Our "well-known" port number */

/* Function Prototypes */
int process_request(int fd);

// This is where we store the per-client state (the number to guess)
// This array is indexed by the connection descriptor
int target[FD_SETSIZE];

/* -------------------------- main() -------------------------- */
int main( int argc, char* argv[] )
{
    int sock, fd;
    int client_len;
    struct sockaddr_in server, client;

    /* These are the socket sets used by select()  */
    fd_set test_set, ready_set;

    printf("FD_SETSIZE = %d\n", FD_SETSIZE);
    /* Mark all entries in the target table as free */
    for (fd=0; fd<FD_SETSIZE; fd++)  
        target[fd] = 0;

    /* Create the rendezvous socket & bind the well-known port */
    /* This is standard boiler-plate code for a server */
    sock = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
    //This line makes our socket a IPv4 socket
    server.sin_family      = AF_INET;
    //Bind to any IPv4 address
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(GUESS_PORT);

    if (bind(sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }

    //Set up the listening queue
    listen(sock, 5);

    /* Initially, the "test set" has in it just the rendezvous descriptor */
    FD_ZERO(&test_set);
    FD_SET(sock, &test_set);
    printf("server ready\n");

    /* Here is the head of the main service loop */
    while (1) {
        /* Because select overwrites the descriptor set, we must
           not use our "permanent" set here, we must use a copy.
        */
        memcpy(&ready_set, &test_set, sizeof test_set);
        //We're only interested in the ready for reading set and ignore the other three sets
        select(FD_SETSIZE, &ready_set, NULL, NULL, NULL);

        /* Did we get a new connection request? If so, we accept it,
           add the new descriptor into the read set, choose a random
           number  for this client to guess
        */
        if (FD_ISSET(sock, &ready_set)) {
            client_len = sizeof client;
            fd = accept(sock, (struct sockaddr *)&client, &client_len);
            FD_SET(fd, &test_set);
            printf("new connection on fd %d\n", fd);
            target[fd] = 1 + rand()%1000;
        }

        /* Now we must check each descriptor in the read set in turn.
           For each one which is ready, we process the client request.
        */
        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (fd == sock) 
                continue;  // Omit the rendezvous descriptor
            if (target[fd] == 0) 
                continue;  // Empty table entry
            if (FD_ISSET(fd, &ready_set)) {
                if (process_request(fd) < 0) {
                    /* If the client has closed its end of the connection,
                       or if the dialogue with the client is complete,  we
                       close our end,  and remove the  descriptor from the
                       read set
                    */
                    close(fd);
                    FD_CLR(fd, &test_set);
                    target[fd] = 0;
                    printf("closing fd=%d\n", fd);
                }
            }
        }
    }
    return 0;           /* Keep compiler happy! */
}

/* This is the application-specific part of the code */

/* ------------------- process_request() ------------------ */
/* Return 0 for normal return, or -1 if encountered EOF
   from client or if number was guessed correctly
*/
int process_request(int fd)
{
    int guess;
    char inbuffer[100], *p = inbuffer;

    // Read a guess from the client

    if (read(fd, inbuffer, 100) <= 0)
            return -1;  // Client closed connection?

    guess = atoi(inbuffer);
    printf("received guess %d on fd=%d\n", guess, fd);

    if (guess > target[fd]) {
        write(fd, "too big\n", 8);
        return 0;
    }
    if (guess < target[fd]) {
        write(fd, "too small\n", 10);
        return 0;
    }
    write(fd, "correct!\n", 9);
    return -1;
}
