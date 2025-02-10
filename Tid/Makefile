CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = timeserver

all: $(TARGETS)

timeserver: timeserver.c
	$(CC) $(CFLAGS) -o timeserver timeserver.c

clean:
	rm -f $(TARGETS)
