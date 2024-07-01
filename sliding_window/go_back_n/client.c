#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 5468
#define BUFFER_SIZE 20

int main()
{
    int client_socket, choice;
    char buffer[BUFFER_SIZE], error[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket creation successful\n");

    // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");

    while (1) {
        // Receive frame
        read(client_socket, buffer, BUFFER_SIZE);

        if (!strcmp(buffer, "Exit")) {
            printf("\nExiting\n");
            break;
        }

        printf("\nReceived %s", buffer);
        printf("\nDo you want to report error? (1-Yes, 0-No): ");
        scanf("%d", &choice);

        if (!choice) {
            write(client_socket, "-1", sizeof("-1"));
        } else {
            printf("Enter the sequence of frame where the error has occurred: ");
            scanf("%s", error);
            // Send acknowledgment
            write(client_socket, error, sizeof(error));
        }
    }

    close(client_socket);

    return 0;
}
