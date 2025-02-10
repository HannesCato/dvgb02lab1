#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TIME_PORT 37 //RFC 868
#define SERVER_IP "127.0.0.1"

int create_client_socket() {
    int sock_fd;
    struct sockaddr_in server_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TIME_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Adressfel");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect-fel");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    return sock_fd;
}

void receive_time(int sock_fd){
    char buffer[100];
    recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[sizeof(buffer) - 1] = '\0';
    printf("Tid från servern: %s\n", buffer);
}

void run_client(){
    int sock_fd = create_client_socket();
    receive_time(sock_fd);
    close(sock_fd);
}

int main() {
    run_client();
    return 0;
}