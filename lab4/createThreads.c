#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define THREAD_NO 10

void *mythread(void *arg) {
    int *id = (int *)arg;  // cast argument back to int pointer
    printf("my id is %d\n", *id);
    free(id);  // free allocated memory
}

int main() {
    pthread_t p[THREAD_NO];
    int i = 0;
    for (i = 0; i < THREAD_NO; i++) {
        int *id = malloc(sizeof(int));  // allocate memory for thread ID
        *id = i;                        // store unique ID
        pthread_create(&p[i], NULL, mythread, id);
    }

    for (i = 0; i < THREAD_NO; i++) {
        pthread_join(p[i], NULL);
    }
    return 0;
}
