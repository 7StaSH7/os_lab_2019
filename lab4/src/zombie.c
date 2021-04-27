#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    pid_t pid = fork();
    if (pid > 0) sleep(30);
    else exit(0);
    return 0;
}