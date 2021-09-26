#define _GNU_SOURCE
#include <assert.h>
#include "matrix.h"
#include <stdio.h>
#include <sched.h> // for GNU
#include <pthread.h>
#define MAX_THREAD 2048
unsigned 
typedef struct Argu{
	int l,r;
	int N;
	unsigned long (*A)[2048],(*B)[2048],(*C)[2048];
}ARGU;
void* task(void* argu){
	ARGU * arg = (ARGU*)argu;
	int N = arg->N;
	int l = arg->l;
	// int r = arg->r;
	unsigned long (*A)[2048] = arg->A;
	unsigned long (*B)[2048] = arg->B;
	unsigned long (*C)[2048] = arg->C;

	for (int j = 0; j < N; j++) {
		unsigned long sum = 0;	// overflow, let it go.
		for (int k = 0; k < N; k++)
			sum += A[l][k] * B[k][j];
		C[l][j] = sum;
	}
	
}
void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {
	int block = (N+MAX_THREAD-1)/MAX_THREAD;
    int c=0;//block count
    pthread_t thread[MAX_THREAD];
    ARGU arg[MAX_THREAD];
    for (int i = 0; i < N;i++ ) {//
        //printf("count=%d\n",c);
        arg[c].l = i;
        // arg[c].r = (i+block)< N ? (i+block) : N; // (i+block-1) -i + 1 = block
        arg[c].N = N;
        arg[c].A = A;
        arg[c].B = B;
        arg[c].C = C;
        // i += block;
        pthread_create(&thread[i],NULL,task,&arg[i]);
        // printf("l=%d r=%d\n",arg[c].l,arg[c].r );
        c++;
    }
    for(int i=0;i<N;i++)
    	pthread_join(thread[i],NULL);
	
}
