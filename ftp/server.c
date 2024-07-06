#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 2003
#define BUFFER_SIZE 1024

int main() {
    struct sockaddr_in server, client;
    char buffer[BUFFER_SIZE];
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("Socket creation failed");
        return 1;
    }
    printf("Socket creation successful\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Binding failed");
        close(s);
        return 1;
    }
    printf("Binding successful\n");

    if (listen(s, 1) == -1) {
        perror("Listening failed");
        close(s);
        return 1;
    }
    printf("Listening...\n");

    socklen_t len = sizeof(client);
    int client_sock = accept(s, (struct sockaddr*)&client, &len);
    if (client_sock == -1) {
        perror("Accept failed");
        close(s);
        return 1;
    }
    printf("Connection successful\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (recv_len <= 0) {
            perror("Recv failed");
            break;
        }

        printf("Requested file: %s\n", buffer);
        FILE *fp = fopen(buffer, "rb");
        if (fp == NULL) {
            if (send(client_sock, "ERROR: File not found", 21, 0) == -1) {
                perror("Send failed");
            }
            continue;
        }

        while ((recv_len = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
            if (send(client_sock, buffer, recv_len, 0) == -1) {
                perror("Send failed");
                break;
            }
        }
        fclose(fp);

        // Indicate the end of file transfer
        if (send(client_sock, "EOF", 3, 0) == -1) {
            perror("Send failed");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        recv_len = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (recv_len <= 0) {
            perror("Recv failed");
            break;
        }
        if (strcmp(buffer, "CLOSE") == 0) {
            printf("Client requested to close the connection\n");
            break;
        }
    }
    close(client_sock);
    close(s);
    return 0;
}
