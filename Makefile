CC	= gcc
LD	= gcc

CFLAGS	= -g -Wall
CFLAGS	+= -I.
LDFLAGS	=
HEADERS =


OBJS	= list.o

LIBS	=

SRCS	= $(patsubst %.o,%.c,$(OBJS))

PRGS	= doubleLinkedList

all:  $(PRGS)

libinterrupt.a:

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c $(INCLUDE) -o $@ $(LIBS)

$(PRGS): $(OBJS)
$(PRGS): $(LIBS)
$(PRGS): % : %.o
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	-rm -f *.o *.a *~ $(PRGS)
