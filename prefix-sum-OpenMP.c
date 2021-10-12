#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils.h"
#include <omp.h>
#define MAXN 10000005
#define MAX_THREAD 6
uint32_t prefix_sum[MAXN];
int main() {
    int n;
    omp_set_num_threads(MAX_THREAD);
    uint32_t key;
    while (scanf("%d %" PRIu32, &n, &key) == 2) {
        int block = (n+MAX_THREAD-1)/MAX_THREAD;
        uint32_t mid[MAX_THREAD];
        #pragma omp parallel for //reduction(+:prefix_sum)
        for (int i = 0; i < MAX_THREAD;i++) {
            uint32_t sum = 0;
            int l = i*block+1;
            int r = (i+1)*block <n ? (i+1)*block : n; // (i+block-1) -i + 1 = block
            // printf("thead %d l=%d,r=%d\n",omp_get_thread_num(),l,r);
            for(int j=l;j<=r;++j){
           // printf("thead %d \n",omp_get_thread_num());
                sum += encrypt(j, key);
                prefix_sum[j] = sum;
            }
            mid[i] = prefix_sum[r];
        }
                    // printf("thead %d i = \n",omp_get_thread_num());
 
        for(int i=1;i<MAX_THREAD;i++)
            mid[i]+=mid[i-1];
 
        #pragma omp parallel for //reduction(+:prefix_sum)
        for (int i = 1; i < MAX_THREAD;i++) {
            int l = i*block+1;
            int r = (i+1)*block <n ? (i+1)*block : n; // (i+block-1) -i + 1 = block
 
            for(int j=l;j<=r;++j){
                prefix_sum[j] += mid[i-1];
            }
        }
        output(prefix_sum, n);
    }
    return 0;
}
