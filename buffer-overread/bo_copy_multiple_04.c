#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
 
void copy1(char *src, char *dest, int len) {
    for (int i=0; i<len; i++) {
        dest[i] = src[i];
    }
}

void copy2(char *src, char *dest, int len) {
    memcpy(dest, src, len*sizeof(char));
}

void copy3(char *src, char *dest, int len) {
    memmove(dest, src, len*sizeof(char));
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

    // Initialization, should only be called once.
    srand(time(NULL));


    // convert first parameter to an integer
    int len = atoi(argv[1]);

    char public[32];
    char secret[32];
    
    // copy public and private strings into buffers
    strcpy(secret, "This is a secret");
    strcpy(public, "This is public data");

    // allocate a local buffers reserve one byte for \0 symbol
    char buffer1[len+1];
    char buffer2[len+1];
    char buffer3[len+1];
 
    // global buffer overread may occur here if `len` is more
    // than length of `public`
    for (int i=0;i<1024;i++) {

        if (rand() % 2==0) {
            copy1(public, buffer1, len);
        }

        if (rand() % 2 == 1) {
            copy2(public, buffer2, len);
        }

        copy3(public, buffer3, len);
    }

    buffer1[len] = buffer2[len]=  buffer3[len]='\0';

    printf("buffer1 by copy1:\n");
    print(buffer1, len);

    printf("buffer2 by copy2:\n");
    print(buffer2, len);

    printf("buffer2 by copy3:\n");
    print(buffer3, len);
}