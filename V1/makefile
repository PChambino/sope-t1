
CC=gcc
CFLAGS=-Wall

PROG=../bin/msh_v1
OBJS=main.o cmd.o msh.o

$(PROG): $(OBJS)
	$(CC) $^ -o $@
	
msh.o: msh.h cmd.h
cmd.o: cmd.h
main.o: msh.h

clean:
	rm $(OBJS) $(PROG)
