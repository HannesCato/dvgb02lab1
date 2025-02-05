#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netinet\in.h>
#include <sys/socket.h> 
#include <netdb.h>

#define PORT 8080
#define BACKLOG 5

 int create_server_socket()
{
 int server_fd;

struct sockaddr_in server_addr;


 server_fd = socket(AF_INET, SOCK_STREAM, 0);
 if(server_fd == -1){
      perror("Failed to create socket");
      exit(EXIT_FAILURE);
 }


 //adress structure
 server_addr.sin_family = AF_INET; //IPv4
 server_addr.sin_addr.s_addr = INADDR_ANY; //
 server_addr.sin_port = htons(8080); //converter

 //bind socket
 if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == - 1)
 {
    perror("Binding failed");
    close(server_fd);
    exit(EXIT_FAILURE);
 }
    printf("Socket binding success %d\n", PORT);

    return server_fd;

}



int main(){
    int server_fd = create_server_socket();


    close(server_fd);
    return 0;
}