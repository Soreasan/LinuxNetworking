/*
Kenneth Adair
rot13 server code.  
*/

#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

/***********************************
No significance to port 1067 except 
that it's not between 0 and 1024.  
Just an arbitrary port
************************************/
#define SERVER_PORT 1067

unsigned char *  readContentsOfFile(char * filename)
{
    printf("File requested is: '%s'\n", filename);
    filename = "FileToOpen.txt";
    char * buffer = 0;
    long length;
    FILE * f = fopen(filename, "rb");
    
    if(f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f) * 4;
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if(buffer)
        {   
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }else{
        printf("Unable to open file.  May not exist or you may not have chmodded it.\n");
    }
 
    if(buffer)
    {   
        return buffer;
    }
}

void getService(int in, int out)
{
    printf("Creating a buffer with an arbitrary size of 1024\n");
    unsigned char buf[1024];
    printf("Creating output which is a character pointer\n");
    unsigned char * output;
    int count;
    while((count = read(in, buf, 1024)) > 0)
    {
        output = readContentsOfFile(buf);
        //write(out, output, count);
        write(out, output, count);
        printf("\n");
    }
}

void main()
{
    int sock, fd, client_len;
    struct sockaddr_in server, client;
    struct servent *service;

    printf("Creating the socket.\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);
    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    printf("listening...\n");

    while(1){
        //client_len = sizeof(client);
        //fd = accept(sock, (struct sockaddr *)&client, &client_len);
        fd = accept(sock, (struct sockaddr *)&client, &client_len);
        printf("got connection\n");
        getService(fd, fd);
        close(fd);
    }
}
