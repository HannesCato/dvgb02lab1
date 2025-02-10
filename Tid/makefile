CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = timeserver timeclient

all: $(TARGETS)

timeserver: timeserver.c
	$(CC) $(CFLAGS) -o timeserver timeserver.c

timeclient: timeclient.c
	$(CC) $(CFLAGS) -o timeclient timeclient.c

clean:
	rm -f $(TARGETS)
