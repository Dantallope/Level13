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
void downloadFile(int fd, const char *filename);
long getFileSize(int fd, const char *filename);
int fileExists(const char *filename);

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
        char filename[256];

        printf("Enter filename for download: ");
        fgets(filename, sizeof(filename),stdin);
        newLineTrim(filename);
        downloadFile(fd, filename);
        
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
    long size;
    char filename[256];

    sendCommand(fd,"LIST\n");
    readLine(fd, line, BUFFER_SIZE);

    if (strncmp(line, "+OK", 3) != 0){
        printf("Server error: %s\n",line);
        return;
    }

    printf("\nAvailable files:\n");
    printf("--------------------------------------\n");
    printf("%-20s %12s\n", "Filename", "Size");
    printf("--------------------------------------\n");

    while(1){
        readLine(fd,line,BUFFER_SIZE);

        if (strcmp(line, ".") == 0){
            break;
        }

        if(sscanf(line, "%ld %255s", &size, filename) == 2){
            printf("%-20s %12ld bytes\n", filename, size);
        }else{
        printf("%s\n", line);
        }
    }


}

long getFileSize(int fd, const char *filename){
    char command[512];
    char response[BUFFER_SIZE];
    long size;

    snprintf(command, sizeof(command), "SIZE %s\n", filename);
    sendCommand(fd, command);

    readLine(fd, response, BUFFER_SIZE);

    if (strncmp(response, "+OK", 3) != 0){
        printf("Server error: %s\n", response);
        return -1;
    }

    if (sscanf(response, "+OK %ld", &size) != 1){
        printf("Couldn't read file size.\n");
        return -1;
    }
    return size;
}

void downloadFile(int fd, const char *filename){
    char command[512];
    char response[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    long fileSize = getFileSize(fd, filename);

    if (fileSize < 0){
        return;
    }

    snprintf(command, sizeof(command), "GET %s\n", filename);
    sendCommand(fd, command);

    readLine(fd, response, BUFFER_SIZE);

    if (strncmp(response, "+OK", 3) != 0) {
        printf("Server error: %s\n", response);
        return;
    }

    FILE *out = fopen(filename, "wb");

    if (out == NULL){
        printf("Couldn't open file for writing.\n");
        return;
    }

    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    long totalRead = 0;

    while (totalRead < fileSize){
        long remaining = fileSize - totalRead;
        int toRead;

        if (remaining < BUFFER_SIZE){
            toRead = remaining;
        }else{
            toRead = BUFFER_SIZE;
        }

        int bytesRead = read(fd, buffer, toRead);

        if (bytesRead <= 0){
            printf("Connection Lost :[");
            break;
        }

        fwrite(buffer, 1, bytesRead, out);
        totalRead += bytesRead;
    }

    gettimeofday(&end,NULL);
    fclose(out);
    
    if (totalRead == fileSize){
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

        double kb = fileSize / 1024.0;
        double speed = kb / elapsed;

        printf("Downloaded %s successfully.\n", filename);
        printf("Size: %.2f KB\n", kb);
        printf("Speed: %.2f KB/sec\n", speed);
    }else{
        printf("Download Failed");
    }
}

int fileExists(const char *filename){
    FILE *fp = fopen(filename, "r");

    if (fp != NULL){
        fclose(fp);
        return 1;
    }
    return 0;
}