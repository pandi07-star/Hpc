#include <stdio.h>
#include <omp.h>

int main() {
    int n, target, sum = 0; 

    printf("Enter array size: ");
    if (scanf("%d", &n) != 1) return 1;

    int array[n];
    printf("Enter %d array elements: \n", n);
    for(int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

    printf("Enter target value: ");
    scanf("%d", &target);

    #pragma omp parallel for reduction(+:sum)
    for(int i = 0; i < n; i++) {
        if(array[i] < target) {
            sum += 1;
        }
    }

    printf("\nTotal number of values lesser than target [%d]: %d\n", target, sum);
    return 0;
}
