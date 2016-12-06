/*
Kenneth Adair
A simple program that reads the contents of a file
Taken from:
http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *  readContentsOfFile(char * filename)
{
    char * buffer = 0;
    long length;
    FILE * f = fopen(filename, "rb");
    
    if(f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
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

int main()
{
    char * filename = "ReadFile.c";
    printf("%s\n", readContentsOfFile(filename));
    return 0;
}
