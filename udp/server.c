#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int main()
{
    struct sockaddr_in server, client;

    int s = socket(AF_INET, SOCK_DGRAM, 0);
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

    socklen_t len = sizeof(client);

    char sm[30], cm[30];
    for (;;) {
        memset(sm, 0, sizeof(sm));
        int recv_len = recvfrom(s, sm, sizeof(sm), 0, (struct sockaddr*)&client, &len);
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
        if (sendto(s, cm, sizeof(cm), 0, (struct sockaddr*)&client, len) == -1) {
            perror("Send failed\n");
            break;
        }

        if (strcmp(cm, "endc") == 0) {
            break;
        }
    }
    return 0;
}
