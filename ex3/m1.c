#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char* argv[]){
        int rank, size;
        MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        printf("Welocome Bro from proc %d of %d proc \n", rank, size);
        MPI_Finalize();
        return 0;
}
