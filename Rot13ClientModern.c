/*
Kenneth Adair
This version of the client uses getaddrinfo() rather than the older resolvers.
This one also uses messages from the commandline rather than stdin
This one has better error checking than the other client.

To run this while the TcpRot13Sever.c compiled file is running type:
./client localhost 1067 apple orange banana
(This assumes you named the outputted file client)
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int main(int argc, char *argv[])
{
    //Here we're declaring our address structures
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];

    //Here we're making sure our users provide enough commandline arguments
    if(argc < 3){
        fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Obtain address(es) matching host/port */

    //Filling in hints address structure
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket (TCP) */
    hints.ai_flags = 0;
    hints.ai_protocol = 0; /* Any protocol */

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    //If getaddrinfo fails this prints out the error message, the int s is used for error checking
    if(s != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /*  getaddrinfo() returns a list of address structures.
        Try each address until we successfully connect(2).
        if socket(2) (or connect(2)) fails, we (close the socket
        and) try the next address */

    //Looping through linked list using the pointer
    for(rp = result; rp != NULL; rp = rp->ai_next){
        //Try to create the socket and continue if it fails
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd == -1)
            continue;

        //If it succeeds we create the connection and break;
        if(connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;  /* Success  */

        //Otherwise we close it and try the next socket
        close(sfd);
    }

    //This is the error handling for if the list returns no correct addresses
    if(rp == NULL){     /* No address succeeded  */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    //Frees up linked list of address structures
    freeaddrinfo(result);   /* No longer needed */

    /*  Send each remaining command-line argument seperately
        to the server, then read and display the response. */

    //Looping through remainder of commandline arguments
    for(j = 3; j < argc; j++){
        len = strlen(argv[j]) + 1;
            /* +1 for terminating null byte */

        //Make sure the commandline arguments aren't longer than the buffer we allocated.
        if(len + 1 > BUF_SIZE){
            fprintf(stderr, "Ignoring long message in argument %d\n", j);
            continue;
        }

        if(write(sfd, argv[j], len) != len){
            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        nread = read(sfd, buf, BUF_SIZE);
        if(nread == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Received %1d bytes: %s\n", (long) nread, buf);
    }
    exit(EXIT_SUCCESS);
}
