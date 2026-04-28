#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define WORD_LEN 100
#define WORKER_COUNT 3

char **word_buffer;
char **result_buffer;
char **dictionary;

int buffer_size;
int dict_size;
int total_words;

int in = 0, out = 0;
int count = 0;
int result_count = 0;
int done = 0;

pthread_mutex_t mutex;
pthread_cond_t not_empty;
pthread_cond_t not_full;

/* Spell check function */
int check_spelling(char *word) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(word, dictionary[i]) == 0)
            return 1;
    }
    return 0;
}

/* Worker thread */
void *spell_checker(void *arg) {

    while (1) {

        pthread_mutex_lock(&mutex);

        while (count == 0 && !done)
            pthread_cond_wait(&not_empty, &mutex);

        if (count == 0 && done) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        char word[WORD_LEN];
        strcpy(word, word_buffer[out]);

        out = (out + 1) % buffer_size;
        count--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        int correct = check_spelling(word);

        pthread_mutex_lock(&mutex);

        if (correct)
            sprintf(result_buffer[result_count++], "%s - Correct", word);
        else
            sprintf(result_buffer[result_count++], "%s - Incorrect", word);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {

    pthread_t workers[WORKER_COUNT];

    printf("Enter dictionary size: ");
    scanf("%d", &dict_size);

    printf("Enter number of words to check: ");
    scanf("%d", &total_words);

    printf("Enter buffer size: ");
    scanf("%d", &buffer_size);

    /* Allocate dictionary dynamically */
    dictionary = malloc(dict_size * sizeof(char *));
    for (int i = 0; i < dict_size; i++) {
        dictionary[i] = malloc(WORD_LEN);
        printf("Enter dictionary word %d: ", i + 1);
        scanf("%s", dictionary[i]);
    }

    /* Allocate buffers dynamically */
    word_buffer = malloc(buffer_size * sizeof(char *));
    result_buffer = malloc(total_words * sizeof(char *));

    for (int i = 0; i < buffer_size; i++)
        word_buffer[i] = malloc(WORD_LEN);

    for (int i = 0; i < total_words; i++)
        result_buffer[i] = malloc(WORD_LEN);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    /* Create worker threads */
    for (int i = 0; i < WORKER_COUNT; i++)
        pthread_create(&workers[i], NULL, spell_checker, NULL);

    /* Producer input */
    for (int i = 0; i < total_words; i++) {

        char input[WORD_LEN];

        printf("Enter word to check %d: ", i + 1);
        scanf("%s", input);

        pthread_mutex_lock(&mutex);

        while (count == buffer_size)
            pthread_cond_wait(&not_full, &mutex);

        strcpy(word_buffer[in], input);

        in = (in + 1) % buffer_size;
        count++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }

    /* Signal completion */
    pthread_mutex_lock(&mutex);
    done = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&mutex);

    /* Wait for threads */
    for (int i = 0; i < WORKER_COUNT; i++)
        pthread_join(workers[i], NULL);

    /* Print results */
    printf("\nSpell Check Results:\n");
    for (int i = 0; i < result_count; i++)
        printf("%s\n", result_buffer[i]);

    /* Free memory */
    for (int i = 0; i < dict_size; i++)
        free(dictionary[i]);
    free(dictionary);

    for (int i = 0; i < buffer_size; i++)
        free(word_buffer[i]);
    free(word_buffer);

    for (int i = 0; i < total_words; i++)
        free(result_buffer[i]);
    free(result_buffer);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return 0;
}
