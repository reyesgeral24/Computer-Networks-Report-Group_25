#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8000
#define DATA_SIZE 99   // 1 byte for type + 99 bytes data
#define BUFFER_SIZE 100

int main() {
    int server_sd, client_sd, n;
    struct sockaddr_in server_addr = {0};
    char filename[256];
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create TCP socket
    server_sd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    bind(server_sd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Listen for connections
    listen(server_sd, 5);

    printf("Server waiting on port %d...\n", PORT);

    // Accept client
    client_sd = accept(server_sd, NULL, NULL);

    // Receive filename
    n = read(client_sd, filename, sizeof(filename) - 1);
    filename[n] = '\0';

    printf("Requested file: %s\n", filename);

    // Try opening file
    fp = fopen(filename, "rb");

    if (fp == NULL) {
        // Send error message
        buffer[0] = 'E';  // Error flag
        strcpy(buffer + 1, "File not found");
        write(client_sd, buffer, strlen(buffer + 1) + 1);
    } else {
        // Send file in chunks
        while ((n = fread(buffer + 1, 1, DATA_SIZE, fp)) > 0) {
            buffer[0] = 'D';  // Data flag
            write(client_sd, buffer, n + 1);
        }
        fclose(fp);
    }

    // Close connection
    close(client_sd);
    close(server_sd);

    return 0;
}
