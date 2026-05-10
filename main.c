#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <libsocket/libinetsocket.h>

#define PORT "3456"
#define BUFFER_SIZE 4096

int main(void){

    int choice;
    int serverChoice;
    char hostname[100];

    printf("Choose a server:\n");
    printf("1. richmond.cs.sierracollege.edu\n");
    printf("2. london.cs.sierracollege.edu\n");
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
}