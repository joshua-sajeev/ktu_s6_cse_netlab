#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define WINDOW_SIZE 4
#define PORT 5468
#define BUFFER_SIZE 20

int main()
{
    int server_socket, client_socket, client_addr_len, i, j, status;
    char buffer[BUFFER_SIZE], frame[BUFFER_SIZE], temp[BUFFER_SIZE], ack[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket creation successful\n");

    // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Bind successful\n");

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Listening for connections...\n");

    // Accept client connection
    client_addr_len = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
    if (client_socket < 0) {
        perror("Accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Client connection accepted\n");

    // Get the text to transmit
    printf("Enter the text: ");
    scanf("%s", buffer);

    i = 0;

    while (i < strlen(buffer)) {
        memset(frame, 0, BUFFER_SIZE);
        strncpy(frame, buffer + i, WINDOW_SIZE); // Transmitting frames
        printf("\nTransmitting frames: ");
        int frame_length = strlen(frame);
        strcat(frame, " ");
        for (j = 0; j < frame_length; j++) {
            printf("%d", i + j);
            sprintf(temp, "%d", i + j);
            strcat(frame, temp);
        }

        printf("\n");
        write(client_socket, frame, sizeof(frame));
        read(client_socket, ack, BUFFER_SIZE); // Read acknowledgment

        sscanf(ack, "%d", &status);

        if (status == -1) {
            printf("Transmission successful\n");
            i += frame_length; // Move the window forward
        } else {
            printf("Received error in %d\n", status);
            i = status; // Move the window to the position of the error
        }
    }

    write(client_socket, "Exit", sizeof("Exit"));
    printf("\nExiting\n");

    close(client_socket);
    close(server_socket);

    return 0;
}
