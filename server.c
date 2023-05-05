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
void handle_client(int sockfd) {
    char buffer[BUFFER_SIZE];
    int num1, num2, result;
    char operator;

    // Read client request from socket and parse the operands and operator
    read(sockfd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%d %c %d", &num1, &operator, &num2);

    // Perform the requested operation based on the operator
    switch (operator) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            // Check for division by zero and send error message to client
            if (num2 == 0) {
                strcpy(buffer, "Error: Division by zero");
                write(sockfd, buffer, strlen(buffer));
                return;
            }
            result = num1 / num2;
            break;
        default:
            // Send error message for invalid operator to client
            strcpy(buffer, "Error: Invalid operator");
            write(sockfd, buffer, strlen(buffer));
            return;
    }

    // Convert the result to string and send it back to the client
    sprintf(buffer, "%d", result);
    write(sockfd, buffer, strlen(buffer));
}

// Main function to set up the server and listen for client connections
int main() {
    int sockfd, newsockfd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Create a new TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_address, 0, sizeof(server_address));

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    // Bind socket to the server address
    bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));

    // Listen for incoming connections
    listen(sockfd, 5);

    // Accept client connections and handle their requests
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_length);
        handle_client(newsockfd);
        close(newsockfd);
    }

    // Close the server socket
    close(sockfd);
    return 0;
}
