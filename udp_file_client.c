#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* PDU structure must match server */
struct pdu {
    char type;
    char data[100];
};

int main(int argc, char **argv)
{
    char *host = "localhost";
    int port = 3000;
    struct hostent *phe;
    struct sockaddr_in sin;
    struct pdu spdu, rpdu;
    int s, n;
    char choice;
    char local_filename[150];
    FILE *fp;

    switch(argc){
        case 2:
            host = argv[1];
            break;
        case 3:
            host = argv[1];
            port = atoi(argv[2]);
            break;
        default:
            fprintf(stderr,
                "Usage: %s host [port]\n", argv[0]);
            exit(1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    /* Map host name to IP address */
    if( phe = gethostbyname(host) ){
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    }
    else if((sin.sin_addr.s_addr =
             inet_addr(host)) == INADDR_NONE){
        fprintf(stderr, "Can't get host entry\n");
        exit(1);
    }

    /* Create UDP socket */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0){
        fprintf(stderr, "Can't create socket\n");
        exit(1);
    }

    /* Connect socket to server */
    if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
        fprintf(stderr, "Can't connect\n");
        exit(1);
    }

    /* Main loop for multiple downloads */
    while(1){
        printf("\n=== UDP File Download Client ===\n");
        printf("D - Download a file\n");
        printf("Q - Quit\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);

        /* Quit */
        if(choice == 'Q' || choice == 'q'){
            printf("Goodbye!\n");
            break;
        }

        /* Download */
        if(choice == 'D' || choice == 'd'){

            /* Prepare and send FILENAME PDU */
            spdu.type = 'C';
            printf("Enter filename to download: ");
            scanf("%s", spdu.data);

            /* Send filename to server */
            write(s, &spdu, strlen(spdu.data) + 2);

            /* Set local filename */
            sprintf(local_filename,
                    "downloaded_%s", spdu.data);

            /* Read first PDU from server */
            n = read(s, &rpdu, sizeof(rpdu));
            if(n < 0){
                fprintf(stderr, "read error\n");
                continue;
            }

            /* Check if error PDU */
            if(rpdu.type == 'E'){
                printf("Server Error: %s\n", rpdu.data);
                continue;
            }

            /* Open local file to save data */
            fp = fopen(local_filename, "wb");
            if(fp == NULL){
                fprintf(stderr, "Can't open local file\n");
                continue;
            }

            printf("Downloading...\n");

            /* Write first chunk to file */
            fwrite(rpdu.data, 1, n - 1, fp);
            printf("Received PDU '%c': %d bytes\n",
                   rpdu.type, n-1);

            /* Keep receiving until FINAL PDU */
            while(rpdu.type != 'F'){
                n = read(s, &rpdu, sizeof(rpdu));
                if(n < 0){
                    fprintf(stderr, "read error\n");
                    break;
                }
                fwrite(rpdu.data, 1, n - 1, fp);
                printf("Received PDU '%c': %d bytes\n",
                       rpdu.type, n-1);
            }

            fclose(fp);
            printf("Done! Saved as: %s\n",
                   local_filename);

        } else {
            printf("Invalid choice. Enter D or Q\n");
        }
    }

    close(s);
    return 0;
}
