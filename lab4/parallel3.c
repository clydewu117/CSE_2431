#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define ARRAY_SIZE 1000000
#define THREAD_NO 10

int sum = 0;

int main(){
    int num[THREAD_NO][ARRAY_SIZE/THREAD_NO];

    srand(100);
    //initialize arrays
    for(int i=0; i< THREAD_NO; i++){
	for(int j=0; j< ARRAY_SIZE/THREAD_NO; j++){
            num[i][j] = rand() % 100;
	}
    }

    printf("sum = %d\n", sum);
    return 0;
}
