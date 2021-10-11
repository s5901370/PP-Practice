#include "matrix.h"
#include <omp.h>
#include <stdio.h>
void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {
    for(int i=0;i<N;++i)
        for(int j=i+1;j<N;++j){
            unsigned long t = B[i][j];
            B[i][j] = B[j][i];
            B[j][i] = t;
        }
    //omp_set_num_threads(MAXTHREAD);
    for (int i = 0; i < N; i++) {
    #pragma omp parallel for
        for (int j = 0; j < N; j++) {
            unsigned long sum = 0;    // overflow, let it go.
            for (int k = 0; k < N; k++)
                sum += A[i][k] * B[j][k];
            C[i][j] = sum;
        }
    }
}
