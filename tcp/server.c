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
    printf("Socket creation successfull\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(2002);
    server.sin_family = AF_INET;

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Binding failed\n");
        close(s);
        return 1;
    }
    printf("Binding successfull\n");

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
        return 1;
    }
    printf("Connection successfull\n");

    char sm[30], cm[30];
    for (;;) {
        memset(sm, 0, sizeof(sm));
        int recv_len = recv(sock, sm, sizeof(sm), 0);
        if (recv_len <= 0) {
            perror("Recv failed\n");
            break;
        }
        printf("Client: %s\n", sm);

        if (strcmp(sm, "endc") == 0) {
            break;
        }

        printf("Server: ");
        scanf("%s", cm);
        if (send(sock, cm, sizeof(cm), 0) == -1) {
            perror("Send failed\n");
            break;
        }

        if (strcmp(cm, "endc") == 0) {
            break;
        }
    }
    return 0;
}
