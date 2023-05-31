CC=gcc
CFLAGS=-g -Wall --std=c99

SRCS1 = matrix.c single_thread_matmul.c multi_thread_matmul.c
DEPS1 = matrix.h
OBJS1 = $(patsubst %.c,%.o,$(SRCS1))

OBJS1A = single_thread_matmul.o matrix.o
CMDS1A = single_thread_matmul
LIBS1A =

OBJS1B = multi_thread_matmul.o matrix.o
CMDS1B = multi_thread_matmul
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
