CC	:= gcc
LD	:= gcc

CFLAGS	:= -g -Wall
CFLAGS	+= -I.
LDFLAGS	:= libinterrupt.a
HEADERS := server.h


OBJS	:= doubleLinkedList.o server.o main.o

LIBS	:= -lm -lrt

SRCS	:= $(patsubst %.o,%.c,$(OBJS))

PRGS	:= main

all: libinterrupt.a $(PRGS)

libinterrupt.a:
	ar -rv libinterrupt.a

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c $(INCLUDE) -o $@ $(LIBS)

$(PRGS): $(OBJS)
$(PRGS): $(LIBS)
$(PRGS): % : %.o
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	-rm -f *.o *.a *~ $(PRGS)


#gcc -Wall main.c server.c doubleLinkedList.c -o main


# CC=gcc
# FLAGS=-g -Wall -Werror
# OBJ= main server doubleLinkedList
# LIBS=
#
# all:  $(OBJ)
# 	@echo "***************************"
# 	@echo "Compilation successfully!"
# 	@echo ""
#
# doubleLinkedList:	doubleLinkedList.c
# 	$(CC) $(CFLAGS)  $(LIBS)  -o doubleLinkedList  doubleLinkedList.c
#
# server:	server.c
# 	$(CC) $(CFLAGS) $(LIBS) -o server  server.c
#
# main: main.c
# 	$(CC) $(CFLAGS)  $(LIBS) -c main.c
#
# #load:
# #	ld -o doubleLinkedList queue.o
#
# clean:
# 	rm -f server doubleLinkedList *.o
# 	@echo "***************************"
# 	@echo "Deleted files!"
# 	@echo  ""

#  HEADERS = server.h
#
#  PRGS = main
#
#  default: main
#
#  main.o: main.c $(HEADERS)
# 		gcc -c main.c -o main.o
#
# main: main.o
# 		gcc main.o -o main
#
# clean:
# 		-rm -f *.o *.a *~ $(PRGS)
