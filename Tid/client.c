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

/*create a client socket with the arguments of sockaddr_in - the
address to store the information of IPv4 and a pointer to the 
server address */
int create_client_socket(struct sockaddr_in *server_addr) {
    int sock_fd;
/*create a socket with AF_INET - IPv4 and SOCK_DGRAM - 
UDP protocol, 0 means no flags.
If socket file descriptor returns -1, an error have occured*/
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
/* memset clears the server_addr structure, might contain random data
that can lead to unspected behavior.*/
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET; //set address family to AF_INET / IPv4
    server_addr->sin_port = htons(TIME_PORT); //set the serverport. htons converts the
                                              //port from host byte order to network byte order
    
/*inet_pton converts IP address to binary and stores it in server_addr
if -1, error*/
    if (inet_pton(AF_INET, SERVER_IP, &server_addr->sin_addr) <= 0) {
        perror("Adressfel");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
//else return socket file descriptor
    return sock_fd;
}
/* the client sends a request to the server
sendto sockfd, no data is sent, message length is 0, 0 flags, server addres and the size of server aderess
if -1, error occures*/
void send_request(int sock_fd, struct sockaddr_in *server_addr) {
    if (sendto(sock_fd, NULL, 0, 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Failed to send request");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Request sent to server.\n");
}
/* After server recive request, server sends time that client recives 32 byte timestamp. */
void receive_time(int sock_fd) {
    uint32_t server_time; //stores time
    socklen_t server_len = sizeof(struct sockaddr_in); //stores size of sockaddr_in, needed for recvfrom
    struct sockaddr_in server_addr; //stores the server address information

    if (recvfrom(sock_fd, &server_time, sizeof(server_time), 0, (struct sockaddr*)&server_addr, &server_len) < 0) {
        perror("Failed to receive time");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    server_time = ntohl(server_time); //converts network byte order to host byte order

    time_t unix_time = (time_t)(server_time - TIME_DIFF); //converts from RFC 868 time to UNNIX time

    printf("Time from server: %s", ctime(&unix_time)); //print the time
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
