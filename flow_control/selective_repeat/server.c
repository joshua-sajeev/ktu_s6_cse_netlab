#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
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
    int serversocket;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t len;
    struct frame f1;
    int windowsize, totalpackets, totalframes, i = 0, j = 0, framessend = 0, k, m, n = 0;
    int repacket[40];
    struct ack acknowledgement;
    char req[50];

    serversocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serversocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    memset(&clientaddr, 0, sizeof(clientaddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serversocket, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Bind failed");
        close(serversocket);
        exit(EXIT_FAILURE);
    }

    len = sizeof(clientaddr);

    printf("Waiting for client connection...\n");
    recvfrom(serversocket, req, sizeof(req), 0, (struct sockaddr *)&clientaddr, &len);
    printf("Client connection obtained: %s\n", req);

    printf("Sending request for window size\n");
    sendto(serversocket, "REQUEST FOR WINDOW SIZE", strlen("REQUEST FOR WINDOW SIZE"), 0, (struct sockaddr *)&clientaddr, len);
    recvfrom(serversocket, (char *)&windowsize, sizeof(windowsize), 0, (struct sockaddr *)&clientaddr, &len);

    printf("The window size obtained is: %d\n", windowsize);
    totalpackets = windowsize * 2;
    totalframes = (totalpackets / windowsize) + (totalpackets % windowsize != 0);

    printf("Total packets to be sent: %d\n", totalpackets);
    printf("Total frames to be transmitted: %d\n", totalframes);

    sendto(serversocket, (char *)&totalpackets, sizeof(totalpackets), 0, (struct sockaddr *)&clientaddr, len);
    recvfrom(serversocket, req, sizeof(req), 0, (struct sockaddr *)&clientaddr, &len);
    sendto(serversocket, (char *)&totalframes, sizeof(totalframes), 0, (struct sockaddr *)&clientaddr, len);
    recvfrom(serversocket, req, sizeof(req), 0, (struct sockaddr *)&clientaddr, &len);

    printf("Press enter to start the process\n");
    fgets(req, 2, stdin);

    j = 0;

    while (n < totalpackets) {
        memset(&f1, 0, sizeof(f1));
        printf("\nInitializing the transmit buffer\n");
        printf("The frame to be sent is %d with packets: ", framessend);

        for (m = 0; m < j; m++) {
            printf("%d ", repacket[m]);
            f1.packet[m] = repacket[m];
        }

        while (j < windowsize && i < totalpackets) {
            printf("%d ", i);
            f1.packet[j] = i;
            i++;
            j++;
        }

        printf("\nSending frame %d\n", framessend);
        sendto(serversocket, (char *)&f1, sizeof(f1), 0, (struct sockaddr *)&clientaddr, len);

        printf("\nWaiting for the acknowledgement\n");
        recvfrom(serversocket, (char *)&acknowledgement, sizeof(acknowledgement), 0, (struct sockaddr *)&clientaddr, &len);

        j = 0;
        k = 0;
        for (m = 0; m < windowsize && n < totalpackets; m++) {
            if (acknowledgement.acknowledge[m] == -1) {
                printf("Negative acknowledgement received for packet: %d\n", f1.packet[m]);
                repacket[j] = f1.packet[m];
                j++;
            } else {
                n++;
            }
        }

        if (j == 0) {
            printf("\nPositive ack received for all packets, waiting for next frame %d\n", framessend);
            framessend++;
            printf("Press enter to proceed\n");
            fgets(req, 2, stdin);
        }
    }

    printf("\nAll frames sent successfully.\nClosing connection with the client.\n");
    close(serversocket);
    return 0;
}
