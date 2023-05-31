#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "matrix.h"

uint64_t get_time_usec()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
    {
        perror("clock_gettime");
        exit(1);
    }
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

typedef struct
{
    matrix *m1;
    matrix *m2;
    matrix *res;
    int start_row;
    int end_row;
} thread_data_t;

void *multiply_matrix_thread(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;
    matrix *m1 = data->m1;
    matrix *m2 = data->m2;
    matrix *res = data->res;

    for (int r = data->start_row; r < data->end_row; ++r)
    {
        for (int c = 0; c < res->num_cols; ++c)
        {
            res->data[r][c] = 0;
            for (int k = 0; k < m1->num_cols; ++k)
            {
                res->data[r][c] += m1->data[r][k] * m2->data[k][c];
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("usage: %s num_threads matrix1_file matrix2_file\n", basename(argv[0]));
        exit(1);
    }

    int num_threads = atoi(argv[1]);
    matrix *m1 = read_matrix(argv[2]);
    matrix *m2 = read_matrix(argv[3]);

    if (m1->num_cols != m2->num_rows)
    {
        printf("Matrix dimensions don't match!");
        exit(1);
    }

    matrix *res = alloc_matrix(m1->num_rows, m2->num_cols);
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(num_threads * sizeof(thread_data_t));

    uint64_t start = get_time_usec();

    int rows_per_thread = m1->num_rows / num_threads;
    int remaining_rows = m1->num_rows % num_threads;

    for (int i = 0; i < num_threads; ++i)
    {
        int start_row = i * rows_per_thread + (i < remaining_rows ? i : 0);
        int end_row = start_row + rows_per_thread + (i < remaining_rows ? 1 : 0);
        thread_data[i].m1 = m1;
        thread_data[i].m2 = m2;
        thread_data[i].res = res;
        thread_data[i].start_row = start_row;
        thread_data[i].end_row = end_row;

        if (pthread_create(&threads[i], NULL, multiply_matrix_thread, &thread_data[i]) != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; ++i)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join");
            exit(1);
        }
    }

    uint64_t stop = get_time_usec();

    print_matrix(res);
    free_matrix(m1);
    free_matrix(m2);
    free_matrix(res);
    free(threads);
    free(thread_data);

    uint64_t total = stop - start;
    fprintf(stderr, "time=%.6lfs\n", total / 1000000.0);

    return 0;
}

