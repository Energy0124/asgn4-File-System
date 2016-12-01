CC=gcc
CFLAGS=-I.

vfs : asg4-vfs.c asg4-func.c
	$(CC) -o $@ $^ -O3 -lm

clean :
	rm vfs
