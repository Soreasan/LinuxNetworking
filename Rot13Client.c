/*
Kenneth Adair
Example taken from Linux Networking class by Chris Brown on Pluralsight
*/

/* Client for rot13 server  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define BSIZE 1024

main(int argc, char *argv[])
{
    struct hostent *host_info;
    struct sockaddr_in server;
    int sock, count;
    char buf[BSIZE]; /* Copy buffer */
    char *server_name;

    /* Get server name from command line.  If none, use "localhost" */
    server_name = (argc>1 ? argv[1] : "localhost");

    /* Create the socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("creating stream socket");
        exit(1);
    }

    /* Look up the host's IP address  */
    host_info = gethostbyname(server_name);
    if(host_info == NULL){
        fprintf(stderr, "%s: unknown host: %s\n", argv[0], server_name);
        exit(2);
    }

    /* Set up the server's socket address  */
    server.sin_family = AF_INET;
    memcpy((char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
    server.sin_port = htons(1067);

    /* Connect to the server */
    if(connect(sock, (struct sockaddr *)&server, sizeof server) < 0){
        perror("connecting to server");
        exit(4);
    }

    /*  OK now we are connected to the server.  Send the user name, then
        collect whatever the server sends back and copy it to stdout.
    */

    printf("connected to server %s\n", server_name);
    while((count = read(0, buf, BSIZE)) > 0){
        write(sock, buf, count);
        read(sock, buf, count);
        write(1, buf, count);
    }
}
