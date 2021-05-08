#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define x 4
#define y 3
#define z 6

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 1234;
int *matrixa;
int matrixb[x*z];

struct argue{
	int arg1;
	int arg2;
};


void *hitung(void *arguments){
	struct argue *args = arguments;

	unsigned long long temp = 1;
	int d1=args->arg1;
	int d2=args->arg2;

	if(d1==0 || d2==0){
		temp = 0;
	}else if(d1>=d2){
		for(int i=d1-d2;i<=d1;i++){
			temp *= 1LL*i;
		}
	}else{
		for(int i=1;i<=d1;i++){
			temp *= 1LL*i;
		}
	}
	printf("%llu\t", temp);
	pthread_exit(0);
}

int main(void){
	int total=x*z;
	
	int shmid = shmget(key, sizeof(matrixa), IPC_CREAT | 0666);
	matrixa = shmat(shmid, 0, 0);

	int cnt=0;
	for(int i=1; i<x+1; i++){
		for(int j=1; j<z+1; j++){
			printf("%d\t", matrixa[cnt]);
			cnt++;
		}
		printf("\n");
	}
	printf("\n");

	cnt=0;
	for(int i=1; i<x+1; i++){
		for(int j=1; j<z+1; j++){
			scanf("%d",&matrixb[cnt]);
			cnt++;
		}
	}
	printf("\n");

	cnt=0;
	for(int i=1; i<x+1; i++){
		for(int j=1; j<z+1; j++){
			struct argue *args = (struct argue *) malloc(sizeof(struct argue));
			args->arg1 = matrixa[cnt];
			args->arg2 = matrixb[cnt];
			pthread_attr_init(&attr);
			iret[cnt] = pthread_create(&tid[cnt], &attr, hitung, args);
			if(iret[cnt]){
				fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
				exit(EXIT_FAILURE);
			}
			pthread_join(tid[cnt], NULL);
			cnt++;
		}
		printf("\n");
	}

	for(int i=0; i<total; i++){
		pthread_join(tid[i], NULL);
	}

	shmdt(matrixa);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
