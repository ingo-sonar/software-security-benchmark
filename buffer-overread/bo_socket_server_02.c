#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/*
    To be developed into an example; relevant because even if
    ASLR and Propolice protections are used, app could be still 
    vulerable.
 */


// Quote: Listing 2 represents a server that contains a buffer overread and
// a buffer overflow vulnerability that can be abused by an attacker
// to inject code even if this application is protected by both 
// Propolice and ASLR. The application and the exploit were performed
// on an emulated (Qemu 0.9.1) ARMR processor running DebianR
// GNU/LinuxR 4.0, but would be equally applicable to an IA32 
// architecture. However, the ARM architecture was chosen because of
// the growing number of mobile devices that contain such processors,
// making it a more widely used processor even than the IA32
// [22].

void handleConnection(int socket){
    char user[100];
    char pass[200];
    char buff[400];
    int c = 0;
    strncpy(buff, "USER:", 100);
    send(socket, buff, 7, 0);
    recv(socket, buff, 400, 0);
    strncpy(user, buff, 100);
    snprintf(buff, 400, "Hello%s\nPASS:", user);
    c=strlen(buff) + 1;
    send(socket, buff, c, 0);
    recv(socket, buff, 400, 0);
    strcpy(pass, buff);
    strncpy(buff, "Loggedin", 100);
    send(socket, buff, 23, 0);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
}
