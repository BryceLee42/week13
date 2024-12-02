#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsocket/libinetsocket.h>

int main()
{
    char hostname[100];
    char line[100];
    char input[100];
    int set = 0;

    while (set == 0)
    {
        printf("Please enter which server to connect to (london/newark):");
        scanf("%s", input);

        if (strcmp(input, "london") == 0)
        {
            strcpy(hostname, "london.cs.sierracollege.edu");
            set = 1;
        }
        else if (strcmp(input, "newark") == 0)
        {
            strcpy(hostname, "newark.cs.sierracollege.edu");
            set = 1;
        }
        else
        {
            printf("Please enter either london or newark.");
        }
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