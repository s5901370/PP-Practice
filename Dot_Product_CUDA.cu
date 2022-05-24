#include <stdio.h>
#include <cuda.h>
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
__device__ static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
__device__ static inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}
 
#define MAXN 16777216
#define Size 512
#define Block MAXN/Size


__global__ void hello(uint32_t C[Block],int* N,uint32_t* key1,uint32_t* key2)
{
  uint32_t sum=0 ,a,b;
  int left = blockIdx.x*Size;
  int right = (blockIdx.x+1)*Size < *N ? (blockIdx.x+1)*Size : *N;
  for (int k = left; k < right; k++){
    a = encrypt(k, *key1);
    b = encrypt(k, *key2);
    sum += a * b;
  }
  C[blockIdx.x] = sum;
}

int main(void)
{
  int *device_N;
  uint32_t *device_C,*device_key1,*device_key2;
  uint32_t  *host_C,*host_key1,*host_key2;
  int *host_N;
  int size = sizeof(uint32_t) * Block;

  // dim3 blocks(Size, Size);

  // cudaMalloc((void *)&device_A, size);
  // cudaMalloc((void *)&device_B, size);
  cudaMalloc((void **)&device_C, size);
  cudaMalloc((void **)&device_N, (int)sizeof(int));
  cudaMalloc((void **)&device_key1, (int)sizeof(uint32_t));
  cudaMalloc((void **)&device_key2, (int)sizeof(uint32_t));

  // host_A = (uint32_t *)malloc(size);
  // host_B = (uint32_t *)malloc(size);
  host_C = (uint32_t *)malloc(size);
  host_N = (int *)malloc(sizeof(int));
  host_key1 = (uint32_t *)malloc(sizeof(uint32_t));
  host_key2 = (uint32_t *)malloc(sizeof(uint32_t));


    while (scanf("%d %" PRIu32 " %" PRIu32, host_N, host_key1, host_key2) == 3) {
        // int chunk = N / 4;
        // for (int i = 0; i < N; i++) {
        //     A[i] = encrypt(i, key1);
        //     B[i] = encrypt(i, key2);
        // }
 
        // for (int i = 0; i < N; i++)
        //     C[i] = A[i] * B[i];
       cudaMemcpy(device_N,host_N,sizeof(int),cudaMemcpyHostToDevice);
      cudaMemcpy(device_key1,host_key1,sizeof(uint32_t),cudaMemcpyHostToDevice);
      cudaMemcpy(device_key2,host_key2,sizeof(uint32_t),cudaMemcpyHostToDevice);
        int ex_N = (*host_N-1) / Size + 1;
          hello <<< ex_N, 1 >>> ((uint32_t *) device_C,(int *)device_N,(uint32_t*)device_key1,(uint32_t *)device_key2);
      cudaMemcpy(host_C, device_C, size, cudaMemcpyDeviceToHost);


      uint32_t sum = 0;
        for (int i = 0; i < ex_N; i++)
            sum += host_C[i];
        printf("%" PRIu32 "\n", sum);
    }

  // cudaMemcpy(device_A, host_A, size, cudaMemcpyHostToDevice);
  // cudaMemcpy(device_B, host_B, size, cudaMemcpyHostToDevice);



  // int k = 0;
  // for (int i = 0; i < Size; i++)
  //   for (int j = 0; j < Size; j++)
  //     printf("host_C[%d][%d] = %d\n", i, j, host_C[k++]);

  // cudaFree(device_A);
  // cudaFree(device_B);
  cudaFree(device_C);
  cudaFree(device_N);
  cudaFree(device_key1);
  cudaFree(device_key2);
  // free(host_A);
  // free(host_B);
  free(host_C);
  free(host_N);
  free(host_key1);
  free(host_key2);
}
      
