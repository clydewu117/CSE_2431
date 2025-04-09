#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE 1000000
#define THREAD_NO 10

int sum = 0;
pthread_mutex_t lock;

int main() {
    int num[THREAD_NO][ARRAY_SIZE / THREAD_NO];

    srand(100);
    // initialize arrays
    for (int i = 0; i < THREAD_NO; i++) {
        for (int j = 0; j < ARRAY_SIZE / THREAD_NO; j++) {
            num[i][j] = rand() % 100;
        }
    }

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < THREAD_NO; i++) {
        int *local_arr = num[i];
        pthread_create(&tid[i], NULL, runner, (void *)local_arr);
    }

    for (int i = 0; i < THREAD_NO; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("sum = %d\n", sum);
    return 0;
}

void *runner(void *param) {
    int *local_arr = (int *)param;
    for (int i = 0; i < ARRAY_SIZE / THREAD_NO; i++) {
        pthread_mutex_lock(&lock);
        sum += local_arr[i];
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}
