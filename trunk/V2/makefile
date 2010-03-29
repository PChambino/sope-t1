
CC=gcc
CFLAGS=-Wall

PROG=../bin/msh_v2
OBJS=main.o cmd.o

$(PROG): $(OBJS)
	$(CC) $^ -o $@
	
cmd.o: cmd.h
main.o: cmd.h

clean:
	rm $(OBJS) $(PROG)
