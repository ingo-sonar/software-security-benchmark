#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MESSAGE_BUFFER 512
#define SOCKET_BUFFER 1024

typedef struct ExMessage {
    int type;
    int sender;
    int msgLength;
    char msgBody[MESSAGE_BUFFER];
} ExMessage;

ExMessage* recastBuffer(char *buffer, ExMessage *msg) {
    int *q = (int*)buffer;    
    msg->type = *q;       
    q++;     
    msg->sender = *q;     
    q++;
    msg->msgLength = *q;  
    q++;

    char *p = (char*)q;
    int i = 0;
    while (i < MESSAGE_BUFFER){
        msg->msgBody[i] = *p;
        p++;
        i++;
    }
}

int readMessageFromSocket(char *buffer, char *message) {
    int success;

    // place contents of the buffer into message structure
    ExMessage *msg = (ExMessage*) malloc(sizeof(ExMessage));
    recastBuffer(buffer, msg);

    // copy message body into string for processing
    int index;
    for (index = 0; index < msg->msgLength; index++) {
        message[index] = msg->msgBody[index];
    }

    // to able to print in any case
    message[index] = '\0';
    free(msg);
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    struct sockaddr_in serv_addr, cli_addr;

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR occured on opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listenSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR occured on binding");
    } 
            
    listen(listenSocket, 5);

    socklen_t clilen = sizeof(cli_addr);

    int acceptedSocket = accept(listenSocket, (struct sockaddr *) &cli_addr, &clilen);
    if (acceptedSocket < 0) {
        error("ERROR occured on accepting");
    } 
    
    char buffer[SOCKET_BUFFER], message[MESSAGE_BUFFER];
    bzero(buffer, SOCKET_BUFFER);

    int n = read(acceptedSocket, buffer, SOCKET_BUFFER);

    if (n < 0) {
        error("ERROR occured on reading from socket");
    }

    readMessageFromSocket(buffer, message);

    printf("Here is the message: %s\n", message);
    
    n = write(acceptedSocket, message, MESSAGE_BUFFER);
    
    if (n < 0) { 
        error("ERROR occured on writing to socket");
    }

    close(acceptedSocket);
    close(listenSocket);
    return 0; 
}