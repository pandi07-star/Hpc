#include <mpi.h>
#include <stdio.h>
#include <stddef.h>
#define MAX 50
struct Student {
    char name[20];
    int roll;
    int total;
    char grade;
};
char findGrade(int marks) {
    if (marks >= 90) return 'A';
    else if (marks >= 75) return 'B';
    else if (marks >= 60) return 'C';
    else if (marks >= 50) return 'D';
    else return 'F';
}
int main() {
    int rank, size, n, chunk;
    struct Student students[MAX];
    MPI_Datatype MPI_STUDENT;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int blocklengths[4];
    MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_CHAR};
    MPI_Aint offsets[4];
    blocklengths[0] = sizeof(students[0].name) / sizeof(char);
    blocklengths[1] = 1; 
    blocklengths[2] = 1;  
    blocklengths[3] = 1;  
    offsets[0] = offsetof(struct Student, name);
    offsets[1] = offsetof(struct Student, roll);
    offsets[2] = offsetof(struct Student, total);
    offsets[3] = offsetof(struct Student, grade);
    MPI_Type_create_struct(4, blocklengths, offsets, types, &MPI_STUDENT);
    MPI_Type_commit(&MPI_STUDENT);
    if (rank == 0) {
        FILE *fp = fopen("students.txt", "r");
        if (fp == NULL) {
            printf("Error: students.txt not found\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fscanf(fp, "%d", &n);

        for (int i = 0; i < n; i++) {
            fscanf(fp, "%s %d %d",
                   students[i].name,
                   &students[i].roll,
                   &students[i].total);
        }
        fclose(fp);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    chunk = n / size;
    if (chunk == 0) {
        if (rank == 0)
            printf("Error: Number of processes > number of students\n");
        MPI_Type_free(&MPI_STUDENT);
        MPI_Finalize();
        return 0;
    }
    struct Student local[chunk];
    MPI_Scatter(students, chunk, MPI_STUDENT,
                local, chunk, MPI_STUDENT,
                0, MPI_COMM_WORLD);
    for (int i = 0; i < chunk; i++) {
        local[i].grade = findGrade(local[i].total);
    }
    MPI_Gather(local, chunk, MPI_STUDENT,
               students, chunk, MPI_STUDENT,
               0, MPI_COMM_WORLD);
    if (rank == 0) {
        FILE *fp = fopen("grades.txt", "w");
        fprintf(fp, "Name Roll Marks Grade\n");
        for (int i = 0; i < n; i++) {
            fprintf(fp, "%s %d %d %c\n",
                    students[i].name,
                    students[i].roll,
                    students[i].total,
                    students[i].grade);
        }
        fclose(fp);
        printf("Output written to grades.txt\n");
    }
    MPI_Type_free(&MPI_STUDENT);
    
    MPI_Finalize();
    return 0;
}
