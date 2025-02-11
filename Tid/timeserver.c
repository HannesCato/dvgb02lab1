#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#define TIME_PORT 37 // RFC 868
#define TIME_DIFF 2208988800U

int create_socket_udp() {
    int sock_fd;
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

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

    ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)client_addr, client_len);
    
    if (bytes_received >= 0) {
        printf("Received request from client %s:%d\n",
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
    } else {
        perror("Receive failed");
    }
}

uint32_t get_time() {
    time_t current_time = time(NULL);
    uint32_t rfc868_time = (uint32_t)(current_time + TIME_DIFF);
    return htonl(rfc868_time); // Konvertera till nätverksbyteordning (32-bit)
}

void send_time(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len) {
    uint32_t server_time = get_time();

    if (sendto(sockfd, &server_time, sizeof(server_time), 0, (struct sockaddr *)client_addr, client_len) < 0) {
        perror("Send failed");
    } else {
        printf("Sent time to client %s:%d -> %u (RFC 868 format)\n",
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(server_time));
    }
}

int main() {
    int sockfd = create_socket_udp();  // Skapa och binda socket
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("Socket created and bound to port %d.\n", TIME_PORT);

    while (1) {
        wait_for_request(sockfd, &client_addr, &client_len);

        send_time(sockfd, &client_addr, client_len);
    }

    close(sockfd);
    return 0;
}
