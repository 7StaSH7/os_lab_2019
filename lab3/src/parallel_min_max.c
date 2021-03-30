#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling

            if (!seed) {
                printf("Error: seed is a positive number\n");
                return -1;
            }

            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling

            if (!array_size) {
                printf("Error: array size is a positive number\n");
                return -1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling

            if (!pnum) {
                printf("Error: pnum is a positive number\n");
                return -1;
            }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int file_pipes[2];
  pipe(file_pipes);
  if (pnum > array_size / 3) pnum = array_size / 3;

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        struct MinMax min_max;

        // parallel somehow
        if (i == pnum - 1) min_max = GetMinMax(array, i * array_size / pnum, array_size);
        else min_max = GetMinMax(array, i * array_size / pnum, (i + 1) * array_size / pnum);

        if (with_files) {
          // use files here
          FILE* file;
          if (i == 0) file = fopen("data.txt", "w");
          else file = fopen("data.txt", "a+");
          fprintf(file, "%d\n", min_max.max);
	      fclose(file);

        } else write(file_pipes[1], &min_max, sizeof(struct MinMax)); // use pipe here
        
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }


  while (active_child_processes > 0) {
    // your code here
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      struct MinMax fileMinMax;
      FILE *file;
      file = fopen("data.txt", "r");
      for (int j = 0; j < i; j++) {
        fscanf(file, "%d", &fileMinMax.min);
        fscanf(file, "%d", &fileMinMax.max);
      }  
      fscanf(file, "%d", &fileMinMax.min);
      fscanf(file, "%d", &fileMinMax.max);
      fclose(file);
       min = fileMinMax.min;
       max = fileMinMax.max;

    } else {
      // read from pipes
      struct MinMax pipeMinMax;
      read(file_pipes[0], &pipeMinMax, sizeof(struct MinMax));
      min = pipeMinMax.min;
      max = pipeMinMax.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
