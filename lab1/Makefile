all: lab1
lab1: helloworld.o
	gcc -o lab1 helloworld.o
helloworld.o: helloworld.c
	gcc -c helloworld.c
clean:
	rm -f lab1 helloworld.o
