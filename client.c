#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8000
#define BUFFER_SIZE 100

int main() {
    int sd, n;
    struct sockaddr_in server_addr = {0};
    char buffer[BUFFER_SIZE];
    char filename[256];
    FILE *fp = NULL;
    int first_packet = 1;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // IMPORTANT: server IP
    inet_aton("10.0.2.3", &server_addr.sin_addr);

    // Connect to server
    connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Get filename from user
    printf("Enter filename: ");
    scanf("%s", filename);

    // Send filename
    write(sd, filename, strlen(filename));

    // Receive data
    while ((n = read(sd, buffer, BUFFER_SIZE)) > 0) {

        if (first_packet) {
            first_packet = 0;

            if (buffer[0] == 'E') {
                buffer[n] = '\0';
                printf("Error: %s\n", buffer + 1);
                close(sd);
                return 0;
            } else {
                fp = fopen("received_file.txt", "wb");
            }
        }

        if (buffer[0] == 'D' && fp != NULL) {
            fwrite(buffer + 1, 1, n - 1, fp);
        }
    }

    if (fp != NULL) {
        fclose(fp);
        printf("File downloaded successfully.\n");
    }

    close(sd);
    return 0;
}
