#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define PORT 19019
#define MAX_CLIENTS 5

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void handleClient(void *clientSocket);

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSockets[MAX_CLIENTS];
    struct sockaddr_in serverAddr, clientAddr;
    int clientCount = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error("WSAStartup failed");
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        error("Socket binding failed");
    }

    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        error("Listen failed");
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        int addrLen = sizeof(clientAddr);
        clientSockets[clientCount] = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSockets[clientCount] == INVALID_SOCKET) {
            error("Accept failed");
        }

        printf("Client %d connected\n", clientCount + 1);

        _beginthread(handleClient, 0, (void *)&clientSockets[clientCount]);

        clientCount++;

        if (clientCount >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. No longer accepting connections.\n");
            break;
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void handleClient(void *clientSocket) {
    SOCKET socket = *((SOCKET *)clientSocket);
    char buffer[256];

    // Generate a random number (0 or 1) to represent the hidden hand
    int hiddenHand = rand() % 2;

    // Send the hidden hand to the client
    sprintf(buffer, "%d", hiddenHand);
    send(socket, buffer, strlen(buffer), 0);

    // Receive the client's guess
    recv(socket, buffer, sizeof(buffer), 0);
    int clientGuess = atoi(buffer);

    // Check if the guess is correct
    if (clientGuess == hiddenHand) {
        send(socket, "Correct! You guessed the right hand.", sizeof(buffer), 0);
    } else {
        send(socket, "Incorrect!You picked the wrong hand", sizeof(buffer), 0);
    }

    // Close the client socket when done
    closesocket(socket);
}
