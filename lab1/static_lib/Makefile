all : hello

hello: libhello.a main.o
	gcc -o main main.o -L. -lhello

libhello.a: hello.o
	ar -rc libhello.a hello.o

hello.o : hello.c
	gcc -c hello.c -o hello.o

main.o: main.c
	gcc -c -o main.o main.c

clean:
	rm -f *.a *.o main
