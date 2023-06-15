// Concurrent calculator TCP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

// Define server port and buffer size
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// Function to handle client requests
void* handle_client(void* arg) {
    int sockfd = *(int*)arg;
    char buffer[BUFFER_SIZE];
    int num1, num2, result;
    char operator;

    // Read client request from socket and parse the operands and operator
    read(sockfd, buffer, BUFFER_SIZE);
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
                write(sockfd, buffer, strlen(buffer));
                close(sockfd);
                pthread_exit(NULL);
            }
            result = num1 / num2;
            printf("Performing division: %d / %d = %d\n", num1, num2, result);
            break;
        default:
            // Send error message for invalid operator to client
            strcpy(buffer, "Error: Invalid operator");
            write(sockfd, buffer, strlen(buffer));
            close(sockfd);
            pthread_exit(NULL);
    }

    // Convert the result to string and send it back to the client
    sprintf(buffer, "%d", result);
    write(sockfd, buffer, strlen(buffer));
    printf("Sent result to client: %d\n", result);

    // Close the client socket and exit the thread
    close(sockfd);
    printf("Closed connection with client\n");
    pthread_exit(NULL);
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
    printf("Server started and listening for connections...\n");

    // Accept client connections and handle their requests concurrently
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &client_address_length);

        // Create a new thread to handle the client request
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, &newsockfd);

        // Detach the thread to allow it to clean up its resources automatically
        pthread_detach(thread);
    }

    // Close the server socket
    close(sockfd);
    return 0;
}
