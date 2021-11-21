#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>
#define endl printf("\n")
int main(int argc, char const *argv[])
{
    int Aw,Ah,Bw,Bh;
    int count =1;
    while(scanf("%d %d %d %d",&Ah,&Aw,&Bh,&Bw) == 4){
        int A[Ah][Aw],B[Bh][Bw];
 
        for(int i=0;i<Ah;++i)
            for(int j=0;j<Aw;++j)
                scanf("%d",&(A[i][j]));
        for(int i=0;i<Bh;++i)
            for(int j=0;j<Bw;++j)
                scanf("%d",&(B[i][j]));
        // for(int i=0;i<Ah;++i){
        //     for(int j=0;j<Aw;++j)
        //         printf("%d ",A[i][j] );
        //     printf("\n");
        // }
 
        int W = Aw - Bw;
        int H = Ah - Bh;
        int max = INT_MAX;
        int x=501,y=501;
        # pragma  omp parallel for 
        for(int i=0;i<=H;++i){
            for(int j=0;j<=W;++j){
                int sum=0;
                for(int k=0;k<Bh;++k){
                    for(int p=0;p<Bw;++p){
                        // if(count == 17) printf("%d ",A[i+k][j+p]);
                        sum += (A[i+k][j+p]-B[k][p])*(A[i+k][j+p]-B[k][p]);
                    }
                    // endl;
                }
 
                // printf("%d %d sum = %d\n",i,j,sum);
                #pragma omp critical
                    if(sum<max)
                        x=i,y=j,max = sum;
                        // if(count == 17) printf("%d %d sum = %d\n",i+1,j+1,sum);
                    else if(sum == max){
                        // if(count == 17) printf("%d %d same sum = %d\n",i+1,j+1,sum);
 
                        if(i < x){
                            x = i;
                            y = j;
                        }
                        else if((i == x) &&(j < y)){
                            x = i;
                            y = j;
                        }
                    }
        }
 
        }
        printf("%d %d\n",x+1,y+1);
        count++;
    }//while
    return 0;
}
