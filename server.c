// SERVER code
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

// List of words for each category
const char* tao[] = {"Bruce Lee", "Eyes", "Jose Rizal"};
const char* hayop[] = {"Dog", "Lion", "Zebra"};
const char* bagay[] = {"Pencil", "Pillow", "TV"};

void handleClient(void* clientSocket) {
    SOCKET ClientSocket = (SOCKET)clientSocket;

    // Send categories to the client
    const char* categoriesMessage = "Categories: tao, hayop, bagay";
    send(ClientSocket, categoriesMessage, strlen(categoriesMessage), 0);

    // Receive selected category from the client
    char category[DEFAULT_BUFLEN];
    int iResult = recv(ClientSocket, category, DEFAULT_BUFLEN, 0);
    if (iResult > 0) {
        category[iResult] = '\0'; // Null-terminate the received data
        printf("Category selected: %s\n", category);

        // Choose words based on the selected category
        const char** words;
        int wordCount;

        if (strcmp(category, "tao") == 0) {
            words = tao;
            wordCount = sizeof(tao) / sizeof(tao[0]);
        } else if (strcmp(category, "hayop") == 0) {
            words = hayop;
            wordCount = sizeof(hayop) / sizeof(hayop[0]);
        } else if (strcmp(category, "bagay") == 0) {
            words = bagay;
            wordCount = sizeof(bagay) / sizeof(bagay[0]);
        } else {
            // Invalid category
            const char* errorMessage = "Invalid category";
            send(ClientSocket, errorMessage, strlen(errorMessage), 0);
            closesocket(ClientSocket);
            return;
        }

        // Start the Pinoy Henyo game logic (replace this part with your actual game logic)
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;

        int i = 0;
        do {
            // Send the current word to the client
            send(ClientSocket, words[i], strlen(words[i]), 0);

            // Receive the client's guess
            recv(ClientSocket, recvbuf, recvbuflen, 0);

            // Compare the guess with the correct word
            if (strcmp(recvbuf, words[i]) == 0) {
                const char* correctMessage = "Oo";  // Correct guess
                send(ClientSocket, correctMessage, strlen(correctMessage), 0);
            } else {
                const char* incorrectMessage = "Hindi";  // Incorrect guess
                send(ClientSocket, incorrectMessage, strlen(incorrectMessage), 0);
            }

            i++;
        } while (i < wordCount);

        // Game over, close the connection
        const char* gameOverMessage = "Game Over";
        send(ClientSocket, gameOverMessage, strlen(gameOverMessage), 0);
    } else {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }

    // Close the client socket
    closesocket(ClientSocket);
}


void startServer(SOCKET ListenSocket) {
    int iResult;

    // Main server loop
    while (1) {
        printf("Waiting for a client to connect...\n");

        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        printf("Client connected. Creating a new thread to handle the client...\n");

        // Create a new thread to handle the client
        _beginthread(handleClient, 0, (void*)ClientSocket);
    }

    // Cleanup code if needed (it will not reach this point in the current loop)
    closesocket(ListenSocket);
    WSACleanup();
}

// Main function...
int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Create a socket for the server to listen on
    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set up the server address to bind and listen on
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(DEFAULT_PORT));

    // Bind the socket
    if (bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Listen on the socket
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Call the function to handle server logic
    startServer(ListenSocket);

    return 0;
}
