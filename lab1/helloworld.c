#include <stdio.h>
#include <stdlib.h>

int* random_10();

int myMinimum(int* arr);

int main() {
    printf("Hello, my name is Baicheng Wu\n");

    int* random_10_nums = random_10();

    int min = myMinimum(random_10_nums);

    printf("Minimum: %d\n", min);

    if (random_10_nums != NULL) {
        free(random_10_nums);
    }
    random_10_nums = NULL;

    return 0;
}

int* random_10() {
    int* arr = calloc(10, sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        exit(0);
    }

    printf("10 random numbers generated: \n");
    for (int i = 0; i < 10; i++) {
        int num = rand() % 100 + 1;
        printf("%d\n", num);
        *(arr + i) = num;
    }

    return arr;
}

int myMinimum(int* arr) {
    int* pt = arr;
    int min = *pt;
    for (int i = 0; i < 10; i++) {
        int cur = *(pt + i);
        if (cur < min) {
            min = cur;
        }
    }

    return min;
}
