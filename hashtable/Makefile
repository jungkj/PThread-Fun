CC=gcc
CFLAGS=-g -Wall --std=c99

SRCS1 = hashtable.c single_thread_test.c multi_thread_test.c
DEPS1 = hashtable.h
OBJS1 = $(patsubst %.c,%.o,$(SRCS1))

OBJS1A = single_thread_test.o hashtable.o
CMDS1A = single_thread_test
LIBS1A =

OBJS1B = multi_thread_test.o hashtable.o
CMDS1B = multi_thread_test
LIBS1B = -lpthread

.PHONY: all
all: $(CMDS1A) $(CMDS1B)

$(OBJS1): %.o: %.c $(DEPS1)
	$(CC) $(CFLAGS) -c -o $@ $<

$(CMDS1A): %: $(OBJS1A)
	$(CC) $(CFLAGS) -o $@ $(OBJS1A) $(LIBS1A)

$(CMDS1B): %: $(OBJS1B)
	$(CC) $(CFLAGS) -o $@ $(OBJS1B) $(LIBS1B)

.PHONY: clean
clean:
	/bin/rm -f $(OBJS1) $(CMDS1A) $(CMDS1B)
