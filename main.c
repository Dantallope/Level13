#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <libsocket/libinetsocket.h>

#define PORT "3456"
#define BUFFER_SIZE 4096

void readLine (int fd, char *buffer, int size);
void newLineTrim (char *str);
int connectToServer(const char *hostname);

int main(void){

    int choice;
    int serverChoice;
    char hostname[100];

    printf("Choose a server:\n");
    printf("1. Richmond\n");
    printf("2. London\n");
    printf("Choice: ");
    scanf("%d", &serverChoice);
    getchar();

    if (serverChoice == 1){
        strcpy(hostname, "richmond.cs.sierracollege.edu");
    }else if (serverChoice == 2){
        strcpy(hostname, "london.cs.sierracollege.edu");
    }else{
        printf("Invalid server choice.\n");
        return 1;
    }

    int fd = connectToServer(hostname);

    if (fd == -1){
        printf("Couldn't connect to server.\n");
        return 1;
    }

    char greeting[BUFFER_SIZE];
    readLine(fd, greeting, BUFFER_SIZE);
    printf("Connected: %s\n", greeting);
}

void readLine (int fd, char *buffer, int size){
    int i = 0;
    char ch;

    while (i < size - 1){
        int readBytes = read(fd, &ch, 1);
        
        if (readBytes <= 0 ){
            break;
        }

        buffer[1] = ch;
        i++;

        if (ch == '\n'){
            break;
        }
    }

    buffer[i] = '\0';

    //trimline func
}

int connectToServer(const char *hostname){
    printf("Connecting to %s...\n",hostname);

    int fd = create_inet_stream_socket(
        hostname,
        PORT,
        LIBSOCKET_IPv4,
        0
    );
    return fd;
}