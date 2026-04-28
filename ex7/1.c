#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int a;
    int b;
    char op;
    int id;
} job_t;
static job_t *queue = NULL;
static int q_size = 0, q_head = 0, q_tail = 0;
sem_t q_full, q_empty, write_sem;
pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
void enqueue_job(job_t j) {
    sem_wait(&q_empty); 
    pthread_mutex_lock(&q_mutex);
    queue[q_tail] = j;
    q_tail = (q_tail + 1) % q_size;   
    pthread_mutex_unlock(&q_mutex);
    sem_post(&q_full);
}
job_t dequeue_job() {
    sem_wait(&q_full);
    pthread_mutex_lock(&q_mutex);
    job_t j = queue[q_head];
    q_head = (q_head + 1) % q_size;
    pthread_mutex_unlock(&q_mutex);
    sem_post(&q_empty);
    return j;
}
void* producer(void* arg) {
    int n = *(int*)arg;
    for(int i = 0; i < n; i++) {
        job_t j;
        j.id = i + 1;
        j.a = rand() % 101;
        j.b = rand() % 101;       
        int op = rand() % 4;
        if(op == 0) j.op = '+';
        else if(op == 1) j.op = '-';
        else if(op == 2) j.op = '*';
        else {
            j.op = '/';
            if(j.b == 0) j.b = 1;
        }
        sem_wait(&write_sem);
        FILE *f = fopen("output.txt", "a");
        if(f) {
            fprintf(f, " Job %d: %d %c %d\n", j.id, j.a, j.op, j.b);
            fclose(f);
        }
        sem_post(&write_sem);
        enqueue_job(j);
    }
    return NULL;
}
void* consumer(void* arg) {
    int n = *(int*)arg;
    for(int i = 0; i < n; i++) {
        job_t j = dequeue_job();       
        long res = 0;       
        if(j.op == '+') res = (long)j.a + j.b;
        else if(j.op == '-') res = (long)j.a - j.b;
        else if(j.op == '*') res = (long)j.a * j.b;
        else if(j.op == '/') res = (long)j.a / j.b;
        sem_wait(&write_sem);
        FILE *f = fopen("output.txt", "a");
        if(f) {
            fprintf(f, " Result %d: %ld\n", j.id, res);
            fprintf(f, " Job %d: %d %c %d = %ld\n", j.id, j.a, j.op, j.b, res);
            fprintf(f, "---------------------------\n");
            fclose(f);
        }
        sem_post(&write_sem);
    }
    return NULL;
}
int main() {
    int n;
    printf("Enter number of operations: ");
    if(scanf("%d", &n) != 1 || n <= 0) return 1;
    srand((unsigned)time(NULL));
    q_size = (n < 8) ? n : 8;
    queue = (job_t*)calloc(q_size, sizeof(job_t));
    sem_init(&q_full, 0, 0);          
    sem_init(&q_empty, 0, q_size);    
    sem_init(&write_sem, 0, 1);       
    remove("output.txt");
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, &n);
    pthread_create(&cons, NULL, consumer, &n);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    printf("Done. Check output.txt\n");
    sem_destroy(&q_full);
    sem_destroy(&q_empty);
    sem_destroy(&write_sem);
    pthread_mutex_destroy(&q_mutex);
    free(queue);
    
    return 0;
}
