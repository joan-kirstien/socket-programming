// CLIENT code
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Create a socket for the client
    SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set up the server address to connect to
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(DEFAULT_PORT));
    inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));

    // Connect to the server
    if (connect(ClientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Unable to connect to server: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Receive categories from the server
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        recvbuf[iResult] = '\0'; // Null-terminate the received data
        printf("Received categories: %s\n", recvbuf);

        // Choose a category and send it to the server
        const char* selectedCategory = "tao"; // Change this as needed
        send(ClientSocket, selectedCategory, strlen(selectedCategory), 0);

        // Receive the count of words from the server
        int wordCount;
        iResult = recv(ClientSocket, (char*)&wordCount, sizeof(int), 0);
        if (iResult != sizeof(int)) {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        // Receive and print words from the server
        for (int i = 0; i < wordCount; ++i) {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult <= 0) {
                printf("recv failed with error: %d\n", WSAGetLastError());
                break;
            }
            recvbuf[iResult] = '\0'; // Null-terminate the received data
            printf("Received word %d: %s\n", i + 1, recvbuf);

        // Game logic
        int correctGuessCount = 0;
        int incorrectGuessCount = 0;

        // Loop to receive and process guesses
        for (int i = 0; i < wordCount; ++i) {
            // Receive the current word from the server
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult <= 0) {
                printf("recv failed with error: %d\n", WSAGetLastError());
                break;
            }
            recvbuf[iResult] = '\0'; // Null-terminate the received data
            printf("Current word: %s\n", recvbuf);

            // Prompt the user for a guess
            printf("Enter your guess (Oo/Hindi/Pwede): ");
            char userGuess[DEFAULT_BUFLEN];
            scanf("%s", userGuess);

            // Send the guess to the server
            send(ClientSocket, userGuess, strlen(userGuess), 0);

            // Receive and process the server's response
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult <= 0) {
                printf("recv failed with error: %d\n", WSAGetLastError());
                break;
            }
            recvbuf[iResult] = '\0'; // Null-terminate the received data
            printf("Server response: %s\n", recvbuf);

            // Update counts based on the server's response
            if (strcmp(recvbuf, "Oo") == 0) {
                printf("Correct guess!\n");
                correctGuessCount++;
            } else if (strcmp(recvbuf, "Hindi") == 0) {
                printf("Incorrect guess!\n");
                incorrectGuessCount++;
            } else {
                printf("Maybe/possibly...\n");
            }
        }

        // Print final results
        printf("Game Over\n");
        printf("Correct guesses: %d\n", correctGuessCount);
        printf("Incorrect guesses: %d\n", incorrectGuessCount);

        // Close the socket and clean up
        closesocket(ClientSocket);
        
    } else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
    }

     else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
         WSACleanup();
    }

   

    return 0;
}
