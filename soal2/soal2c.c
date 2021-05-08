#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>
int main() {
	int status;
	pid_t pid;
	int pipe1[2];
	int pipe2[2];

	if(pipe(pipe1) == -1){
		exit(EXIT_FAILURE);
		exit(1);
	}
	if((pid = fork()) == -1){
		exit(EXIT_FAILURE);
		exit(1);
	}else if(pid == 0){
		dup2(pipe1[1], 1);

		close(pipe1[0]);
		close(pipe1[1]);

		char *argv[] = {"ps", "aux", NULL};
        execv("/bin/ps", argv);
	}
	if(pipe(pipe2) == -1){
		exit(EXIT_FAILURE);
		exit(1);
	}
	if((pid = fork()) == -1){
		exit(EXIT_FAILURE);
		exit(1);
	}else if(pid == 0){
		while(wait(&status) > 0);
		dup2(pipe1[0], 0);
		dup2(pipe2[1], 1);

		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);

		char *argv[] = {"sort", "-nrk 3,3", NULL};
        execv("/bin/sort", argv);
	}

	if((pid = fork()) == -1){
		exit(EXIT_FAILURE);
		exit(1);
	}else if (pid == 0){
		while(wait(&status) > 0);
		dup2(pipe2[0], 0);

		close(pipe2[0]);
		close(pipe2[1]);

		char *argv[] = {"head", "-5", NULL};
        execv("/bin/head", argv);
		wait(NULL);
	}
	return 0;
}
