# Kompilator
CC = gcc
# Kompileringsflaggor
CFLAGS = -Wall -Wextra -std=c11
# Utdatafil (binärfil)
TARGET = server
# Källfiler
SRCS = server.c
# Objektfiler (för framtida expansion)
OBJS = $(SRCS:.c=.o)


all: $(TARGET)

# Kompilera servern
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Kompilera enskilda .o-filer
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Starta servern
run: $(TARGET)
	./$(TARGET)

# Rensa gamla filer
clean:
	rm -f $(TARGET) $(OBJS)

# Rensa och bygg om
rebuild: clean all
