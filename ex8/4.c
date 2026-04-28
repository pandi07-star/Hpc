#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int size, window;
    printf("Enter the size and window:");
    scanf("%d %d", &size, &window);

    int *A = (int*)malloc(size * sizeof(int));
    int *sumArray = (int*)malloc((size - window + 1) * sizeof(int));

    #pragma omp parallel for
    for(int i = 0; i < size; i++)
        A[i] = rand() % 100;

    for(int i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");

    #pragma omp parallel for
    for(int i = 0; i <= size - window; i++) {
        int sum = 0;
        for(int j = 0; j < window; j++)
            sum += A[i + j];
        sumArray[i] = sum;
    }

    for(int i = 0; i <= size - window; i++)
        printf("%d ", sumArray[i]);
    printf("\n");

    free(A);
    free(sumArray);
    return 0;
}
