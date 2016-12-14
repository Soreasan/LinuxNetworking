/*
Kenneth Adair
I wasn't able to get this one working.  =(
Taken from Linux Network Programming course on Pluralsight.

Remote cat client using TFTP Server (UDP socket implementation).
Usage: rcat hostname filename

This program assumes that you've done the following:
1. sudo apt-get install xinetd ftfpd
2. vim /etc/xinetd.d/tftp
The file created for step 2 looks like this:
service tftp
{
    protocol = udp
    port = 69
    socket_type = dgram
    wait = yes
    user = nobody
    server = /usr/sbin/in.tftpd
    server_args = /tftpboot
    disable = no
}
3. sudo mkdir /tftpboot
4. sudo chmod 777 /tftpboot/
5. sudo chown nobody /tftpboot/
6. sudo /etc/init.d/xinetd restart
7. sudo lsof -i | grep tftp
8. cp /etc/services /tftpboot/
*/

#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>

#define TFTP_PORT   69          /* tftp's well-known port number */
#define BSIZE       600         /* size of our data buffer */
#define MODE        "octet"

#define OP_RRQ      1           /* TFTP op-codes */
#define OP_DATA     3
#define OP_ACK      4
#define OP_ERROR    5

int main(int argc, char *argv[])
{
    int sock;                   /* Socket descriptor */
    struct sockaddr_in server;  /* Server's address */
    struct hostent *host;       /* Server host info */
    char buffer[BSIZE], *p;     /* buffer to send and receive packets */
    int count, server_len;

    //Error checking to make sure we have correct number of arguments
    if(argc != 3){
        fprintf(stderr, "usage: %s hostname filename\n", argv[0]);
        exit(1);
    }

    /* Create a datagram socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    /* Get the server's address */
    host = gethostname(argv[1]);
    if(host == NULL){
        fprintf(stderr, "unknown host: %s\n", argv[1]);
        exit(2);
    }
    server.sin_family   = AF_INET;
    memcpy(&server.sin_addr.s_addr, host->h_addr, host->h_length);
    server.sin_port = htons(TFTP_PORT);

    /*  Build a tftp Read Request packet.  This is messy because the
        fields have variable length, so we can't use a structure.
        Note: pointer p is one past the end of the full packet
        when done!
    */
    *(short *)buffer = htons(OP_RRQ);   /* The op-code */
    p = buffer + 2;
    strcpy(p, argv[2]);     /* The file name */
    p += strlen(argv[2]);   /* Keep the nul */
    strcpy(p, MODE);        /* The Mode */
    p += strlen(MODE) + 1;

    /* Send Read Request to tftp server */
    count = sendto(sock, buffer, p-buffer, 0, (struct sockaddr *)&server, sizeof server);

    /*  Loop, collecting data packets from the server, until a short
        packet arrives.  This indicates the end of the file.  
    */
    do{
        server_len = sizeof server;
        count = recvfrom(sock, buffer, BSIZE, 0, (struct sockaddr *)&server, &server_len);
        if(ntohs(*(short *)buffer) == OP_ERROR){
            fprintf(stderr, "rcat: %s\n", buffer+4);
        }
        else{
            write(1, buffer+4, count-4);
            /*  Send an ack packet.  The block number we want to ack is
                already in teh buffer so we just need to change the
                opcode.  Note that the ACK is sent to the port number
                which the server just sent the data from, NOT to port
                69
            */
            *(short *)buffer = htons(OP_ACK);
            sendto(sock, buffer, 4, 0, (struct sockaddr *)&server, sizeof server);
        }
    }while(count == 516);  
    /*  We carry on looping as long as the block is 516 bytes
        512 for the data, 2 bytes for the opcode and 2 bytes for the block number
        When we get a shorter packet than that we know there are no more and we
        fall out the loop and then we're done.
    */

    /* done */
    return 0;
}
