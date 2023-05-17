CC = gcc
CFLAGS = -Wall -ansi -pedantic
LD = gcc
LDFLAGS = 
OBJFILES = hencode.o hdecode.o tree.o htable.o

all: htable.o hencode.o hdecode.o tree.o
	$(LD) $(LDFLAGS) -o hencode tree.o hencode.o
	$(LD) $(LDFLAGS) -o hdecode tree.o hdecode.o
	$(LD) $(LDFLAGS) -o htable tree.o htable.o
htable: htable.c
	$(CC) $(CFLAGS) -c -o htable.o htable.c
hencode: hencode.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c
hdecode: hdecode.c
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c
tree: tree.c
	$(CC) $(CFLAGS) -c -o tree.o tree.c

clean :
	rm -f $(OBJFILES) $(MAIN) *~