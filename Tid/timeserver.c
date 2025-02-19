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

/*create and set up a UDP socket for a server. This function 
returns an interger which is the socket file descriptor sock_fd.
AF_INET use IPv4, SOCK_DGRAM use UDP, 0 - let the system choose
the appropriate protocol.
If socket creation fails (sock_fd < 0) error occurs */
int create_socket_udp() {
    int sock_fd;
    struct sockaddr_in server_addr;
//sockaddr_in is structure used to store address information for IPv4
//server_addr is a variable of type sockaddr_in
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
/* memset is used to clear the server_addr structure
by setting all it bytes to 0.
*/
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET; //specifies use of IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //allows socket to listen on all available networks
    server_addr.sin_port = htons(TIME_PORT); 
    //assigns a port number to the socket
    //htons convert the port number from host byte order to network byte order
    //TIME_PORT predefined port number 37
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    //bind function connects sock_fd with the server_addr
    //if bind fails, print error message
    //close(sock_fd) is called to release socket before exiting
    
    return sock_fd;
    //returns socket file descriptor
}


/*  sockfd is the socket file descriptor
    client_addr is a pointer to a sockaddr_in structure where
    clients address will be stored.
    client_len is a pointer to the size of client_addr, needed for recvfrom*/
void wait_for_request(int sockfd, struct sockaddr_in *client_addr, socklen_t *client_len) {
    char buffer[1];  // Tom buffer (RFC 868 kräver ingen data i förfrågan)

    printf("Waiting for request...\n");

// recvfrom() is used  to recieve UDP packets
    ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)client_addr, client_len);
    
// if bytes_received >= 0, client has sent a request
//inet_ntoa converts clients IP address
//ntohs converts clients port from network byte order to host byte order
    if (bytes_received >= 0) {
        printf("Received request from client %s:%d\n",
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
    } else {
        perror("Receive failed");
    }
}
//generates a 32-bit timestamp that follows RFC 868
uint32_t get_time() {
    time_t current_time = time(NULL);
    //retreives the current time as the number of seconds since Jan 1, 1970
    uint32_t rfc868_time = (uint32_t)(current_time + TIME_DIFF);
    return htonl(rfc868_time); // Konvertera till nätverksbyteordning (32-bit)
}
// retrives time from get_time(), sends it via sendto()
void send_time(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len) {
    uint32_t server_time = get_time();

// sends the 4 byte RFC 868 time value to the client
    if (sendto(sockfd, &server_time, sizeof(server_time), 0, (struct sockaddr *)client_addr, client_len) < 0) {
        perror("Send failed");
    } else {
        printf("Sent time to client %s:%d -> %u (RFC 868 format)\n",
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), ntohl(server_time));
               //converts
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
