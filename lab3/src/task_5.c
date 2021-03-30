#include <unistd.h>

int main( int argc, char **argv ) {
	int pid = fork();
    
	if (pid == 0) {
		execvp( "./sequential", argv);
		return 0;
	}
	return 0;
}