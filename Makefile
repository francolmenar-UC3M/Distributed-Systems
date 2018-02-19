# CC	= gcc
# LD	= gcc
#
# CFLAGS	= -g -Wall
# CFLAGS	+= -I.
# LDFLAGS	= libinterrupt.a
# HEADERS = server.h
#
#
# OBJS	= doubleLinkedList.o server.o
#
# LIBS	= -lm -lrt
#
# SRCS	= $(patsubst %.o,%.c,$(OBJS))
#
# PRGS	= main
#
# all: libinterrupt.a $(PRGS)
#
# libinterrupt.a:
# 	ar -rv libinterrupt.a
#
# %.o: %.c $(HEADERS)
# 	$(CC) $(CFLAGS) -c $*.c $(INCLUDE) -o $@ $(LIBS)
#
# $(PRGS): $(OBJS)
# $(PRGS): $(LIBS)
# $(PRGS): % : %.o
# 	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS) $(LIBS)
#
# clean:
# 	-rm -f *.o *.a *~ $(PRGS)

 HEADERS = server.h

 PRGS = main

 default: main

 main.o: main.c $(HEADERS)
		gcc -c main.c -o main.o

main: main.o
		gcc main.o -o main

clean:
		-rm -f *.o *.a *~ $(PRGS)
