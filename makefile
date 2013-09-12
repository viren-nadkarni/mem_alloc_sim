all: main.o lib.o
	gcc main.o lib.o -lncurses -o mem_alloc_sim

main.o: main.c lib.h
	gcc -I . -c main.c lib.h

lib.o: lib.c
	gcc -I . -c lib.c
	
clean:
	rm -r *.o

