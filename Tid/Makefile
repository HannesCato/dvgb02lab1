CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SERVER = timeserver
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): timeserver.c
	$(CC) $(CFLAGS) -o $(SERVER) timeserver.c

$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $(CLIENT) client.c

clean:
	rm -f $(SERVER) $(CLIENT)
