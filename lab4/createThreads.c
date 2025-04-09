#include <stdio.h>
#include <pthread.h>
#define THREAD_NO 10

void *mythread(void *arg) {
    int *id = (int *)arg;
    printf("my id is %d\n", *id);
}

int main(){
    pthread_t p[THREAD_NO];
    int i = 0;
    for(i=0; i<THREAD_NO; i++){
        pthread_create(&p[i], NULL, mythread, &i);
    }

    for(i=0; i<THREAD_NO; i++){
	pthread_join(p[i], NULL);
    }
    return 0;
}
