#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

int validateNumber(const char *s){
    while (*s) {
        if (*s < '0' || *s > '9') return 0;
        *s++;
    }

    return 1;
}

int validateIPv4(char *ip) {  // To check whether the IP is valid or not
    int i, num, dots = 0;
    
    char *ptr;
    if (ip == NULL) {
        return 0;
    }
    ptr = strtok(ip, "."); // Cut the string using dor delimiter
    if (ptr == NULL) {
        return 0;
    }

    while (ptr) {
        // Check whether the sub string is holding only number or not
        if (!validateNumber(ptr)) {
            return 0;
        }
        num = atoi(ptr); // Convert substring to number
        if (num >= 0 && num <= 255) {
            ptr = strtok(NULL, "."); // Cut the next part of the string
        }                               
        if (ptr != NULL) {
            dots++; // Increase the dot count
        } 
        else {
            return 0;
        } 
    }

    if (dots != 3) {  // If the number of dots are not 3, return false
        return 0; 
    }
    return 1;
}

int hostLookup(char *userSppliedAddr, char *hostName){
    // Ensures userSppliedAddr is in the right format
    validateIPv4(userSppliedAddr);

    in_addr_t *addr = inet_addr(userSppliedAddr);
    struct hostent *hp = gethostbyaddr(addr, sizeof(struct in_addr), AF_INET);

    strcpy(hostName, hp->h_name);
    return strlen(hp->h_name);
}

void print(char *buffer, unsigned int len) {
    for (int i=0; i<len; i++) {
        if (buffer[i] != 0) {
            printf("%c", buffer[i]);
        }
    }
    printf("\n");
}
 
int main(int argc, char **argv) {

    char hostName[64];
    char userSppliedAddr[15];

    if(argc < 2) {
        printf("no parameters specified\n");
        exit(-1);
    }

    strncpy(userSppliedAddr, argv[1], sizeof(userSppliedAddr));
    int len = hostLookup(userSppliedAddr, hostName);
    print(hostName, len);
}
