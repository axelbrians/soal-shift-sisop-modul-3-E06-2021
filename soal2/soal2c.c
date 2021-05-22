#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>
int pid;
int pipe1[2];
int pipe2[2];
int main(){
	// create pipe1
	if (pipe(pipe1) == -1) {
		perror("bad pipe1");
		exit(1);
	}
	// fork (ps aux)
	if ((pid = fork()) == -1) {
		perror("bad fork1");
		exit(1);
	}else if (pid == 0) {
		// input from stdin
		// output to pipe1
		dup2(pipe1[1], 1);
		// close fds
		close(pipe1[0]);
		close(pipe1[1]);
		// exec
		execlp("ps", "ps", "aux", NULL);
		// exec didn't work, exit
		perror("bad exec ps");
		exit(1);
	}
	// parent
	
	// create pipe2
	if (pipe(pipe2) == -1) {
		perror("bad pipe2");
		exit(1);
	}
	// fork (sort -nrk 3,3)
	if ((pid = fork()) == -1) {
		perror("bad fork2");
		exit(1);
	} else if (pid == 0) {
		// input from pipe1
		dup2(pipe1[0], 0);
		// output to pipe2
		dup2(pipe2[1], 1);
		// close fds
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		// exec
		execlp("sort", "sort", "-nrk 3,3", NULL);
		// exec didn't work, exit
		perror("bad exec sort -nrk 3,3");
		exit(1);
	}
	// parent

	// close unused fds
	close(pipe1[0]);
	close(pipe1[1]);
	
	// fork (head -5)
	if ((pid = fork()) == -1) {
		perror("bad fork3");
		exit(1);
	}else if (pid == 0){
		// input from pipe2
		dup2(pipe2[0], 0);
		// output to stdout (already done)
		// close fds
		close(pipe2[0]);
		close(pipe2[1]);
		// exec
		execlp("head", "head", "-5", NULL);
		// exec didn't work, exit
		perror("bad exec head -5");
		exit(1);
	}
	// parent
}