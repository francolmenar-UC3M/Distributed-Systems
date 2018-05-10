CC = gcc

CCFLAGS = -g -Wall -Werror -pedantic
LDLIBS = -pthread

SOURCES = server.c ../../read_line.c
BIN_FILES = server

all: $(BIN_FILES)
.PHONY: all

server: server.o read_line.o
	$(CC) $(CCFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $^ $(LDLIBS) -c $<

clean:
	rm -f $(BIN_FILES) *.o
