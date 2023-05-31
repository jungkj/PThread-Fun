#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <time.h>
#include <sys/time.h>
#include "matrix.h"

uint64_t get_time_usec()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
    {
        perror("clock_gettime");
        exit(1);
    }
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s matrix1_file matrix2_file\n", basename(argv[0]));
        exit(1);
    }

    matrix *m1 = read_matrix(argv[1]);
    matrix *m2 = read_matrix(argv[2]);

    uint64_t start = get_time_usec();
    matrix *res = multiply_matrix(m1, m2);
    uint64_t stop = get_time_usec();

    print_matrix(res);
    free_matrix(m1);
    free_matrix(m2);
    free_matrix(res);

    uint64_t total = stop-start;
    fprintf(stderr, "time=%.6lfs\n", total/1000000.0);
    
    return 0;
}
