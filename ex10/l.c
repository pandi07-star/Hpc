#include <string.h>
#include <omp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_LINE 1024
#define QUEUE_SIZE 100

typedef struct {
    char *buffer[QUEUE_SIZE];
    int front, rear, count;
    omp_lock_t lock;
} queue_t;

queue_t queue;
omp_lock_t print_lock;

int producers_done = 0;
int total_producers;

// Initialize queue
void init_queue(queue_t *q) {
    q->front = q->rear = q->count = 0;
    omp_init_lock(&q->lock);
}

// Enqueue
int enqueue(queue_t *q, char *line) {
    int success = 0;

    omp_set_lock(&q->lock);
    if (q->count < QUEUE_SIZE) {
        q->buffer[q->rear] = line;
        q->rear = (q->rear + 1) % QUEUE_SIZE;
        q->count++;
        success = 1;
    }
    omp_unset_lock(&q->lock);

    return success;
}

// Dequeue
char* dequeue(queue_t *q) {
    char *line = NULL;

    omp_set_lock(&q->lock);
    if (q->count > 0) {
        line = q->buffer[q->front];
        q->front = (q->front + 1) % QUEUE_SIZE;
        q->count--;
    }
    omp_unset_lock(&q->lock);

    return line;
}

// Producer
void producer(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;

    char buffer[MAX_LINE];

    while (fgets(buffer, MAX_LINE, fp)) {
        char *line = strdup(buffer);

        while (!enqueue(&queue, line)) {
            usleep(1000);
        }
    }

    fclose(fp);

    #pragma omp atomic
    producers_done++;

    omp_set_lock(&print_lock);
    printf("Producer thread %d finished\n", omp_get_thread_num());
    omp_unset_lock(&print_lock);
}

// Consumer
void consumer() {
    int tid = omp_get_thread_num();

    while (1) {
        char *line = dequeue(&queue);

        if (line) {
            omp_set_lock(&print_lock);

            printf("\n[Thread %d processing line]\n", tid);

            char *token = strtok(line, " \t\n");
            while (token != NULL) {
                printf("Thread %d: %s\n", tid, token);
                token = strtok(NULL, " \t\n");
            }

            omp_unset_lock(&print_lock);

            free(line);
        } 
        else {
            if (producers_done == total_producers) {
                omp_set_lock(&print_lock);
                printf("Thread %d exiting (no more work)\n", tid);
                omp_unset_lock(&print_lock);
                break;
            }

            usleep(1000);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s file1 file2 ...\n", argv[0]);
        return 1;
    }

    total_producers = argc - 1;

    init_queue(&queue);
    omp_init_lock(&print_lock);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        if (tid < total_producers) {
            producer(argv[tid + 1]);
        } else {
            consumer();
        }
    }

    omp_destroy_lock(&queue.lock);
    omp_destroy_lock(&print_lock);

    printf("\nAll work done. Program exiting successfully.\n");

    return 0;
}
