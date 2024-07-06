#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    struct sockaddr_in server, client;
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("Socket creation failed\n");
        return 1;
    }
    printf("Socket creation successful\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(2007);
    server.sin_family = AF_INET;

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Binding failed\n");
        close(s);
        return 1;
    }
    printf("Binding successful\n");

    if (listen(s, 1) == -1) {
        perror("Listening failed\n");
        close(s);
        return 1;
    }
    printf("Listening...\n");

    socklen_t len = sizeof(client);
    int sock = accept(s, (struct sockaddr*)&client, &len);
    if (sock == -1) {
        perror("Accept failed\n");
        close(s);
        return 1;
    }
    printf("Connection successful\n");

    char buffer[30];

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        int recv_len = recv(sock, buffer, sizeof(buffer), 0);
        if (recv_len <= 0) {
            perror("Recv failed\n");
            break;
        }
        if (strcmp(buffer, "endc") == 0) {
            break;
        }

        printf("Client: %s\n", buffer);

        printf("Server: ");
        scanf("%s", buffer);

        if (send(sock, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed\n");
            break;
        }
    }
    printf("Got all frames\n");

    close(sock);
    close(s);
    return 0;
}
