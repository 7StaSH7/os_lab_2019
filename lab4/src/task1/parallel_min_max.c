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
#include <signal.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t* ChildPid;
int activeChildProcesses = 0;
int pnum;

void kill_child(int argo) {
  int status;
  int isKilled = -1;
  for (int i = 0 ; i < pnum; i++) {
    int result = waitpid(ChildPid[i], &status, WNOHANG);
        if (result == 0) {
          isKilled = kill(ChildPid[i], SIGKILL);
          if (isKilled == 0) printf("\nChild %d was killed\n", ChildPid[i]);
          else printf("\nChild %d wasn't killed\n", ChildPid[i]);
        } 
        else {
            printf("ChildPid №%d was finished\n", ChildPid[i]);
            activeChildProcesses--; 
        }
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  int alarm_time = -1;
  bool with_files = false;

  while (true) {
  	int current_optind = optind ? optind : 1;
	
	static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
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
                    if (seed <= 0) {
                        printf("seed should be positive!\n\n");
                        return 1;
                    }
                    break;

                case 1:
                    array_size = atoi(optarg);
                    // your code here
                    if (array_size <= 0) {
                        printf("array_size should be positive!\n\n");
                        return 1;
                    }
                    break;

                case 2:
                    pnum = atoi(optarg);
                    // your code here
                    if (pnum <= 0) {
                        printf("pnum should be positive!\n\n");
                        return 1;
                    }
                    break;

                case 3:
                    alarm_time = atoi(optarg);
                    if (alarm_time <= 0) {
                        printf("alarm_time should be positive!\n\n");
                        return 1;
                    }
                    break;

                case 4:
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
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" --timeout \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipefd[2];
  pipe(pipefd);  
  
  ChildPid = malloc(sizeof(int) * pnum);

  for (int i = 0; i < pnum; i++) {  
    	pid_t child_pid = fork();
        if (child_pid >= 0) {
        // successful fork
        ChildPid[i] = child_pid;
            activeChildProcesses += 1;
            
            if (child_pid == 0) {
                // child process
                // parallel somehow
                struct MinMax myMinMax;
            
                if (i != pnum - 1) myMinMax = GetMinMax(array, i * array_size / pnum, (i + 1) * array_size / pnum);
                else myMinMax = GetMinMax(array,i * array_size / pnum, array_size);
            
                if (with_files) {
                    // use files here
                    FILE* file = fopen("task1.txt", "a");
                    fwrite(&myMinMax, sizeof(struct MinMax), 1, file);
                    fclose(file);
                    exit(0);
                } 
                else {
                    // use pipe here
                    write(pipefd[1], &myMinMax, sizeof(struct MinMax));
                    exit(0);
                }
                return 0;
            }
        }
    else {
        printf("Fork failed!\n");
        return 1;
    }
  }

  if(alarm_time != -1) {
    signal(SIGALRM, kill_child); 
    alarm(alarm_time);
    pause();
  }


  while (activeChildProcesses > 0) {
    // your code here
    close(pipefd[1]);
    wait(NULL);
    activeChildProcesses -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  
  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    struct MinMax myMinMax;	  
	  
    if (with_files) {
      // read from files
      FILE* file = fopen("task1.txt", "r");
      fseek(file, i * sizeof(struct MinMax), SEEK_SET);
      fread(&myMinMax, sizeof(struct MinMax), 1, file);
      fclose(file);	
    } 
    // read from pipes
    else read(pipefd[0], &myMinMax, sizeof(struct MinMax));

    min = myMinMax.min;
    max = myMinMax.max;
	
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