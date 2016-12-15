/*
Kenneth Adair

Wasn't able to get this one to work either.  =(

To compile do this:
gcc [filename] -o [output filename] -lncurses

Taken from Linux Network Programming
Broadcast peer-to-peer service using sockets
This solution generates the x, y values randomly
and displays the received values in a table
*/

#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
/* sudo apt-get install libncurses5-dev */
#include <ncurses.h>
#include <string.h>

#define UPDATE_PORT 2066

/* Define the broadcast packet format */
typedef struct packet{
    char text[64];
    int x;
    int y;
} Packet;

/* Function prototypes */
void display_update(Packet p);

int main(int argc, char *argv[])
{
    int sock;                   /* Socket descriptor */
    struct sockaddr_in server;  /* Broadcast address */
    struct sockaddr_in client;
    int client_len, yes = 1;
    Packet pkt;

    /* Initialize the curses package */
    initscr();
    cbreak();   /* Disable input buffering */
    noecho();   /* Disable echoing */

    /* Create a datagram socket and enable broadcasting */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof yes);

    /* Bind our well-known port number  */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(UPDATE_PORT);
    if(bind(sock, (struct sockaddr *)&server, sizeof server) < 0){
        printw("server: bind failed\n"); refresh(); exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = 0xffffffff;
    server.sin_port = htons(UPDATE_PORT);

    /*  Create an additional process.  The parent acts as the client,
        periodically broadcasting updates to ayone who happens to be
        listening on port 2066.  The child acts as the server,
        receiving the broadcasts and displaying the data.
    */
    if(fork()){ /* PARENT (client) here */
        if(argc>1) strcpy(pkt.text, argv[1]);
        else gethostname(pkt.text, 64);
        pkt.x = 40; pkt.y = 12;

        while(1){
            pkt.x = rand() % 80;
            pkt.y = rand() % 24;
            /* Broadcast update packet to servers */
            sendto(sock, (char *)&pkt, sizeof pkt, 0, 
                    (struct sockaddr *)&server, sizeof server);
            sleep(1);
        }
    }   /* End of parent (client) code */

/* --------------------------------------------------------- */
    else{   /* CHILD (server) here */
        /*  Enter our service loop, receiving packets
            and displaying them in some appropriate way.
        */
        while(1){
            /* Receive an update packet */
            client_len = sizeof client;
            recvfrom(sock, (char *)&pkt, sizeof pkt, 0,
                    (struct sockaddr *)&client, &client_len);

            /* Display the packet's contents */
            display_update(pkt);
        }
    }   /* End of child (server) code */
    return 0;   /* Keeps the compiler happy */
}

/*  This version displays p.text, p.x, and p.y in a table indexed by
    the text name.
*/
void display_update(Packet p)
{
#define TSIZE 50

    static char table[TSIZE][64];
    static int entries = 0;
    int i, found;

    /*  Search the table for an entry with a matching text string.  If
        found, the ordinal position of the entry in the table is used to
        determine the position on the screen at which this entry will be
        displayed.  The x, y values are only displayed; they are NOT kept
        in the table.  If no matching entry is found and the table is not 
        full, a new entry is created.
    */

    for(i = 0, found = 0; i < entries; i++){
        if(strcmp(p.text, table[i]) == 0){
            found = 1;      /* Found a matching entry */
            break;
        }
    }
    if(!found){
        if(entries == TSIZE)    /* Table is full */
            return;
        strcpy(table[entries], p.text);
        i = entries++;  /* Create new table entry */
    }
    /* Update the display of the i'th entry in the table */
    move(i, 1);
    printw("%16s %4d %4d", p.text, p.x, p.y);
}
