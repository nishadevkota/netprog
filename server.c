// winsock_server.c
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    // Setup server info
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }

    // Listen
    listen(server_socket, 1);
    printf("Server is waiting for client on port %d...\n", PORT);

    // Accept client
    client_socket = accept(server_socket, (struct sockaddr *)&client, &client_len);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed\n");
        return 1;
    }

    // Random number generation
    srand((unsigned int)time(NULL));
    int secret = rand() % 100 + 1;
    printf("Secret number is %d\n", secret);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        int guess = atoi(buffer);
        printf("Client guessed: %d\n", guess);

        const char *response;
        if (guess < secret)
            response = "Higher\n";
        else if (guess > secret)
            response = "Lower\n";
        else {
            response = "Correct\n";
            send(client_socket, response, strlen(response), 0);
            break;
        }

        send(client_socket, response, strlen(response), 0);
    }

    printf("Client guessed correctly. Game over.\n");
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
