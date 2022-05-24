#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define KERNEL_LOCAL_SZ 8

#define MAXN 2048
char host_Mtx[2][MAXN*MAXN];

__device__ inline void move_border(char g[][8+2], char *IN,
    int localX, int localY, int localSz, int x, int y, int binN) {
    if (localX == 1) {
        g[localX-1][localY] = IN[(x-1) * binN + y];
        if (localY == 1)
            g[localX-1][localY-1] = IN[(x-1) * binN + (y-1)];
        if (localY == localSz)
            g[localX-1][localY+1] = IN[(x-1) * binN + (y+1)];
    }
    if (localY == 1)    g[localX][localY-1] = IN[x * binN + (y-1)];
    if (localY == localSz)    g[localX][localY+1] = IN[x * binN + (y+1)];
    if (localX == localSz) {
        g[localX+1][localY] = IN[(x+1) * binN + y];
        if (localY == 1)
            g[localX+1][localY-1] = IN[(x+1) * binN + (y-1)];
        if (localY == localSz)
            g[localX+1][localY+1] = IN[(x+1) * binN + (y+1)];
    }
}
__global__ void simulate(char *IN, char *OUT, int *binN,int *N) {
    int localX = threadIdx.x+1;
    int localY = threadIdx.y+1;
    int x = blockIdx.x*8+localX;
    int y = blockIdx.y*8+localY;
    int localSz = 8;//8
    // printf("x = %d,y = %d, localX = %d, locaY = %d, localSz = %d\n",x,y,localX,localY,localSz);
    __shared__ char g[8+2][8+2];
 
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
 
    // move itself to local
    char t = IN[x * (*binN) + y];
    g[localX][localY] = t;
    // move border to local
    move_border(g, IN, localX, localY, localSz, x, y,*binN);
    __syncthreads();
    // barrier(CLK_LOCAL_MEM_FENCE);
 
    if (x > *N || y > *N)    return ;
 
    int adj = 0;
    for (int i = 0; i < 8; i++) {
        int cx = localX + dx[i];
        int cy = localY + dy[i];
        adj += g[cx][cy];
    }
    OUT[x * (*binN) + y] = (t == 0 && adj == 3) || (t == 1 && (adj == 2 || adj == 3));
}

int main(void)
{
  char *device_IN[2];
  int * device_binN;
  int * device_N;
  int binN;
  int size = sizeof(char) * MAXN*MAXN;

  cudaMalloc((void **)&device_IN[0], size);
  cudaMalloc((void **)&device_IN[1], size);
  cudaMalloc((void **)&device_binN,(int)sizeof(int));
  cudaMalloc((void **)&device_N,(int)sizeof(int));

  int N,M;
  assert(scanf("%d %d", &N, &M) == 2);
  static char str[2048][2048];
  for (int i = 1; i <= N; i++)
      assert(scanf("%s", str[i]+1) == 1);
  binN = N+2;
  while (binN % KERNEL_LOCAL_SZ)
      binN++;
  // printf("binN = %d\n",binN );
  memset(host_Mtx, 0, sizeof(host_Mtx));
  for (int i = 1; i <= N; i++) {
      for (int j = 1; j <= N; j++)
          host_Mtx[0][i*binN + j] = str[i][j] - '0';
  } 
  // for (int i = 1; i <= N; i++) {
  //       for (int j = 1; j <= N; j++)
  //           putchar(host_Mtx[0][i*binN+j]+'0');
  //       puts("");
  //   }

  int flag = 0;
  dim3 block(KERNEL_LOCAL_SZ, KERNEL_LOCAL_SZ);
  dim3 grid(binN/KERNEL_LOCAL_SZ, binN/KERNEL_LOCAL_SZ);
  cudaMemcpy(device_IN[0], host_Mtx[0], size, cudaMemcpyHostToDevice);
  cudaMemcpy(device_IN[1], host_Mtx[1], size, cudaMemcpyHostToDevice);
  cudaMemcpy(device_binN, &binN, sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(device_N, &N, sizeof(int), cudaMemcpyHostToDevice);
  for (int it = 0; it < M; it++) {
        // -- set argument to kernel
        // clStat = clSetKernelArg(clKrn[0], 0, sizeof(cl_mem), &clMemIn[0][flag]);
        // CheckFailAndExit(clStat);
        // clStat = clSetKernelArg(clKrn[0], 1, sizeof(cl_mem), &clMemIn[0][!flag]);
        // CheckFailAndExit(clStat);
 
  simulate <<< grid, block >>> ((char *)device_IN[flag], (char *)device_IN[!flag],(int *)device_binN,(int*)device_N);
        // -- execute
        // clStat = clEnqueueNDRangeKernel(clQue[0], clKrn[0], 2, globalOffset,
        //         globalSize, localSize, 0, NULL, NULL);
        // CheckFailAndExit(clStat);
        flag = !flag;
  }
  cudaMemcpy(host_Mtx[flag], device_IN[flag], sizeof(char)*binN*binN, cudaMemcpyDeviceToHost);
  // cudaMemcpy(host_Mtx[flag], device_IN[flag], size, cudaMemcpyDeviceToHost);
    // -- read back
    // clStat = clEnqueueReadBuffer(clQue[0], clMemIn[0][flag], CL_TRUE, 0,
    //         sizeof(char)*binN*binN, hostMtx[flag], 0, NULL, NULL);
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++)
            putchar(host_Mtx[flag][i*binN+j]+'0');
        puts("");
    }
        // puts("");


  


  cudaFree(device_IN[0]);
  cudaFree(device_IN[1]);
  cudaFree(device_binN);
  cudaFree(device_N);
  // free(host_A);
  // free(host_B);
  // free(host_C);
}
			
