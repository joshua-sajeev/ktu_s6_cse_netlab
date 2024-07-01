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
    server.sin_port = htons(2007);
    server.sin_family = AF_INET;

    int c = connect(s, (struct sockaddr*)&server, sizeof(client));
    if (c == -1) {
        perror("Connection failed\n");
        return 1;
    }
    printf("Connection to server\n");

    int total_frames;
    printf("Enter no of frames to be sent: ");
    scanf("%d", &total_frames);

    char frame[30], ack[30];
    int flag = 1;
    do {

        sprintf(frame, "%d", total_frames);
        do {
            if (send(s, frame, sizeof(frame), 0) == -1) {
                perror("Send failed\n");
                break;
            }

            if (strcmp(frame, "endc") == 0) {
                break;
            }

            memset(ack, 0, sizeof(ack));
            int recv_len = recv(s, ack, sizeof(ack), 0);
            if (recv_len <= 0) {
                perror("Recv failed\n");
                break;
            }
            printf("Acknowledgment: %s\n", ack);

            if (strcmp(ack, "1") == 0) {
                flag = 0;
            }

            if (strcmp(ack, "endc") == 0) {
                break;
            }
        } while (flag);
        flag = 1;
        total_frames--;
    } while (total_frames);
    sprintf(frame, "%s", "endc");
    if (send(s, frame, sizeof(frame), 0) == -1) {
        perror("Send failed\n");
        return 1;
    }
    printf("Transmitted all frames");
    close(s);

    return 0;
}
