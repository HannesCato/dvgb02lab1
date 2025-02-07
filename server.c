#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <errno.h>

#define PORT 8080
#define BACKLOG 5

//test

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
 //  //bind socket
 if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == - 1)
 {
    perror("Binding failed");
    close(server_fd);
    exit(EXIT_FAILURE);
 }
    printf("Socket binding success %d\n", PORT);
    return server_fd;
}

void start_listening(int server_fd)
{
if(listen(server_fd, BACKLOG) == - 1)
{
    perror("Listening failed");
    close(server_fd);
    exit(EXIT_FAILURE);
}
printf("Listening to port %d\n", PORT);
}
int accept_connection(int server_fd)
{
struct sockaddr_in client_addr;
socklen_t addr_size = sizeof(client_addr);
int client_socket = accept(server_fd, (struct sockaddr*) &client_addr, &addr_size);
if(client_socket == - 1)
{
    perror("Failed to accept");
    return -1;
}
    
    printf("Connection Accepted");
    return client_socket;
}

void handle_client(int client_socket)
{
    char buffer[1024];

    
    int bytes_recieved = read(client_socket, buffer, sizeof(buffer) - 1);
    buffer[bytes_recieved] = '\0';
    printf("Recieved request: \n%s\n", buffer);

    char filename[256] = "index.html";
    sscan(buffer, "GET /%s HTTP/1.1," filename);

    response(client_socket, filename);
}

void response(int client_socket, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file){
        perror("File not found");
        no_response(client_socket);
        return;
    }

    char buffer[1024];
    char response_header[256];

    snprintf(response_header, sizeof(response_header),
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "\r\n");

    send(client_socket, response_header, strlen(response_header), 0);

    while(fgets(buffer, sizeof(buffer), file ) != NULL) {
        send(client_socket, buffer, strlen(buffer), 0);
    }
    fclose(file);
    close(client_socket);
}

void no_response(int client_socket)
{
    const char *response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body><h1>404 Not Found</h1><p>The requested file was not found.</p></body></html>";

    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}
int main(){
    int server_fd = create_server_socket();
    

    close(server_fd);
    return 0;
}