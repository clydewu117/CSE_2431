all: createThreads parallel1 parallel2 parallel3

createThreads: createThreads.c
	gcc -o createThreads createThreads.c -pthread

parallel1: parallel1.c
	gcc -o parallel1 parallel1.c -pthread

parallel2: parallel2.c
	gcc -o parallel2 parallel2.c -pthread

parallel3: parallel3.c
	gcc -o parallel3 parallel3.c -pthread

sequential: sequential.c
	gcc -o sequential sequential.c -pthread

clean:
	rm -f createThreads parallel1 parallel2 parallel3 sequential