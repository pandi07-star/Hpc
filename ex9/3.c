#include <stdio.h>
#include <omp.h>


int main() {
    int n, max_val;

    printf("Enter array size: ");
    scanf("%d", &n);

    int array[n];
    printf("Enter %d array elements: \n", n);
    for(int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

   
    max_val = -1; 

    #pragma omp parallel for reduction(max : max_val)
    for(int i = 0; i < n; i++) {
        if(array[i] > max_val) {
            max_val = array[i];
        }
    }

    printf("Max value from given array: %d\n", max_val);
    return 0;
}
