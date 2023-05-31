#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#define NUM_BUCKETS 5     // Buckets in hash table
#define NUM_KEYS 100000   // Number of keys inserted per thread
int num_threads = 1;      // Number of threads (configurable): COULD NOT FIGURE OUT HOW TO GET NUM_THREADS FROM ARGS AND USE IT THE WAY I DID HERE AS A GLOBAL VARIABLE)
int keys[NUM_KEYS];
pthread_mutex_t mutexes[NUM_BUCKETS]; //Array of locks

typedef struct _bucket_entry {
    int key;
    int val;
    struct _bucket_entry *next;
} bucket_entry;

bucket_entry *table[NUM_BUCKETS];

void panic(char *msg) {
    printf("%s\n", msg);
    exit(1);
}

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

// Inserts a key-value pair into the table
void insert(int key, int val) {

    int i = key % NUM_BUCKETS;

    pthread_mutex_lock(&mutexes[i]); //acquire lock

    bucket_entry *e = (bucket_entry *) malloc(sizeof(bucket_entry));
    if (!e) panic("No memory to allocate bucket!");
    e->next = table[i];
    e->key = key;
    e->val = val;
    table[i] = e;

    pthread_mutex_unlock(&mutexes[i]); // release lock


}

// Retrieves an entry from the hash table by key
// Returns NULL if the key isn't found in the table
bucket_entry * retrieve(int key) {
    bucket_entry *b;

    for (b = table[key % NUM_BUCKETS]; b != NULL; b = b->next) {
        if (b->key == key) return b;

    }

    return NULL;
}

void * put_phase(void *arg) {
    long tid = (long) arg;
    int key = 0;

    // If there are k threads, thread i inserts
    //      (i, i), (i+k, i), (i+k*2)

    for (key = tid ; key < NUM_KEYS; key += num_threads) {
        insert(keys[key], tid);
    }

    pthread_exit(NULL);
}

void * get_phase(void *arg) {
    long tid = (long) arg;
    int key = 0;
    long lost = 0;

    for (key = tid ; key < NUM_KEYS; key += num_threads) {

          if (retrieve(keys[key]) == NULL) lost++;
    }

    printf("[thread %ld] %ld keys lost!\n", tid, lost);

    pthread_exit((void *)lost);
}

int main(int argc, char **argv) {
    long i;
    int j;
    pthread_t *threads;
    
    srandom(time(NULL));
    //Initialize array of locks
     for (j=0; j< NUM_BUCKETS; j++)
    {
        pthread_mutex_init( &mutexes[j], NULL );
    }

    if (argc != 2) {
        panic("usage: ./parallel_hashtable <num_threads>");
    }
    if ((num_threads = atoi(argv[1])) <= 0) {
        panic("must enter a valid number of threads to run");
    }

    srandom(time(NULL));
    for (i = 0; i < NUM_KEYS; i++)
        keys[i] = random();

    threads = (pthread_t *) malloc(sizeof(pthread_t)*num_threads);
    if (!threads) {
        panic("out of memory allocating thread handles");
    }

    // Insert keys in parallel
    uint64_t start = get_time_usec();
    for (i = 0; i < num_threads; i++) {

        pthread_create(&threads[i], NULL, put_phase, (void *)i);

  }
    // Barrier
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    uint64_t stop = get_time_usec();
    uint64_t total = stop-start;
    fprintf(stderr, "insert time=%.6lfs\n", total/1000000.0);


    // Reset the thread array
    memset(threads, 0, sizeof(pthread_t)*num_threads);

    // Retrieve keys in parallel
    start = get_time_usec();
    for (i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, get_phase, (void *)i);
    }

    // Collect count of lost keys
    long total_lost = 0;
    long *lost_keys = (long *) malloc(sizeof(long) * num_threads);
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], (void **)&lost_keys[i]);
        total_lost += lost_keys[i];
    }
    stop = get_time_usec();
   
    fprintf(stderr, "search time=%.6lfs\n", total/1000000.0);
    
    return 0;
}

