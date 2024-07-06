#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>

#define PORT 5018
#define BUF_SIZE 1024

struct frame {
    int packet[40];
};

struct ack {
    int acknowledge[40];
};

int main() {
    int clientsocket;
    struct sockaddr_in serveraddr;
    socklen_t len;
    struct hostent *server;
    struct frame f1;
    int windowsize, totalpackets, totalframes, i = 0, j = 0, framesreceived = 0, k, m, n = 0;
    int repacket[40];
    struct ack acknowledgement;
    char req[50];

    clientsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientsocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);

    printf("Sending request to the server\n");
    sendto(clientsocket, "HI I AM CLIENT", strlen("HI I AM CLIENT"), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    len = sizeof(serveraddr);

    printf("Waiting for reply\n");
    recvfrom(clientsocket, req, sizeof(req), 0, (struct sockaddr *)&serveraddr, &len);
    printf("The server has sent: %s\n", req);

    printf("Enter the window size: ");
    scanf("%d", &windowsize);

    printf("Sending window size\n");
    sendto(clientsocket, (char *)&windowsize, sizeof(windowsize), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    printf("Waiting for the server response\n");
    recvfrom(clientsocket, (char *)&totalpackets, sizeof(totalpackets), 0, (struct sockaddr *)&serveraddr, &len);
    printf("The total packets are: %d\n", totalpackets);
    sendto(clientsocket, "RECEIVED", strlen("RECEIVED"), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    recvfrom(clientsocket, (char *)&totalframes, sizeof(totalframes), 0, (struct sockaddr *)&serveraddr, &len);
    printf("The frames of windows are: %d\n", totalframes);
    sendto(clientsocket, "RECEIVED", strlen("RECEIVED"), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    printf("Starting the process of receiving:\n");

    while (n < totalpackets) {
        memset(&f1, 0, sizeof(f1));
        printf("\nInitializing the receiver buffer\n");

        printf("The expected frame is %d with packets: ", framesreceived);

        for (m = 0; m < j; m++) {
            printf("%d ", repacket[m]);
        }

        printf("\nWaiting for the frame\n");
        recvfrom(clientsocket, (char *)&f1, sizeof(f1), 0, (struct sockaddr *)&serveraddr, &len);
        printf("Received frame %d\n", framesreceived);

        k = 0;
        for (m = 0; m < windowsize && k < totalpackets; m++) {
            printf("Packet %d: ", f1.packet[m]);
            scanf("%d", &acknowledgement.acknowledge[m]);
            if (acknowledgement.acknowledge[m] == -1) {
                repacket[j] = f1.packet[m];
                j++;
            } else {
                n++;
            }
            k++;
        }

        framesreceived++;
        sendto(clientsocket, (char *)&acknowledgement, sizeof(acknowledgement), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    }

    printf("\nAll frames received successfully.\nClosing connection with the server.\n");
    close(clientsocket);
    return 0;
}
