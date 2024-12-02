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
        printf("(L) List files\n");
        printf("(D) Download\n");
        printf("(DA) Download All\n");
        printf("(Q) Quit\n");
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
            int transferred;
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
                FILE* readFile = fopen(filename, "r");
                char deleteFileChar = 'A';
                int runCode = 1;
                if (readFile!=NULL)
                {
                    printf("You are about to delete the current %s and replace it with a new %s. Are you sure you want to continue? (Y/N)", filename, filename);
                    while(deleteFileChar != 'Y' && deleteFileChar != 'N')
                    {
                        scanf("%c", &deleteFileChar);
                    }
                    if (deleteFileChar == 'Y')
                    {
                        runCode = 1;
                    }
                    else if (deleteFileChar == 'N')
                    {
                        runCode = 0;
                    }
                }
                if (runCode == 1)
                {
                    size = atoi(strtok(line, "+OK "));
                    char buffer[1000];
                    transferred = 0;
                    fprintf(server, "GET %s\n", filename);
                    fgets(line, 100, server);
                    FILE* writeFile = fopen(filename, "w");
                    while(transferred<size)
                    {
                        if(size-transferred<1000)
                        {
                            fread(buffer, sizeof(char), size-transferred, server);
                            fwrite(buffer, sizeof(char), size-transferred, writeFile);
                            transferred += size-transferred;
                        }
                        else
                        {
                            fread(buffer, sizeof(char), 1000, server);
                            fwrite(buffer, sizeof(char), 1000, writeFile);
                            transferred += 1000;
                        }
                    }
                    
                    fclose(writeFile);
                }
            }
        }
        if (strcmp(input, "da") == 0 || strcmp(input, "DA") == 0)
        {
            int numOfFiles = 0;
            char *filename;
            char ** filenames = malloc(15*sizeof(char *));
            int fileSizes[15];
            int size = 0;
            int transferred;

            fprintf(server, "LIST\n");
            fgets(line, 100, server);
            fgets(line, 100, server);
            while (line[0] != '0')
            {
                size = atoi(strtok(line, " "));
                filename = strtok(NULL, " ");
                trim(filename);
                filenames[numOfFiles] = malloc(strlen(filename+1));
                strcpy(filenames[numOfFiles], filename);
                fileSizes[numOfFiles] = size;

                numOfFiles += 1;
                fgets(line, 100, server);
            }
            fgets(line, 100, server);

            for(int i=0; i<numOfFiles; i++)
            {
                size = fileSizes[i];
                char buffer[1000];
                transferred = 0;
                fprintf(server, "GET %s\n", filenames[i]);
                fgets(line, 100, server);
                FILE* writeFile = fopen(filenames[i], "w");
                while(transferred<size)
                {
                    if(size-transferred<1000)
                    {
                        fread(buffer, sizeof(char), size-transferred, server);
                        fwrite(buffer, sizeof(char), size-transferred, writeFile);
                        transferred += size-transferred;
                    }
                    else
                    {
                        fread(buffer, sizeof(char), 1000, server);
                        fwrite(buffer, sizeof(char), 1000, writeFile);
                        transferred += 1000;
                    }
                }
                fclose(writeFile);
            }
            free(filenames);
        }
    }
    fclose(server);
}
