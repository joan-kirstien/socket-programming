
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
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

    // Receive the hidden hand from the server
    recv(clientSocket, buffer, sizeof(buffer), 0);
    int hiddenHand = atoi(buffer);

    // Get user input for the guess
    printf("Enter your guess (0 or 1): ");
    scanf("%d", &clientGuess);

    // Send the guess to the server
    sprintf(buffer, "%d", clientGuess);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive and print the result
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    // Clean up Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
