// Calculator UDP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Define server port and buffer size
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// Function to handle client requests
void handle_client(int sockfd, struct sockaddr_in client_address, socklen_t client_address_length) {
    char buffer[BUFFER_SIZE];
    int num1, num2, result;
    char operator;

    // Receive client request from socket and parse the operands and operator
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &client_address_length);
    sscanf(buffer, "%d %c %d", &num1, &operator, &num2);

    printf("Received request from client: %d %c %d\n", num1, operator, num2);

    // Perform the requested operation based on the operator
    switch (operator) {
        case '+':
            result = num1 + num2;
            printf("Performing addition: %d + %d = %d\n", num1, num2, result);
            break;
        case '-':
            result = num1 - num2;
            printf("Performing subtraction: %d - %d = %d\n", num1, num2, result);
            break;
        case '*':
            result = num1 * num2;
            printf("Performing multiplication: %d * %d = %d\n", num1, num2, result);
            break;
        case '/':
            // Check for division by zero and send error message to client
            if (num2 == 0) {
                strcpy(buffer, "Error: Division by zero");
                sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_address_length);
                return;
            }
            result = num1 / num2;
            printf("Performing division: %d / %d = %d\n", num1, num2, result);
            break;
        default:
            // Send error message for invalid operator to client
            strcpy(buffer, "Error: Invalid operator");
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_address_length);
            return;
    }

    // Convert the result to string and send it back to the client
    sprintf(buffer, "%d", result);
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_address_length);
    printf("Sent result to client: %d\n", result);
}

// Main function to set up the server and listen for client requests
int main() {
    int sockfd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Create a new UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_address, 0, sizeof(server_address));

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    // Bind socket to the server address
    bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    printf("Server started and listening for connections...\n");

    // Handle client requests
    while (1) {
        // Concurrent processing does not apply to UDP since it is connectionless.
        // Each client request is handled sequentially in the handle_client() function.
        handle_client(sockfd, client_address, client_address_length);
    }

    // Close the server socket
    close(sockfd);
    return 0;
}
