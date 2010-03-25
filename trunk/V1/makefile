
CC=gcc
CFLAGS=-Wall -c

PROG=../bin/msh_v1
OBJS=main.o cmd.o

$(PROG): $(OBJS)
	$(CC) $^ -o $@
	
%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

main.o: main.c cmd.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJS) $(PROG)
	