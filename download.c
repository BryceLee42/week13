#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsocket/libinetsocket.h>

int main(int argc, char *argv[])
{
    char hostname[100];
    char line[100];

    if (argc != 2)
    {
        printf("Please enter either london or newark.");
        exit(1);
    }

    if (strcmp(argv[1], "london") == 0)
    {
         strcpy(hostname, "london.cs.sierracollege.edu");
    }
    else if (strcmp(argv[1], "newark") == 0)
    {
         strcpy(hostname, "newark.cs.sierracollege.edu");
    }
    else
    {
        printf("Please enter either london or newark.");
        exit(1);
    }

    int fd = create_inet_stream_socket(hostname, "3456", LIBSOCKET_IPv4, 0);

    if (fd<0)
    {
        printf("Failed to connect to server.\n");
        exit(1);
    }

    printf("Connected to %s\n", hostname);

    FILE *server = fdopen(fd, "r+");

    fgets(line, 100, server);
    printf("%s", line);
    fprintf(server, "HELO\n");
    fgets(line, 100, server);
    printf("%s", line);
    fprintf(server, "PING\n");
    fgets(line, 100, server);
    printf("%s", line);


}