#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFSIZE 64
#define MSG "Any Message \n"

int
main(int argc, char **argv)
{
    char *host = "localhost";
    int port = 3000;
    char now[100];
    struct hostent *phe;
    struct sockaddr_in sin;
    int s, n, type;

    switch (argc) {
        case 1:
            break;
        case 2:
            host = argv[1];
        case 3:
            host = argv[1];
            port = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "usage: UDPtime [host [port]]\n");
            exit(1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if ( phe = gethostbyname(host) ){
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    }
    else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
        fprintf(stderr, "Can't get host entry \n");

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
        fprintf(stderr, "Can't create socket \n");

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        fprintf(stderr, "Can't connect to %s %s \n", host, "Time");

    (void) write(s, MSG, strlen(MSG));

    n = read(s, (char *)&now, sizeof(now));
    if (n < 0)
        fprintf(stderr, "Read failed\n");

    write(1, now, n);
    exit(0);
}
