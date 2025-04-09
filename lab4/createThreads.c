#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define THREAD_NO 10

void *mythread(void *arg) {
    int *id = (int *)arg;
    printf("my id is %d\n", *id);
    free(id);
}

int main() {
    pthread_t p[THREAD_NO];
    int i = 0;
    for (i = 0; i < THREAD_NO; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&p[i], NULL, mythread, id);
    }

    for (i = 0; i < THREAD_NO; i++) {
        pthread_join(p[i], NULL);
    }
    return 0;
}
