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

    int c = connect(s, (struct sockaddr*)&server, sizeof(client));
    if (c == -1) {
        perror("Connection failed\n");
        return 1;
    }
    printf("Connection to server\n");

    char sm[30], cm[30];
    for (;;) {
        printf("Client: ");
        scanf("%s", sm);
        if (send(s, sm, sizeof(sm), 0) == -1) {
            perror("Send failed\n");
            break;
        }

        if (strcmp(sm, "endc") == 0) {
            break;
        }

        memset(cm, 0, sizeof(cm));
        int recv_len = recv(s, cm, sizeof(cm), 0);
        if (recv_len <= 0) {
            perror("Recv failed\n");
            break;
        }
        printf("Server: %s\n", cm);

        if (strcmp(cm, "endc") == 0) {
            break;
        }
    }
    return 0;
}
