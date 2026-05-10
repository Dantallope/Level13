#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <libsocket/libinetsocket.h>

int main(int argc, char *argv[]){
    if (argc != 2)
    {
        printf("Usage: %s server\n" , argv[0]);
        return 1;
    }

    char hostname[100];
    double total = 0;

    strcpy(hostname, argv[1]);
    strcat(hostname, ".cs.sierracollege.edu");

    printf("Connecting to %s\n", hostname);

    int fd = create_inet_stream_socket(hostname, "5055", LIBSOCKET_IPv4, 0);

    if (fd == -1)
    {
        printf("Could not connect to server\n");
        return 1;
    }

    printf("Connected to %s\n", hostname);

    sleep(1);

    for (int i = 0; i<3;i++){
        printf("Sending data %d\n",i + 1);

        struct timeval start, end;
        gettimeofday(&start,NULL);

        char message[] = "hello\n";
        write(fd, message, strlen(message));

        char buffer[100];
        read(fd, buffer, sizeof(buffer));

        gettimeofday(&end,NULL);

        double startTime = start.tv_sec + start.tv_usec / 1000000.0;
        double endTime = end.tv_sec + end.tv_usec / 1000000.0;

        double elapsedMs = (endTime - startTime) * 1000;

        printf("Received data %d: %.2f ms\n", i + 1, elapsedMs);
        total += elapsedMs;


    }
    printf("Ending connectoin\n");
    close(fd);

    printf("Avg RTT: %.2f ms\n", total / 3);
}