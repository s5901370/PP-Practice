#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define endl printf("\n")
#define MAXM 1000000
#define MAXN 10000
#define max(a,b)(a>b?a:b)
int dp[2][MAXM+5];
int W[MAXN+5],V[MAXN+5];
int main(int argc, char const *argv[])
{
    int N,M;
    while(scanf("%d %d",&N,&M) == 2){
        for(int i=1;i<=N;++i)
            scanf("%d %d",&W[i],&V[i]);
        memset(dp,0,sizeof(dp));
        int a = 0; // iterate between row 0 and row 1
 
        for(int i=1;i<=N;++i){
            int b = 1-a;
            int v = V[i],w = W[i];
            #pragma omp parallel for
            for(int j=0;j<w;++j){
                dp[b][j] = dp[a][j];
            }
            #pragma omp parallel for
            for(int j=w;j<=M;++j){
                dp[b][j] = max(dp[a][j],dp[a][j-w]+v);
            }
            a = 1-a;
        }
        // printf("AAA\n");
        printf("%d\n",dp[a][M]);
    }
 
 
 
    return 0;
}
