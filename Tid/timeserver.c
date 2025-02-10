#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TIME_PORT 37 // RFC 868

int create_socket_udp() {
    int sock_fd;
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    //reset memory
    memset(&server_addr, 0, sizeof(server_addr));

    //Adress and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  
    server_addr.sin_port = htons(TIME_PORT);  

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    return sock_fd;
}
void wait_for_request(int sockfd, struct sockaddr_in *client_addr, socklen_t *client_len) {
    char buffer[1];  // Tom buffer (RFC 868 kräver ingen data i förfrågan)

    printf("Waiting for request...\n");

    // Mottar en tom förfrågan från klienten
    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)client_addr, client_len) >= 0) {
        printf("Received request from client\n");
    } else {
        perror("Receive failed");
    }
}

int main() {
    int sockfd = create_socket_udp();  // Skapa och binda socket
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("Socket skapades och är bunden till port %d.\n", TIME_PORT);

    // Vänta på en UDP-förfrågan
    wait_for_request(sockfd, &client_addr, &client_len);

    close(sockfd);
    return 0;
}



/**
 */


/*
//wait for UDP request



 void get_time()
{
 //get time in RFC-868 format
}


void send_time()
{
    //send time back to client
}


void run()
{
    //main loop
}
    */