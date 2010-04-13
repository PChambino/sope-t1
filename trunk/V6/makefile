
CC=gcc
CFLAGS=-Wall

PROG=../bin/msh_v6
OBJS=main.o cmd.o msh.o exec.o

$(PROG): $(OBJS)
	$(CC) $^ -o $@
	
exec.o: exec.h cmd.h
msh.o: msh.h cmd.h exec.h
cmd.o: cmd.h msh.h
main.o: msh.h

clean:
	rm $(OBJS) $(PROG)
