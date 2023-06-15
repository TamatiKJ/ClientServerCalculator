// Client Calculator
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Define server address and port
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// Function to perform the client request
void perform_request(int sockfd, struct sockaddr_in server_address, socklen_t server_address_length) {
    char buffer[BUFFER_SIZE];
    printf("Enter the expression (e.g., 5 + 3): ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Remove newline character from the buffer
    buffer[strcspn(buffer, "\n")] = '\0';

    // Send the request to the server
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_address, server_address_length);

    // Receive and print the response from the server
    memset(buffer, 0, BUFFER_SIZE);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, &server_address_length);
    printf("Result: %s\n", buffer);
}

int main() {
    int sockfd;
    struct sockaddr_in server_address;
    socklen_t server_address_length = sizeof(server_address);

    // Create a socket for both TCP and UDP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(server_address.sin_addr)) <= 0) {
        perror("Invalid server address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server (for TCP)
    if (connect(sockfd, (struct sockaddr *)&server_address, server_address_length) < 0) {
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }

    // Perform the client request
    perform_request(sockfd, server_address, server_address_length);

    // Close the socket
    close(sockfd);
    return 0;
}
