#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* PDU structure as defined in assignment */
struct pdu {
    char type;
    char data[100];
};

int main(int argc, char *argv[])
{
    struct sockaddr_in sin, fsin;
    struct pdu spdu, rpdu;
    int s, alen, n;
    FILE *fp;
    struct stat file_stat;
    long file_size, bytes_sent;
    int port = 3000;

    switch(argc){
        case 1:
            break;
        case 2:
            port = atoi(argv[1]);
            break;
        default:
            fprintf(stderr, "Usage: %s [port]\n", argv[0]);
            exit(1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    /* Create UDP socket */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0){
        fprintf(stderr, "can't create socket\n");
        exit(1);
    }

    /* Bind socket */
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
        fprintf(stderr, "can't bind to %d port\n", port);
        exit(1);
    }

    listen(s, 5);
    printf("UDP File Server started on port %d\n", port);

    /* Main server loop */
    while(1){
        alen = sizeof(fsin);

        printf("Waiting for client request...\n");

        /* Wait for FILENAME PDU from client */
        n = recvfrom(s, &rpdu, sizeof(rpdu), 0,
                    (struct sockaddr *)&fsin, &alen);

        if(n < 0){
            fprintf(stderr, "recvfrom error\n");
            continue;
        }

        /* Check its a FILENAME PDU type C */
        if(rpdu.type != 'C'){
            fprintf(stderr, "Expected FILENAME PDU\n");
            continue;
        }

        printf("Client requested file: %s\n", rpdu.data);

        /* Try to open requested file */
        fp = fopen(rpdu.data, "rb");

        if(fp == NULL){
            /* Send ERROR PDU */
            spdu.type = 'E';
            sprintf(spdu.data, "Error: File not found");
            sendto(s, &spdu, strlen(spdu.data) + 2, 0,
                  (struct sockaddr *)&fsin, sizeof(fsin));
            printf("File not found - sent ERROR PDU\n");
            continue;
        }

        /* Get file size using stat */
        stat(rpdu.data, &file_stat);
        file_size = file_stat.st_size;
        bytes_sent = 0;

        printf("File size: %ld bytes\n", file_size);

        /* Send file in 100 byte chunks */
        while(1){
            /* Read 100 bytes from file */
            n = fread(spdu.data, 1, 100, fp);

            if(n <= 0) break;

            bytes_sent += n;

            /* Last chunk = FINAL PDU */
            /* Otherwise = DATA PDU   */
            if(bytes_sent >= file_size){
                spdu.type = 'F';
                printf("Sending FINAL PDU (%d bytes)\n", n);
            } else {
                spdu.type = 'D';
                printf("Sending DATA PDU (%d bytes)\n", n);
            }

            /* Send PDU to client */
            sendto(s, &spdu, n + 1, 0,
                  (struct sockaddr *)&fsin, sizeof(fsin));
        }

        fclose(fp);
        printf("Transfer complete: %ld bytes sent\n",
               bytes_sent);
    }

    close(s);
    return 0;
}
