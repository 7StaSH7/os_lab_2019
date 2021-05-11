CC=gcc
CFLAGS=-I.

all: parallel process_memory parallel_sum

parallel: ./task1/utils.o ./task1/find_min_max.o ./task1/utils.h ./task1/find_min_max.h
	$(CC) -o ./task1/parallel ./task1/utils.o ./task1/find_min_max.o ./task1/parallel_min_max.c $(CFLAGS)

utils.o : ./task1/utils.h
	$(CC) -o ./task1/utils.o -c ./task1/utils.c $(CFLAGS)

find_min_max.o : ./task1/utils.h ./task1/find_min_max.h
	$(CC) -o ./task1/find_min_max.o -c ./task1/find_min_max.c $(CFLAGS)

process_memory:
	$(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_sum: ./task1/utils.o ./task1/utils.h
	$(CC) -o psum ./task1/utils.o -pthread parallel_sum.c $(CFLAGS)

clean :
	rm ./task1/utils.o ./task1/find_min_max.o ./task1/parallel process_memory psum