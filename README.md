# Matrix Multiplication

Turned a single threaded matrix multiplication program into a multi-threaded one where the main thread spawns N hardware threads and each thread must handle as equal share with the other threads. 

# Hash Table

hashtable.h and hashtable.c contain code for the hash table.  This particular hash table uses C-strings as keys and integers as values in each (key,value) item.  single_thread_test.c contains code that inserts a large number of keys into the hash table then searches the hash table for those keys to ensure that they were added correctly.  Both the inserting and search tasks are timed. <br/> 

I have made it so that multiple threads can simultaneously access the hashtable at the same time.
