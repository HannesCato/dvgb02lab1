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

    // 4. Binda socketen till port 37
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    return sock_fd;
}

int main() {
    // Skapa och binda socketen
    int sock_fd = create_socket_udp();
    printf("Socket skapades och är bunden till port %d.\n", TIME_PORT);

    // Stäng socketen (kommer att ersättas senare när vi har en loop)
    close(sock_fd);
    return 0;
}


/* void wait_req(){

}

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