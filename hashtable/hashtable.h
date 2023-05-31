#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct _hashitem
{
    char *key;
    int value;
} hashitem;

typedef struct _hashbucket
{
    hashitem *item;
    struct _hashbucket *next;
    struct _hashbucket *prev;
} hashbucket;

typedef struct _hashtable
{
    hashbucket **buckets;
    int capacity;
} hashtable;

hashtable *make_hashtable(int capacity);
void hashtable_insert(hashtable *hash, char *key, int value);
hashitem *hashtable_search(hashtable *hash, char *key);
void print_hashtable(hashtable *hash);
void destroy_hashtable(hashtable *hash);

#endif
