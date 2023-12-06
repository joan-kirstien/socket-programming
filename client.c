#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 19019
#define MAX_MSG_SIZE 256

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_MSG_SIZE];
    int clientGuess;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error("WSAStartup failed");
    }

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.6"); // Change this to the server's IP address
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        error("Connection failed");
    }

    printf("Connected to server\n");

    while (1) {
        // Receive the hidden hand from the server
        recv(clientSocket, buffer, sizeof(buffer), 0);
        int hiddenHand = atoi(buffer);

        // Get user input for the guess
        printf("\nEnter your guess (0 or 1, x to exit):");
        scanf("%s", buffer);

        // Send the guess to the server
        send(clientSocket, buffer, strlen(buffer), 0);

        if (buffer[0] == 'x') {
            // Exit the game if 'x' is entered
            break;
        }

        // Receive and print the result
        recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);
    }

    // Clean up Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
