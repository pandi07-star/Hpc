#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int rows, cols;
    printf("Enter the Rows and Cols:");
    scanf("%d %d", &rows, &cols);

    int **matrix = (int**)malloc(rows * sizeof(int*));
    for(int i = 0; i < rows; i++)
        matrix[i] = (int*)malloc(cols * sizeof(int));

    #pragma omp parallel for
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            matrix[i][j] = rand() % 100;

    #pragma omp parallel for ordered
    for(int i = 0; i < rows; i++) {
        #pragma omp ordered
        {
            for(int j = 0; j < cols; j++)
                printf("%d ", matrix[i][j]);
            printf("\n");
        }
    }

    #pragma omp parallel for
    for(int i = 0; i < rows; i++) {
        int start = 0, end = cols - 1;
        while(start < end) {
            int temp = matrix[i][start];
            matrix[i][start] = matrix[i][end];
            matrix[i][end] = temp;
            start++;
            end--;
        }
    }

    #pragma omp parallel for ordered
    for(int i = 0; i < rows; i++) {
        #pragma omp ordered
        {
            for(int j = 0; j < cols; j++)
                printf("%d ", matrix[i][j]);
            printf("\n");
        }
    }

    for(int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);

    return 0;
}
