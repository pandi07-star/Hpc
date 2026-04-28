#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#define SHM_KEY 8001
#define MAX 1000

/*void print_matrix(const char *name, int *M, int n)
{
    printf("\n%s:\n", name);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++)
            printf("%6d", M[i*n+j]);
        printf("\n");
    }
}*/

int main()
{
    int n;
    printf("Enter matrix order N (<=1000): ");
    scanf("%d",&n);

    if(n<1 || n>MAX) return 1;

    int size = n * n * sizeof(int);

    int shmid_n   = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666);
    int shmid_A   = shmget(IPC_PRIVATE, size, IPC_CREAT|0666);
    int shmid_B   = shmget(IPC_PRIVATE, size, IPC_CREAT|0666);
    int shmid_ADD = shmget(IPC_PRIVATE, size, IPC_CREAT|0666);
    int shmid_SUB = shmget(IPC_PRIVATE, size, IPC_CREAT|0666);
    int shmid_MUL = shmget(IPC_PRIVATE, size, IPC_CREAT|0666);
    int shmid_TA  = shmget(IPC_PRIVATE, size, IPC_CREAT|0666);
    int shmid_pt  = shmget(IPC_PRIVATE, sizeof(double), IPC_CREAT|0666);

    if(shmid_n<0 || shmid_A<0 || shmid_B<0 || shmid_ADD<0 ||
       shmid_SUB<0 || shmid_MUL<0 || shmid_TA<0 || shmid_pt<0){
        perror("shmget failed");
        return 1;
    }

    int *N   = shmat(shmid_n,NULL,0);
    int *A   = shmat(shmid_A,NULL,0);
    int *B   = shmat(shmid_B,NULL,0);
    int *ADD = shmat(shmid_ADD,NULL,0);
    int *SUB = shmat(shmid_SUB,NULL,0);
    int *MUL = shmat(shmid_MUL,NULL,0);
    int *TA  = shmat(shmid_TA,NULL,0);
    double *ptime = shmat(shmid_pt,NULL,0);

    if(N==(void*)-1 || A==(void*)-1 || B==(void*)-1 ||
       ADD==(void*)-1 || SUB==(void*)-1 ||
       MUL==(void*)-1 || TA==(void*)-1 || ptime==(void*)-1){
        perror("shmat failed");
        return 1;
    }

    *N = n;
    srand(time(NULL));

    for(int i=0;i<n*n;i++){
        A[i] = rand()%10;
        B[i] = rand()%10;
    }

    struct timespec s,e;
    clock_gettime(CLOCK_MONOTONIC,&s);

    if(fork()==0){
        for(int i=0;i<n*n;i++) ADD[i]=A[i]+B[i];
        exit(0);
    }
    if(fork()==0){
        for(int i=0;i<n*n;i++) SUB[i]=A[i]-B[i];
        exit(0);
    }
    if(fork()==0){
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++){
                MUL[i*n+j]=0;
                for(int k=0;k<n;k++)
                    MUL[i*n+j]+=A[i*n+k]*B[k*n+j];
            }
        exit(0);
    }
    if(fork()==0){
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                TA[j*n+i]=A[i*n+j];
        exit(0);
    }

    for(int i=0;i<4;i++) wait(NULL);

    clock_gettime(CLOCK_MONOTONIC,&e);
    *ptime = (e.tv_sec-s.tv_sec)+(e.tv_nsec-s.tv_nsec)/1e9;

  /*  print_matrix("Matrix A",A,n);
    print_matrix("Matrix B",B,n);
    print_matrix("Addition (Parallel)",ADD,n);
    print_matrix("Subtraction (Parallel)",SUB,n);
    print_matrix("Multiplication (Parallel)",MUL,n);
    print_matrix("Transpose of A (Parallel)",TA,n);*/

    printf("\nParallel Execution Time: %f sec\n",*ptime);

    return 0;
}
