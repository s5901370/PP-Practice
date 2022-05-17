/* header */
 
#include <stdio.h>
#include <assert.h>
#include <CL/cl.h>
#include <stdint.h>
#define UINT uint32_t
#define MAXGPU 10
#define MAXK 1024
#define MAXLOG 4096
#define MAXN 1024*1024
cl_uint IN[6][MAXN],TMP[2][MAXN];
/* main */
void rand_gen(UINT c, int N, UINT A[MAXN]) {
    UINT x = 2, n = N*N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x = (x * x + c + i + j)%n;
            A[i*N+j] = x;
        }
    }
}
UINT signature(int N, UINT A[MAXN]) {
    UINT h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            h = (h + A[i*N+j]) * 2654435761LU;
    }
    return h;
}
 
int main(int argc, char *argv[])
{
  cl_int status;
  cl_platform_id platform_id;
  cl_uint platform_id_got;
  status = clGetPlatformIDs(1, &platform_id,&platform_id_got);
  assert(status == CL_SUCCESS && platform_id_got == 1);
  cl_device_id GPU[MAXGPU];
  cl_uint GPU_id_got;
  status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU,MAXGPU, GPU, &GPU_id_got);
  assert(status == CL_SUCCESS);
  /* getcontext */
  cl_context context = clCreateContext(0, 1, GPU, 0, 0,&status);
  assert(status == CL_SUCCESS);
  /* commandqueue */
  // const cl_queue_properties properties[] =
  //   {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
  cl_command_queue commandQueue = clCreateCommandQueueWithProperties(context, GPU[0],0, &status);
  assert(status == CL_SUCCESS);
  /* kernelsource */
  FILE *kernelfp = fopen("matrix-lib.cl", "r");
  assert(kernelfp != 0);
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
  cl_program program = clCreateProgramWithSource(context, 1, &constKernelSource, &kernelLength, &status);
  assert(status == CL_SUCCESS);
  /* buildprogram */
  status = clBuildProgram(program, 1, GPU, 0, 0, 0);
  if(status != CL_SUCCESS){
    char log[MAXLOG];
  size_t logLength;
  clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, MAXLOG, log, &logLength);
  puts(log);
  }
  assert(status == CL_SUCCESS);
  // printf("Build program completes\n");
  /* createkernel */
  cl_kernel kernelm = clCreateKernel(program, "mul", &status);
  cl_kernel kernela= clCreateKernel(program, "add", &status);
  assert(status == CL_SUCCESS);
  int N, S[6];
    scanf("%d", &N);
    for (int i = 0; i < 6; i++) {
        scanf("%d", &S[i]);
        rand_gen(S[i], N, IN[i]);
    }
 
 
  /* createbuffer */
    cl_mem bufferIN[6];
    for (int i = 0; i < 6; ++i){
      bufferIN[i] = clCreateBuffer(context ,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * N * sizeof(cl_uint), IN[i], &status);
      assert(status == CL_SUCCESS);
    }
    cl_mem bufferTMP[6];
    for (int i = 0; i < 6; ++i){
      bufferTMP[i] = clCreateBuffer(context ,CL_MEM_READ_WRITE, N * N * sizeof(cl_uint), 0, &status);
      assert(status == CL_SUCCESS);
    }
 
 
    size_t globalThreads[] = {(size_t)N*N};
  size_t localThreads[] = {1};
  status = clSetKernelArg(kernelm, 3, sizeof(cl_int), &N);
 
   // AB
    // multiply(N, IN[0], IN[1], TMP[0]);
  status = clSetKernelArg(kernelm, 0, sizeof(cl_mem), (void*)&bufferIN[0]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 1, sizeof(cl_mem), (void*)&bufferIN[1]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 2, sizeof(cl_mem), (void*)&bufferTMP[0]);
  assert(status == CL_SUCCESS);
status = clEnqueueNDRangeKernel(commandQueue, kernelm, 1, 0, globalThreads, localThreads, 0, 0, 0);
clFinish(commandQueue);
    // CD
    // multiply(N, IN[2], IN[3], TMP[1]);
status = clSetKernelArg(kernelm, 0, sizeof(cl_mem), (void*)&bufferIN[2]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 1, sizeof(cl_mem), (void*)&bufferIN[3]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 2, sizeof(cl_mem), (void*)&bufferTMP[1]);
  assert(status == CL_SUCCESS);
status = clEnqueueNDRangeKernel(commandQueue, kernelm, 1, 0, globalThreads, localThreads, 0, 0, 0);
clFinish(commandQueue);
 
    // AB+CD
    // add(N, TMP[0], TMP[1], TMP[2]);
status = clSetKernelArg(kernela, 0, sizeof(cl_mem), (void*)&bufferTMP[0]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernela, 1, sizeof(cl_mem), (void*)&bufferTMP[1]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernela, 2, sizeof(cl_mem), (void*)&bufferTMP[2]);
  assert(status == CL_SUCCESS);
status = clEnqueueNDRangeKernel(commandQueue, kernela, 1, 0, globalThreads, localThreads, 0, 0, 0);
clFinish(commandQueue);
clEnqueueReadBuffer(commandQueue, bufferTMP[2], CL_TRUE,
          0, N *N* sizeof(cl_uint), TMP[2],
          0, NULL, NULL);
 
    printf("%u\n", signature(N, TMP[2]));
 
    // ABE
    // multiply(N, TMP[0], IN[4], TMP[3]);
status = clSetKernelArg(kernelm, 0, sizeof(cl_mem), (void*)&bufferTMP[0]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 1, sizeof(cl_mem), (void*)&bufferIN[4]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 2, sizeof(cl_mem), (void*)&bufferTMP[3]);
  assert(status == CL_SUCCESS);
status = clEnqueueNDRangeKernel(commandQueue, kernelm, 1, 0, globalThreads, localThreads, 0, 0, 0);
clFinish(commandQueue);
    // CDF
    // multiply(N, TMP[1], IN[5], TMP[4]);
status = clSetKernelArg(kernelm, 0, sizeof(cl_mem), (void*)&bufferTMP[1]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 1, sizeof(cl_mem), (void*)&bufferIN[5]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernelm, 2, sizeof(cl_mem), (void*)&bufferTMP[4]);
  assert(status == CL_SUCCESS);
status = clEnqueueNDRangeKernel(commandQueue, kernelm, 1, 0, globalThreads, localThreads, 0, 0, 0);
clFinish(commandQueue);
    // ABE+CDF
    // add(N, TMP[3], TMP[4], TMP[5]);
status = clSetKernelArg(kernela, 0, sizeof(cl_mem), (void*)&bufferTMP[3]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernela, 1, sizeof(cl_mem), (void*)&bufferTMP[4]);
  assert(status == CL_SUCCESS);
  status = clSetKernelArg(kernela, 2, sizeof(cl_mem), (void*)&bufferTMP[5]);
  assert(status == CL_SUCCESS);
status = clEnqueueNDRangeKernel(commandQueue, kernela, 1, 0, globalThreads, localThreads, 0, 0, 0);
clFinish(commandQueue);
clEnqueueReadBuffer(commandQueue, bufferTMP[5], CL_TRUE,
          0, N *N* sizeof(cl_uint), TMP[5],
          0, NULL, NULL);
    printf("%u\n", signature(N, TMP[5]));
 
  /* setarg */
 
 
 
 
  clReleaseContext(context);  /* context etcmake */
  clReleaseCommandQueue(commandQueue);
  clReleaseProgram(program);
  clReleaseKernel(kernelm);
  clReleaseKernel(kernela);
    for (int i = 0; i < 6; ++i){
      clReleaseMemObject(bufferIN[i]);
      clReleaseMemObject(bufferTMP[i]);
    }
 
  return 0;
}
/* kernel
__kernel void add(__global unsigned int  *A,
        __global unsigned int *B,
        __global unsigned int *C) {
        int i = get_global_id(0);
            C[i] = A[i] + B[i];
}
 
 
__kernel void mul(__global unsigned int *A, 
                  __global unsigned int *B, 
                  __global unsigned int *C,
                  int N) {
  int id = get_global_id(0);
  int a = id/N, b = id%N;
  unsigned int sum = 0;    // overflow, let it go.
  for (int i = 0; i < N; i++) {
        sum += A[a*N+i] * B[b+i*N];
    }
    C[id] = sum;
}
/* end */

*/

/* end */
