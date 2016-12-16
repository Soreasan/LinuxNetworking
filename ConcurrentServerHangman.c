/*
Kenneth Adair 
Concurrent hangman server 
Taken from Linux Networking course on Pluralsight by Dr. Chris Brown
*/

#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

extern time_t time();

/*  Retrieves the words in the file "words" and moves them into an array.
    Not best practices but is the way it's done in this example.  
    It's basically the same as typing out all the words. */
char *word[] = {
    #include "words"
};

/*  This is the size of the word array in bytes 
    divided by the size in bytes of a single word.
    This let's us calculate how many elements are
    in the array. */
#define NUM_OF_WORDS    (sizeof(word)/sizeof(word[0]))
#define MAXLEN  80  /* Maximum size of any string in the world */
//Port the user will connect to.  
#define HANGMAN_TCP_PORT    1068


void play_hangman(int in, int out);

int main(int argc, char * argv[])
{
    int sock, msgsock, client_len;
    struct sockaddr_in server, client;

    //This line of code prevents zombies.
    signal(SIGCHLD, SIG_IGN);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("creating stream socket");
        exit(1);
    }

    //Filling in our sockaddr_in structure
    //AF_INET tells our socket it will communicate using IPv4 addresses
    server.sin_family      = AF_INET;
    // INADDR_ANY is the "any address" in IPv4.  
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    //HANGMAN_TCP_PORT is 1068 which we defined above.
    server.sin_port        = htons(HANGMAN_TCP_PORT);

    //Bind the port number to the socket and exit if we can't do it
    if (bind(sock, (struct sockaddr *) &server, sizeof (server)) < 0) 
    {
        perror("binding socket");
        exit(2);
    }

    //Set up a listen queue
    listen(sock, 5);
    //This let's us see what's happening
    fprintf(stderr, "hangman server: listening...\n");

    //Here we enter the service loop
    while (1) 
    {
        //This is just used to help us create the msgsock to accept connections
        client_len = sizeof(client);
        //This let's us accept incoming connections
        msgsock = accept(sock, (struct sockaddr *) &client, &client_len);
        if (msgsock < 0) 
        {
            perror("accepting connection");
            exit(3);
        }
        //If fork returns 0 that means it's the child
        if (fork() == 0) 
        {
            close(sock);        /* Child -- Process Request */
            printf("new child (pid %d) using descriptor %d\n", getpid(), msgsock);
            //random number generator that's used to pick a word
            srand((int)time((long *)0));    /* Randomise the seed */
            //One msgsock is used for input, the other for output
            play_hangman(msgsock, msgsock);
            printf("child (pid %d) exiting\n", getpid());
            exit(0);
        }
        else
            close(msgsock); /* If we're the parent we close and loop around */
    }

    /* done */
    return 0;
}

/* ------------------ play_hangman() ------------------------ */
/* Plays one game of hangman,  returning when the  word has been
   guessed or all the player's "lives" have been used.  For each
   "turn"  of the game,  a line is read  from  stream "in".  The
   first character of this line is  taken as the player's guess.
   After each guess and prior to the first guess, a line is sent
   to stream "out". This consists of the word as guessed so far,
   with - to show unguessed letters,   followed by the number of
   lives remaining.
   Note that this function neither knows nor cares  whether its
   input and output streams refer to sockets, devices, or files.
*/
void play_hangman(int in, int out)
{
    char *whole_word, part_word[MAXLEN], guess[MAXLEN], outbuf[MAXLEN];
    int lives   = 12;       /* Number of lives left */
    int game_state = 'I';   /* I ==> Incomplete     */
    int i, good_guess, word_length;

    /* Pick a word at random from the list */
    whole_word = word[rand() % NUM_OF_WORDS];
    word_length = strlen(whole_word);

    /* No letters are guessed initially */
    for (i=0; i < word_length; i++)
        part_word[i] = '-'; /* Loop through and fill the array with dashes to display  */
    part_word[i] = '\0'; /* Last slot of the array is filled with a null terminator or whatever */

    sprintf(outbuf, " %s   %d\n", part_word, lives);
    write(out, outbuf, strlen(outbuf));

    while ( game_state == 'I')
    { 
        read(in, guess, MAXLEN);    /* Get guess letter from player */
        good_guess = 0;
        for (i=0; i<word_length; i++)
        { 
            if (guess[0] == whole_word[i])
            { 
                good_guess = 1;
                part_word[i] = whole_word[i];
            }
        }
        if (! good_guess) 
            lives--;
        if (strcmp(whole_word, part_word) == 0)
            game_state = 'W';       /* W ==> User Won */
        else if (lives == 0)
        { 
            game_state = 'L';       /* L ==> User Lost */
            strcpy(part_word, whole_word);  /* Show User the word */
        }
        sprintf(outbuf, " %s   %d\n", part_word, lives);
        write(out, outbuf, strlen(outbuf));
    }
}
