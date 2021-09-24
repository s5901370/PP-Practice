#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils.h"
#include <sched.h> // for GNU
#include <assert.h>
#include <pthread.h>


#define MAXN 10000005
#define MAX_THREAD 8
uint32_t key;
uint32_t prefix_sum[MAXN];
typedef struct argu{
    int l,r;
    uint32_t *pre;//array
    uint32_t inter;// mid 
}ARGU;
void* task1(void* argu){
    ARGU *arg=(ARGU*)argu;
    int l=arg->l;
    int r=arg->r;
    uint32_t sum=0;
    for(int i=l;i<=r;i++){
        sum += encrypt(i, key);
        prefix_sum[i] = sum;

    }
    //printf("left=%d,right=%d\n",l,r);
}
void* task2(void* argu){
    ARGU *arg=(ARGU*)argu;
    int l=arg->l;
    int r=arg->r;
    for(int i=l;i<=r;i++){
        prefix_sum[i] += arg->inter;
    }
}
int main() {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 0; i < 6; i++)
            CPU_SET(i, &cpuset); //set CPU 0~5
    assert(sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0);

    int n;
    while (scanf("%d %" PRIu32, &n, &key) == 2) {
        uint32_t sum = 0;
        //int stage = (int)ceil(log2(n));
        int block = (n+MAX_THREAD-1)/MAX_THREAD;
        int c=0;//block count
        pthread_t thread[MAX_THREAD];
        ARGU arg[MAX_THREAD];
        for (int i = 1; i <= n; ) {//prefix sum start at 1
            //printf("count=%d\n",c);
            arg[c].l = i;
            arg[c].r = (i+block-1)<n ? (i+block-1) : n; // (i+block-1) -i + 1 = block(tree questionXD)
            i += block;
            pthread_create(&thread[c],NULL,task1,&arg[c]);
            c++;
        }
        //printf("c=%d\n",c);
        uint32_t mid[c];

        for(int i=0;i<c;i++){
            pthread_join(thread[i],NULL);
            mid[i] = prefix_sum[arg[i].r]; //get intermediate prefix sum
        }
        for(int i=1;i<c;i++)
            mid[i]+=mid[i-1];;;
        for(int i=1;i<c;i++){
            arg[i].inter = mid[i-1];
            pthread_create(&thread[i],NULL,task2,&arg[i]);
        }
        for(int i=1;i<c;i++){
            pthread_join(thread[i],NULL);
        }


        // for(int i=1;i<=n;i++)
        //     printf("%d\n",prefix_sum[i]);///////////////
        // printf("\n\n");
        // sum=0;/////////////
        // for (int i = 1; i <= n; i++) {////////////////
        //     sum += encrypt(i, key);
        //     prefix_sum[i] = sum;
        //     printf("%d\n",prefix_sum[i]);
        // }

        output(prefix_sum, n);
    }
    return 0;
}