#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 5

void response(int client_socket, const char *filename);
void no_response(int client_socket);

int create_server_socket()
{
    int server_fd;
    struct sockaddr_in server_addr;
    int opt = 1;

    // Skapa socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    // 🔥 Sätt SO_REUSEADDR innan bind() för att undvika "Address already in use"
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Setsockopt error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Konfigurera serverns adress
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket till port
    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
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
    if(listen(server_fd, BACKLOG) == -1)
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

    printf("Waiting for connection...\n"); 
    int client_socket = accept(server_fd, (struct sockaddr*) &client_addr, &addr_size);

    if(client_socket == -1)
    {
        perror("Failed to accept connection");
        return -1;
    }
    
    printf("Connection Accepted! Client socket: %d\n", client_socket); 
    return client_socket;
}

void handle_client(int client_socket)
{
    printf("Client connected, reading request...\n"); 

    char buffer[1024];
    char filename[256] = "index.html";  

    int bytes_received = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_received <= 0) {
        perror("Reading error from client");
        close(client_socket);
        return;
    }
    buffer[bytes_received] = '\0';

    printf("Received request: %s\n", buffer);

    char method[10], path[256], protocol[10];
    sscanf(buffer, "%9s %255s %9s", method, path, protocol);

    if (strcmp(path, "/") == 0) {
        strcpy(filename, "index.html");
    } else {
        snprintf(filename, sizeof(filename), "%s", path + 1);
    }

    printf("Requested file: %s\n", filename);

    response(client_socket, filename);

    printf("Response function completed.\n");
}

void response(int client_socket, const char *filename)
{
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "sample_website/%s", filename); // Sökväg till sample_website/

    printf("Trying to open file: %s\n", filepath);

    printf("Full file path: %s\n", filepath);


    FILE *file = fopen(filepath, "rb");
    if (!file){
        perror("File not found");
        printf("404 for file: %s\n", filepath);
        no_response(client_socket);
        return;
    }

    printf("File opened successfully: %s\n", filepath);

    char buffer[1024];
    char response_header[256];

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    printf("File size: %ld bytes\n", file_size);

    snprintf(response_header, sizeof(response_header),
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: %ld\r\n"
    "Content-Type: text/html\r\n"
    "\r\n", file_size);

    printf("Sending response header...\n");
    if (send(client_socket, response_header, strlen(response_header), 0) == -1) {
        perror("Error sending response header");
        fclose(file);
        close(client_socket);
        return;
    }
    printf("Response header sent!\n");

    printf("Sending file content...\n");
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytes_read, 0) == -1) {
            perror("Error sending file content");
            break;
        }
    }

    printf("File sent successfully!\n");

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

    if (send(client_socket, response, strlen(response), 0) == -1) {
        perror("Error sending 404 response");
    }

    close(client_socket);
}

int main() {
    int server_fd = create_server_socket();
    start_listening(server_fd);

    while (1) {
        int client_socket = accept_connection(server_fd);
        if (client_socket != -1) {
            handle_client(client_socket);
        }
    }

    close(server_fd);
    return 0;
}
