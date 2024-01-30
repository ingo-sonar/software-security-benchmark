#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int returnChunkSize(int chunkSize, int buffSize) {
    //  return -1 to indicate an error
    if (chunkSize > buffSize) return -1;
    else if ((buffSize % chunkSize) == 0) return chunkSize;
    else return buffSize;
}

void print(char *buffer, int len) {
    for (int i=0; i<len; i++) {
        if (buffer[i] != 0) {
            printf("%c", buffer[i]);
        }
    }
    printf("\n");
}
 
int main(int argc, char **argv) {

    if(argc < 2) {
        printf("no parameters specified\n");
        exit(-1);
    }

    // convert first parameter to an integer
    int chunk = atoi(argv[1]);

    char public[32];
    char secret[32];
    
    // copy public and private strings into buffers
    strcpy(public, "This is public data");
    strcpy(secret, "This is a secret");
 
    // allocate a local buffer reserve one byte for \0 symbol
    char buffer[chunk+1];
 
    // global buffer overread may occur here if `len` is more
    // than length of `public` e.g.: uint - 1 = 4294967295.
    memcpy(buffer, public, returnChunkSize(chunk, 32));
    buffer[chunk] = 0;

    printf("buffer by memcpy:\n");
    print(buffer, chunk);

}