#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int n;
    scanf("%d",&n);
    if(n<1 || n>1000) return 1;

    int **A = malloc(n*sizeof(int*));
    int **B = malloc(n*sizeof(int*));
    int **C = malloc(n*sizeof(int*));
    int **P = malloc(n*sizeof(int*));

    for(int i=0;i<n;i++){
        A[i]=malloc(n*sizeof(int));
        B[i]=malloc(n*sizeof(int));
        C[i]=malloc(n*sizeof(int));
        P[i]=malloc(n*sizeof(int));
    }

    double parallel_time;

    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) scanf("%d",&A[i][j]);
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) scanf("%d",&B[i][j]);
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) scanf("%d",&P[i][j]);
    scanf("%lf",&parallel_time);

    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++){
            C[i][j]=0;
            for(int k=0;k<n;k++) C[i][j]+=A[i][k]*B[k][j];
        }
    clock_gettime(CLOCK_MONOTONIC,&end);
    double serial_time = (end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1e9;

    // OUTPUT (organized)
   /* printf("\nSerial Result Matrix:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++)
            printf("%6d",C[i][j]);  // 6-space aligned
        printf("\n");
    }*/

   /* printf("\nParallel Result Matrix:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++)
            printf("%6d",P[i][j]);
        printf("\n");
    }*/

    printf("\nSerial Time   : %f sec",serial_time);
    printf("\nParallel Time : %f sec",parallel_time);

    if(serial_time>parallel_time) printf("\nMax Time Taken: SERIAL\n");
    else printf("\nMax Time Taken: PARALLEL\n");

    // Free memory
    for(int i=0;i<n;i++){ free(A[i]); free(B[i]); free(C[i]); free(P[i]); }
    free(A); free(B); free(C); free(P);

    return 0;
}
