#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#define MAX 67108864
#define MMAX 2147483648
#define TEST 1000
#define setbit(A,n) (A[n/32] |= (1 << n%32))
#define isbit(A,n) (A[n/32] & (1<<n%32))

// int prime[MAX]={0}; // 0 is prime
// void setbit(long int n){
//     int i = n/32;
//     int k = n%32;
//     prime[i] |= (1 << k);
// }
// int isbit(long int n){
//     int i = n/32;
//     int k = n%32;
//     //printf("n = %d i=%d k=%d ",n,i,k);
//     return prime[i]&(1 << k);
// }

int main() {
    int *prime = malloc(MAX*sizeof(int));
    memset(prime,0,sizeof(prime));
    // printf("HERE\n");
    int L,R;
    setbit(prime,1);
    #ifdef _OPENMP
    //omp_set_num_threads(8);
    #endif
    int limit = sqrt(MMAX);
    for(long int i=2;i<=limit;++i){
        if(!isbit(prime,i)){
            // printf("i = %d\n",i );
            #pragma omp parallel for 
            for(long int j=i*i;j<MMAX;j+=i){
                // printf("j=%d\n", j);
                setbit(prime,j);
            }
        }
    }
    // printf("HERE\n");
    // for(int i=1;i<TEST;++i){
    //     if(isbit(i))
    //         printf("%d",1);
    //     else
    //         printf("%d",0);
    // }
    while (scanf("%d %d", &L, &R) == 2) {
        int sum=0;
        #pragma omp parallel for reduction(+:sum)
        for(long int i=L;i<=R;++i){
            if(!isbit(prime,i)) ++sum;
        }
        printf("%d\n",sum );
    }
    return 0;
}
