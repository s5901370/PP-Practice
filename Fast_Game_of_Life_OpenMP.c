#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define endl printf("\n")
#define get(A,i,j) A[i-1][j-1]+A[i-1][j]+A[i-1][j+1]+A[i][j-1]+A[i][j+1]+A[i+1][j-1]+A[i+1][j]+A[i+1][j+1]
#define MAXM 1005
#define MAXN 2005
int N,M;
int arr[2][MAXN][MAXN];

int main(int argc, char const *argv[])
{
	// omp_set_num_threads(8);
	while(scanf("%d %d\n",&N,&M) == 2){
		char line[MAXN];
		memset(arr,0,sizeof(arr));
		int a = 0;
		for(int i=1;i<=N;++i){
			scanf("%s",line);
			for(int j=1;j<=N;++j)
				arr[a][i][j] = line[j-1]-'0';
		}
		// for(int i=0;i<N;++i){
		// 	for(int j=0;j<N;++j)
		// 		printf("%c",arr[a][i*N+j]);
		// 	endl;
		// }
		//memset(arr[a],0,sizeof(arr[a]));
		for(int i=0;i<M;++i){
			#pragma omp parallel for schedule(static, 32)
			for(int j=1;j<=N;++j){
			// #pragma omp parallel for schedule(static, 32)
				for(int k=1;k<=N;++k){
					int ret = get(arr[a],j,k);
					// int dead = 0;
					// if(arr[a][j][k] == 1){
					// 	if((ret == 2) || (ret == 3))
					// 		dead = 1;
					// 	else{
					// 		dead = 0;
					// 	}
					// }
					// else if(arr[a][j][k] == 0){
					// 	if(ret == 3)
					// 		dead = 1;
					// 	else
					// 		dead = 0;
					// }
					arr[1-a][j][k] = (arr[a][j][k] == 1 && (ret == 2 || ret == 3) )
					|| (arr[a][j][k] == 0 && ret == 3);
					
				}
			}
			a = 1-a;
		}

		for(int i=1;i<=N;++i){
			for(int j=1;j<=N;++j)
				printf("%d",arr[a][i][j]);
			printf("\n");
		}
		// printf("HERE\n");
		// printf("%d\n",arr[0]);
		// printf("%d\n",arr[0]);
	}
	
	

	return 0;
}
