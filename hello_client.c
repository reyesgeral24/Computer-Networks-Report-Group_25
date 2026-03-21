#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        int sd;
        struct sockaddr_in server = {0};
        char buf[256];

        sd = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_family = AF_INET;
        server.sin_port = htons(8000);
        inet_aton("192.168.2.51", &server.sin_addr);

        connect(sd, (struct sockaddr*)&server, sizeof(server));

        int n = read(sd, buf, sizeof(buf) - 1);
        if (n > 0) {
                buf[n] = '\0';
                printf("Received: %s\n", buf);
        }
        else if (n == 0) {
                printf("Connection terminated\n");
        }

        close(sd);
        return 0;
}
