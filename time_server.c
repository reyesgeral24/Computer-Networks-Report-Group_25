#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>

int
main(int argc, char *argv[])
{
    struct sockaddr_in fsin;
    char buf[100];
    char *pts;
    int sock;
    time_t now;
    int alen;
    struct sockaddr_in sin;
    int s, type;
    int port=3000;

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

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
        fprintf(stderr, "can't creat socket\n");

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        fprintf(stderr, "can't bind to %d port\n",port);

    listen(s, 5);
    alen = sizeof(fsin);

    while (1) {
        if (recvfrom(s, buf, sizeof(buf), 0,
            (struct sockaddr *)&fsin, &alen) < 0)
            fprintf(stderr, "recvfrom error\n");
        (void) time(&now);
        pts = ctime(&now);
        (void) sendto(s, pts, strlen(pts), 0,
            (struct sockaddr *)&fsin, sizeof(fsin));
    }
}
