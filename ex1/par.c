#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    int n;
    fprintf(stderr, "Enter matrix size (NxN, max 1000): ");
    scanf("%d", &n);
    if(n<1 || n>1000){ fprintf(stderr,"Invalid size!\n"); return 1; }

    int **A = malloc(n * sizeof(int*));
    int **B = malloc(n * sizeof(int*));
    int **C = malloc(n * sizeof(int*));
    int **pipefd = malloc(n * sizeof(int*));

    for(int i=0;i<n;i++){
        A[i]=malloc(n*sizeof(int));
        B[i]=malloc(n*sizeof(int));
        C[i]=malloc(n*sizeof(int));
        pipefd[i]=malloc(2*sizeof(int));
    }

    srand(time(NULL));

    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            A[i][j]=rand()%10, B[i][j]=rand()%10;

    // Print matrices in organized format (stderr)
   /* fprintf(stderr,"\nMatrix A:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++) fprintf(stderr,"%4d",A[i][j]);
        fprintf(stderr,"\n");
    }*/

    /*fprintf(stderr,"\nMatrix B:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++) fprintf(stderr,"%4d",B[i][j]);
        fprintf(stderr,"\n");
    }*/

    // Create pipes
    for(int i=0;i<n;i++) pipe(pipefd[i]);

    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);

    // Fork children
    for(int i=0;i<n;i++){
        if(fork()==0){
            close(pipefd[i][0]);
            int *row=malloc(n*sizeof(int));
            for(int j=0;j<n;j++){
                row[j]=0;
                for(int k=0;k<n;k++) row[j]+=A[i][k]*B[k][j];
            }
            write(pipefd[i][1], row, n*sizeof(int));
            close(pipefd[i][1]);
            free(row);
            exit(0);
        }
    }

    // Parent collects rows
    for(int i=0;i<n;i++){
        close(pipefd[i][1]);
        read(pipefd[i][0], C[i], n*sizeof(int));
        close(pipefd[i][0]);
        wait(NULL);
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    double parallel_time=(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1e9;

    // Send data to serial
    printf("%d\n",n);
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) printf("%d ",A[i][j]);
    printf("\n");
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) printf("%d ",B[i][j]);
    printf("\n");
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++) printf("%d ",C[i][j]);
    printf("\n");
    printf("%f\n",parallel_time);

    // Free memory
    for(int i=0;i<n;i++){ free(A[i]); free(B[i]); free(C[i]); free(pipefd[i]); }
    free(A); free(B); free(C); free(pipefd);

    return 0;
}
