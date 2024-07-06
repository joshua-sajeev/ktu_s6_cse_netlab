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
    struct sockaddr_in server;
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

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Connection failed");
        close(s);
        return 1;
    }
    printf("Connected to server\n");

    printf("Enter filename to download: ");
    scanf("%s", buffer);

    if (send(s, buffer, strlen(buffer), 0) == -1) {
        perror("Send failed");
        close(s);
        return 1;
    }

    while (1) {
        int n = recv(s, buffer, BUFFER_SIZE, 0);
        if (n <= 0) {
            perror("Recv failed");
            break;
        }
        if (strncmp(buffer, "EOF", 3) == 0) {
            printf("File transfer complete\n");
            break;
        }
        if (strncmp(buffer, "ERROR:", 6) == 0) {
            printf("%s\n", buffer);
            break;
        }
        fwrite(buffer, 1, n, stdout);
    }

    // Send a close message to the server
    if (send(s, "CLOSE", 5, 0) == -1) {
        perror("Send failed");
    }

    close(s);
    return 0;
}
