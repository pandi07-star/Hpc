#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int SIZE;
    printf("Enter the array size: ");
    scanf("%d", &SIZE);

    int *A = (int*)malloc(SIZE * sizeof(int));
    int *B = (int*)malloc(SIZE * sizeof(int));
    int *C = (int*)malloc(SIZE * sizeof(int));
    
    srand(time(NULL));

    int i;
    for (i = 0; i < SIZE; i++) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    #pragma omp parallel for
    for (i = 0; i < SIZE; i++) {
        C[i] = A[i] + B[i];
    }

    #pragma omp parallel for ordered
    for (i = 0; i < SIZE; i++) {
        #pragma omp ordered
        printf("Index %d: %d + %d = %d\n", i, A[i], B[i], C[i]);
    }

    free(A);
    free(B);
    free(C);

    return 0;
}
