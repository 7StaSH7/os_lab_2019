#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "./task1/utils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args) {
  int sum = 0;

  for (int i = args->begin; i < args->end; ++i)
  sum += args->array[i];

  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                    {"seed", required_argument, 0, 0},
                                    {"array_size", required_argument, 0, 0},
                                    {0, 0, 0, 0}};
  
  int threads_num = -1;
  int seed = -1;
  int array_size = -1;
  
  while (true) {    
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0: {
        switch (option_index)  {
          case 0: {
            threads_num = atoi(optarg);
            if (threads_num <= 0){
              printf("\nThe number of threads is a positive number!\n\n");
              return 1;
            }
          }
          break;

          case 1: {
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("\nSeed is a positive number!\n\n");
              return 1;
            }
          }  
          break;

          case 2: {
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("\nArray_size is a positive number!\n\n");
              return 1;
            }
          }
          break;

          default:
            printf("\nIndex %d is out of options\n\n", option_index);
        }
      }
      break;

      case '?':
      break;

      default:
        printf("\ngetopt returned character code 0%o?\n\n", c);
    }
  }

  pthread_t threads[threads_num];
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct SumArgs args[threads_num];

  for (int i = 0; i < threads_num; ++i) {
    args[i].array = array;
    args[i].begin = i * array_size / threads_num;
    
    if (i == threads_num - 1) args[i].end = array_size;
    else args[i].end = (i + 1) * array_size / threads_num;
  }

  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args)) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }
  
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  for (int i = 0; i < threads_num; ++i) pthread_join(threads[i], NULL);

  free(array);
  printf("Total: %d\n", total_sum);
  printf("Elapsed time: %fms\n\n", elapsed_time);
  return 0;
}
