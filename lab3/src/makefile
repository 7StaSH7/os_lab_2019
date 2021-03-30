CC=gcc
CFLAGS=-I.

all : sequential_min_max parallel_min_max task_5

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

task_5 : task_5.c 
	$(CC) -o task_5  task_5.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential parallel
