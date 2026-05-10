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
void sendCommand(int fd, const char *command);
void listFiles(int fd);

int main(void){

    int choice;
    char choice2;
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

    do {
        printf("\nMenu\n");
        printf("L)ist files\n");
        printf("D)ownload a file\n");
        printf("Q)uit\n");
        
        scanf("%c",&choice2);

    getchar();

    if (choice2 == 'L'){
        listFiles(fd);
    }else if (choice2 == 'D'){
        //downloadfile
    }else if (choice2 == 'Q'){
        //quit
    }else{
        printf("Invalid menu selection");
    }
} while(choice != 'Q');

close(fd);
}

void readLine (int fd, char *buffer, int size){
    int i = 0;
    char ch;

    while (i < size - 1){
        int readBytes = read(fd, &ch, 1);
        
        if (readBytes <= 0 ){
            break;
        }

        buffer[i] = ch;
        i++;

        if (ch == '\n'){
            break;
        }
    }

    buffer[i] = '\0';

    newLineTrim(buffer);
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

void newLineTrim(char *str){
    int len = strlen(str);

    if (len > 0 && str[len-1] == '\n'){
        str[len - 1] = '\0';
    }
}

void sendCommand(int fd, const char *command){
    write(fd, command, strlen(command));
}

void listFiles(int fd){
    char line[BUFFER_SIZE];

    sendCommand(fd,"LIST\n");
    readLine(fd, line, BUFFER_SIZE);

    // if (strncmp(line, "+OK", 3) != 0){
    //     printf("Server error: %s\n",line);
    //     return;
    // }

    printf("\nAvailable files:\n");
    printf("%-15s %s\n", "Size", "Filename");

    while(1){
        readLine(fd,line,BUFFER_SIZE);

        if (strcmp(line, ".") == 0){
            break;
        }
        printf("%s\n", line);
    }


}