#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsocket/libinetsocket.h>

void trim(char string[100])
{
    int i = 0;
    while(string[i] != '\n' && string[i] != '\0')
    {
        i++;
    }
    string[i] = '\0';
}

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

    while (strcmp(input, "q") != 0 && strcmp(input, "Q") !=0)
    {
        printf("L)ist files\n");
        printf("D)ownload\n");
        printf("Q)uit\n");
        printf("What would you like to do?");
        scanf("%s", input);

        if (strcmp(input, "l") == 0 || strcmp(input, "L") == 0)
        {
            char *size;
            char *filename;
            char spaces[100] = "";
            int numOfSpaces;
            fprintf(server, "LIST\n");
            fgets(line, 100, server);
            fgets(line, 100, server);
            trim(line);
            printf("Filename                  Size\n");
            printf("------------------------------\n");

            while (line[0] != '0')
            {
                size = strtok(line, " ");
                filename = strtok(NULL, " ");

                numOfSpaces = 30 - strlen(size) - strlen(filename);
                for(int i=0; i<numOfSpaces; i++)
                {
                    strcat(spaces, " ");
                }
                printf("%s%s%s\n", filename, spaces, size);
                strcpy(spaces, "");

                fgets(line, 100, server);
                trim(line);
            }
            fgets(line, 100, server);
        }
        else if (strcmp(input, "d") == 0 || strcmp(input, "D") == 0)
        {
            char filename[100];
            int size = 0;
            int count = 0;
            printf("Please enter the filename:");
            scanf("%s", filename);
            fprintf(server, "SIZE %s\n", filename);
            fgets(line, 100, server);
            if (strstr(line, "-ERR") != NULL)
            {
                printf("Error: File not found\n");
            }
            else
            {
                size = atoi(strtok(line, "+OK "));
                printf("%d", size);
                FILE* writeFile = fopen(filename, "w");
                fprintf(server, "GET %s\n", filename);
                fgets(line, 100, server);
                while (count<size)
                {
                    fgets(line, 100, server);
                    count += strlen(line);
                    fprintf(writeFile, "%s", line);
                }
                fclose(writeFile);
            }
        }
    }
    fclose(server);
}