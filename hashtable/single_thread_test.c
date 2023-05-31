#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <time.h>
#include <sys/time.h>
#include "hashtable.h"

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

void insert_keys(hashtable *hash, char **keys, int num_keys)
{
    for (int i=0; i<num_keys; ++i)
    {
        hashtable_insert(hash, keys[i], i);
    }
}

int search_keys(hashtable *hash, char **keys, int num_keys)
{
    int num_missing = 0;
    for (int i=0; i<num_keys; ++i)
    {
        if (hashtable_search(hash, keys[i]) == NULL)
        {
            ++num_missing;
        }
    }
    return num_missing;
}

char *random_key(int len)
{
    char *key = (char *)malloc((len+1)*sizeof(char));
    if (key == NULL)
    {
        perror("malloc");
        exit(1);
    }
    for (int i=0; i<len; ++i)
    {
        key[i] = (random() % 26) + 'a';
    }
    key[len] = '\0';
    return key;
}

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("usage: %s\n", basename(argv[0]));
        exit(1);
    }

    srandom(time(NULL));

    int num_keys = 100000;
    int key_len = 4;

    char **keys;
    keys = (char **)malloc(num_keys*sizeof(char *));
    if (keys == NULL)
    {
        perror("malloc");
        exit(1);
    }

    for (int i=0; i<num_keys; ++i)
    {
        keys[i] = random_key(key_len);;
    }

    hashtable *hash = make_hashtable(64);

    uint64_t start = get_time_usec();
    insert_keys(hash, keys, num_keys);
    uint64_t stop = get_time_usec();
    uint64_t total = stop-start;
    fprintf(stderr, "insert time=%.6lfs\n", total/1000000.0);

    start = get_time_usec();
    int num_missing_keys = search_keys(hash, keys, num_keys);
    stop = get_time_usec();
    total = stop-start;
    fprintf(stderr, "Missing keys: %d\n", num_missing_keys);
    fprintf(stderr, "search time=%.6lfs\n", total/1000000.0);

    for (int i=0; i<num_keys; ++i)
    {
        free(keys[i]);
    }
    free(keys);

    destroy_hashtable(hash);

    return 0;
}
