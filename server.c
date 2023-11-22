
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define MAX_CLIENTS 5

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSockets[MAX_CLIENTS];
    struct sockaddr_in serverAddr, clientAddr;
    int clientCount = 0;
    char buffer[256];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error("WSAStartup failed");
    }

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        error("Socket binding failed");
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        error("Listen failed");
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept incoming connections
        int addrLen = sizeof(clientAddr);
        clientSockets[clientCount] = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSockets[clientCount] == INVALID_SOCKET) {
            error("Accept failed");
        }

        printf("Client %d connected\n", clientCount + 1);

        // Generate a random number (0 or 1) to represent the hidden hand
        int hiddenHand = rand() % 2;

        // Send the hidden hand to the client
        sprintf(buffer, "%d", hiddenHand);
        send(clientSockets[clientCount], buffer, strlen(buffer), 0);

        // Receive the client's guess
        recv(clientSockets[clientCount], buffer, sizeof(buffer), 0);
        int clientGuess = atoi(buffer);

        // Check if the guess is correct
        if (clientGuess == hiddenHand) {
            send(clientSockets[clientCount], "Correct! You guessed the right hand.", sizeof(buffer), 0);
        } else {
            send(clientSockets[clientCount], "Incorrect! Try again.", sizeof(buffer), 0);
        }

        // Close the client socket when done
        closesocket(clientSockets[clientCount]);

        clientCount++;

        if (clientCount >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. No longer accepting connections.\n");
            break;
        }
    }

    // Clean up Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
