#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#define TIME_PORT 37 // RFC 868
#define SERVER_IP "127.0.0.1"
#define TIME_DIFF 2208988800U  // Sekunder mellan 1900 och 1970

int create_client_socket(struct sockaddr_in *server_addr) {
    int sock_fd;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(TIME_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr->sin_addr) <= 0) {
        perror("Adressfel");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    return sock_fd;
}

void send_request(int sock_fd, struct sockaddr_in *server_addr) {
    if (sendto(sock_fd, NULL, 0, 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Failed to send request");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Request sent to server.\n");
}

void receive_time(int sock_fd) {
    uint32_t server_time;
    socklen_t server_len = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr;

    if (recvfrom(sock_fd, &server_time, sizeof(server_time), 0, (struct sockaddr*)&server_addr, &server_len) < 0) {
        perror("Failed to receive time");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    server_time = ntohl(server_time);

    time_t unix_time = (time_t)(server_time - TIME_DIFF);

    printf("Time from server: %s", ctime(&unix_time));
}

void run_client() {
    struct sockaddr_in server_addr;
    int sock_fd = create_client_socket(&server_addr);

    send_request(sock_fd, &server_addr);
    receive_time(sock_fd);

    close(sock_fd);
}

int main() {
    run_client();
    return 0;
}
