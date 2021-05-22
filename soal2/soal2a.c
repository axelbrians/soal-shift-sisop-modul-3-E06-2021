#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define x 4
#define y 3
#define z 6

int matrixa[x][y];
int matrixb[y][z];
int hasil[x][z];

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 1234;
int *matrix_hasil;
int cnt=0;

struct argue{
	int arg1;
	int arg2;
};

void *hitung(void *arguments){
	struct argue *args = arguments;

	int temp = 0;
	int d1=args->arg1;
	int d2=args->arg2;

	for(int i=0; i<y; i++){
		temp = temp + (matrixa[d1][i] * matrixb[i][d2]);
	}

	hasil[d1][d2] = temp;
	pthread_exit(0);
}

int main(void){
	int total=x*z;
	
	int shmid = shmget(key, sizeof(matrix_hasil), IPC_CREAT | 0666);
	matrix_hasil = shmat(shmid, 0, 0);

	for(int i=1;i<=4;i++)
		for(int j=1;j<=3;j++)
			scanf("%d",&matrixa[i][j]);
	for(int i=1;i<=3;i++)
		for(int j=1;j<=6;j++)
			scanf("%d",&matrixb[i][j]);

	for(int i=1; i<x+1; i++){
		for(int j=1; j<z+1; j++){
			struct argue *args = (struct argue *) malloc(sizeof(struct argue));
			args->arg1 = i-1;
			args->arg2 = j-1;
			pthread_attr_init(&attr);
			iret[cnt] = pthread_create(&tid[cnt], &attr, hitung, args);
			if(iret[cnt]){
				fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
				exit(EXIT_FAILURE);
			}
			cnt++;
		}
	}
	cnt=0;
	for(int i=0; i<x; i++){
		for(int j=0; j<z; j++){
			pthread_join(tid[cnt], NULL);
			printf("%d\t", hasil[i][j]);
			matrix_hasil[cnt] = hasil[i][j];
			cnt++;
		}
		printf("\n");
	}
	shmdt(matrix_hasil);
	return 0;
}