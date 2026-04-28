#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int size;
    printf("Enter the array size: ");
    scanf("%d", &size);

    int *A = (int*)malloc(size * sizeof(int));  
    int i, constant;
    
    printf("Enter the value to add: ");
    scanf("%d", &constant);  

    #pragma omp parallel for
    for(i = 0; i < size; i++) {
        A[i] = rand() % 100;
    }
    
    #pragma omp parallel for ordered
    for(i = 0; i < size; i++) {
        #pragma omp ordered
        printf("Before adding: A[%d] = %d\n", i, A[i]);
    }

    #pragma omp parallel for
    for(i = 0; i < size; i++) {
        A[i] = A[i] + constant;
    }
    
    #pragma omp parallel for ordered
    for(i = 0; i < size; i++) {
        #pragma omp ordered
        printf("After adding: A[%d] = %d\n", i, A[i]);
    }
    
    free(A);
    
    return 0;
}
