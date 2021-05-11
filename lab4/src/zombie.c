#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main () {
    pid_t childPid;

    for (int i = 0; i < 5; i++) childPid = fork(); 
    if (childPid == 0) exit(0); 
    else if (childPid > 0) printf("Created a child process %d\n", childPid);
    sleep(30);
    
    return 0;
}