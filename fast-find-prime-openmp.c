#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#define MAX 46340
#define MMAX 2147483648LL
#define TEST 1000
#define MAXTHREAD 8
bool *prim = NULL;
int size=0;
int p[4800];
 
void task(bool *prim,int l,int r,int L){
    if(l==1)prim[0]=1;
    for(int j=0;j<size;++j){
        int x=p[j];
        if(x*x > r){
         break;
     }
        int re = l/x;
        if(re<=1)
            re = 2*x;
        else if(l%x){
            re = (l/x +1)*x;
        }
        else{
            re = l;
        }
        for(int k=re;k<=r;k+=x){
            prim[k-L]=1;
        }
    }
 
}
int task2(bool *prim,int l,int r,int L){
    int sum=0;
    for(int j=l;j<=r;j++){
            // printf("j=%d bool=%d ",j,prim[j-l]);
        if(prim[j-L] == 0){
            ++sum;
        }
    }
    return sum;
}
 
int main() {
    // printf("%d\n",sizeof(bool));
    bool prime[MAX+1] = {0};
    // printf("HERE\n");
    int L,R;
    int limit = sqrt(MAX);
    for(int i=2;i<=MAX;++i){
        if(!prime[i]){
            // printf("i = %d\n",i );
            //#pragma omp parallel for 
            for(int j=i*i;j<=MAX;j+=i){
                // printf("j=%d\n", j);
                prime[j]=1;;
            }
        }
    }
    for(int i=2;i<=MAX;++i){
        if(!prime[i]){
            p[size++]=i;
        }
    }
    // for (int i = 0; i < size; ++i)
    // {
    //     printf("%d ",p[i]);
    // }
    // printf("size = %d\n", size);;
    // printf("here\n");
    while (scanf("%d %d", &L, &R) == 2) {
        int block = (R-L+MAXTHREAD)/MAXTHREAD;
        int sum=0;
        prim =  (bool*)calloc((R-L+10),sizeof(bool));//all is prime
        // for(int w=0;w<=R-L+1;++w){
        //     if(prim[w]!=0)printf("NOT ZERO\n");
        //         // prim[w]=0;
        // }
        int chunk = 1<<17;
        #pragma omp parallel for reduction(+:sum)
        for(int i=L;i<=R;i+=chunk){
            int l=i;
            int r = i+chunk-1 < R ? i+chunk-1 :R;
            // printf("thead %d,l %d, r %d\n",omp_get_thread_num(),l,r);
            // memset(prim,0,sizeof(prim));
            task(prim,l,r,L);
            sum += task2(prim,l,r,L);
 
        }
        // #pragma omp parallel for reduction(+:sum)
        // for(int i=L;i<=R;i+=chunk){
            // int l=i;
            // int r = i+chunk-1 < R ? i+chunk-1 :R;
            // printf("thead %d,l %d, r %d\n",omp_get_thread_num(),l,r);
            // memset(prim,0,sizeof(prim));
 
        // }
        if(prim != NULL) 
            free(prim);
        // prim = NULL;
        printf("%d\n",sum );
    }
    return 0;
}
