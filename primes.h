/*
Kenneth Adair
These are the things that the server and client
need to agree on to count the primes.
Taken from linux Networking course on pluralsight. 
*/

#define PRIME_PORT 1070

struct subrange {
    int min;
    int max;
};

