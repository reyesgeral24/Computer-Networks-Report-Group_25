#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
        int serv_sd, clnt_sd;
        struct sockaddr_in addr = {0};
        char msg[] = "Hello World!\n";

        serv_sd = socket (AF_INET, SOCK_STREAM, 0);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8000);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(serv_sd, (struct sockaddr*)&addr, sizeof(addr));
        listen(serv_sd, 5);

        printf("Server waiting\n");

        clnt_sd = accept(serv_sd, NULL, NULL);
        write(clnt_sd, msg, sizeof(msg));

        close(clnt_sd);
        close(serv_sd);
        return 0;
}
