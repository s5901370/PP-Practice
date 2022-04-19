#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define MAXN 18
char g[MAXN][MAXN];
int n;  /* a global n */
int numSolution;
 
int ok(int position[], int next, int test)
{
  for (int i = 0; i < next; i++)
    if (position[i] == test || 
    (abs(test - position[i]) == next - i))
      return 0;
  return 1;
}
int queen(int position[], int next)
{
  if (next >= n)
    return 1;
// #pragma omp critical
//     numSolution++;
int sum = 0;
  for (int test = 0; test < n; test++){
    // printf("H");
    if(g[next][test] == '*'){
        // printf("HERE\n");
        continue;
    }
    if (ok(position, next, test)) {
      position[next] = test;
      sum += queen(position, next + 1);
    }
  }
  return sum;
}
int main() {
    int time = 1;
    while (scanf("%d\n", &n) == 1) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                scanf("%c ", &g[i][j]);
                scanf("\n");
            }
        }
        // for (int i = 0; i < n; i++) {
        //     for (int j = 0; j < n; j++) {
        //         printf("%c ", g[i][j]);
 
        //     }
        //         printf("\n");
        // }
        int position[n];
        numSolution = 0;
    //     for (int i = 0; i < next; i++)
    // if (position[i] == test || 
    // (abs(test - position[i]) == next - i))
    //   return 0;
        #pragma omp parallel for private (position) collapse(3) schedule(dynamic) reduction(+:numSolution)
          for (int i = 0; i < n; i++) {
            for(int j=0;j<n;++j){
                for(int k=0;k<n;++k){
                    if(g[0][i] == '*' || g[1][j] == '*' || g[2][k] == '*'){
                        // printf("HERE\n");
                        continue;
                    }
                    if(i==j || j==k || i==k){
                        // printf("ERE\n");
                        continue;
                    }
                    if(abs(i-j)==1 || abs(j-k) == 1 || abs(i-k) == 2){
                        // printf("RE\n");
                        continue;
                    }
                    position[0] = i;
                    position[1] = j;
                    position[2] = k;
 
                    numSolution += queen(position, 3);
                }
            }
 
          }
          printf("Case %d: %d\n", time,numSolution);
          time++;
 
    }
    return 0;
}
