#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "hashtable.h"

// "private" functions

hashitem *make_item(char *key, int value);
void print_item(hashitem *item);
void destroy_item(hashitem *item);



hashbucket *make_bucket();
void add_to_bucket(hashbucket *bucket, hashitem *item);
hashitem *find_in_bucket(hashbucket *bucket, char *key);
void print_bucket(hashbucket *bucket);
void destroy_bucket(hashbucket *bucket);

#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock
pthread_mutex_t hashlock;



int djb2_hash(hashtable *hash, char *key);

hashitem *make_item(char *key, int value)
{
    if (key == NULL)
    {
        printf("make_item: can't have a NULL key!\n");
        exit(1);
    }

    hashitem *item = (hashitem *)malloc(sizeof(hashitem));
    if (item == NULL)
    {
        perror("malloc");
        exit(1);
    }

    item->key = strdup(key);
    item->value = value;
    return item;
}

void print_item(hashitem *item)
{
    if (item == NULL)
    {
        return;
    }
    printf("  %s:%d\n", item->key, item->value);
}

void destroy_item(hashitem *item)
{
    if (item == NULL)
    {
        return;
    }

    free(item->key);
    free(item);
}

hashbucket *make_bucket()
{
    hashbucket *bucket = (hashbucket *)malloc(sizeof(hashbucket));
    if (bucket == NULL) 
    {
        perror("malloc");
        exit(1);
    }
    bucket->item = NULL;
    bucket->next = bucket;
    bucket->prev = bucket;
    return bucket;
}

void add_to_bucket(hashbucket *bucket, hashitem *item)
{
    if (bucket == NULL || item == NULL)
    {
        printf("add_to_bucket error!");
        exit(1);
    }

    hashbucket *new = (hashbucket *)malloc(sizeof(hashbucket));
    if (new == NULL) 
    {
        perror("malloc");
        exit(1);
    }
    new->item = item;

    // add to end of list
    new->next = bucket;
    new->prev = bucket->prev;

    bucket->prev->next = new;
    bucket->prev = new;
}

hashitem *find_in_bucket(hashbucket *bucket, char *key)
{
    if (bucket == NULL)
    {
        return NULL;
    }

    hashbucket *cur = bucket->next;
    while (cur != bucket)
    {
        if (strcmp(cur->item->key, key) == 0)
        {
            return cur->item;
        }
        cur = cur->next;
    }

    return NULL;
}

void print_bucket(hashbucket *bucket)
{
    if (bucket == NULL)
    {
        return;
    }

    hashbucket *cur = bucket->next;
    while (cur != bucket)
    {
        print_item(cur->item);
        cur = cur->next;
    }
}

void destroy_bucket(hashbucket *bucket)
{
    if (bucket == NULL)
    {
        return;
    }

    hashbucket *cur = bucket->next;
    while (cur != bucket)
    {
        cur = cur->next;
        destroy_item(cur->prev->item);
        free(cur->prev);
    }
    free(bucket);
}

int djb2_hash(hashtable *hash, char *key)
{
    if (key == NULL)
    {
        printf("djb2_hash: can't hash a NULL key!\n");
        exit(1);
    }

    // see http://www.cse.yorku.ca/~oz/hash.html
    int hashval = 5381;
    int c;

    while ((c = *key++))
    {
        hashval = ((hashval << 5) + hashval) + c; /* hashval * 33 + c */
    }

    return hashval % hash->capacity ;
}

hashtable *make_hashtable(int capacity)
{
    if (capacity < 1)
    {
        printf("make_hashtable: can't have non-positive capacity!\n");
        exit(1);
    }

    hashtable *hash = (hashtable *)malloc(sizeof(hashtable));
    if (hash == NULL)
    {
        perror("malloc");
        exit(1);
    }

    hash->capacity = capacity;

    hash->buckets = (hashbucket **)malloc(capacity*sizeof(hashbucket *));

    for (int i=0; i<capacity; ++i)
    {
        hash->buckets[i] = NULL;
    }

    return hash;
}

void hashtable_insert(hashtable *hash, char *key, int value)
{
    if (hash == NULL || key == NULL)
    {
        printf("hashtable_insert: can't have NULL hash table or key!\n");
        exit(1);
    }

    lock(&hashlock);
    int index = djb2_hash(hash, key);
    
    if (hash->buckets[index] == NULL)
    {
        hash->buckets[index] = make_bucket();
        hashitem *item = make_item(key, value);
        add_to_bucket(hash->buckets[index], item);
        unlock(&hashlock);
        return;
    }

    hashbucket *bucket = hash->buckets[index];
    hashitem *find_item = find_in_bucket(bucket, key);
    if (find_item == NULL)
    {
        lock(&hashlock);
        hashitem *item = make_item(key, value);
        add_to_bucket(bucket, item);
        unlock(&hashlock);
        return;
    }

    find_item->value = value;
}

hashitem *hashtable_search(hashtable *hash, char *key)
{
    if (hash == NULL || key == NULL)
    {
        printf("hashtable_search: can't have NULL hash table or key!\n");
        exit(1);
    }
    lock(&hashlock);
    int index = djb2_hash(hash, key);
    unlock(&hashlock);
    hashbucket *bucket = hash->buckets[index];
    if (bucket == NULL)
    {
        return NULL;
    }
    lock(&hashlock);
    return find_in_bucket(bucket, key);
}

void print_hashtable(hashtable *hash)
{
    if (hash == NULL)
    {
        return;
    }

    for (int i=0; i<hash->capacity; ++i)
    {
        printf("Bucket %d\n", i);
        print_bucket(hash->buckets[i]);
    }
}

void destroy_hashtable(hashtable *hash)
{
    if (hash == NULL)
    {
        return;
    }

    for (int i=0; i<hash->capacity; ++i)
    {
        destroy_bucket(hash->buckets[i]);
    }

    free(hash->buckets);
    free(hash);
}
