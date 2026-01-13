#include <cuda_runtime_api.h>
#include <cublas_v2.h>
#include <iostream>
#include <chrono>

#define ftype double

#define CHECK_CUDA(X) \
  do { \
    cudaError_t rstatus = X; \
    if (rstatus != cudaSuccess) { \
      fprintf(stderr, "cuda error: %s (%d) %s %d\n", \
              cudaGetErrorString(rstatus), rstatus, __FILE__, __LINE__); \
      abort(); \
    } \
  } while (0)

#define CHECK_CUBLAS(X) \
  do { \
    cublasStatus_t rstatus = X; \
    if (rstatus != CUBLAS_STATUS_SUCCESS) { \
      fprintf(stderr, "cublas error: %d %s %d\n", \
              rstatus, __FILE__, __LINE__); \
      abort(); \
    } \
  } while (0)

static inline void DoMatmul(ftype *A, ftype *B, unsigned N, cublasHandle_t handle)
{
    double hAlpha = 1.0;
    CHECK_CUBLAS(
        cublasDaxpy(handle, N, &hAlpha, A, 1, B, 1)
    );
}

int main()
{
    cublasHandle_t handle;
    CHECK_CUBLAS(cublasCreate(&handle));
    CHECK_CUBLAS(
        cublasSetPointerMode(handle, CUBLAS_POINTER_MODE_HOST)
    );

    // unsigned N = 12800;
    unsigned N = 20480 * 20480;

    ftype *A, *B;
    CHECK_CUDA(cudaMalloc((void**)&A, sizeof(ftype) * N));
    CHECK_CUDA(cudaMalloc((void**)&B, sizeof(ftype) * N));

    ftype *hA = new ftype[N];
    ftype *hB = new ftype[N];

    unsigned REPEATS = 50;
    unsigned WARMUPS = 5;

    for (unsigned i = 0; i < WARMUPS; i++) {
        CHECK_CUDA(cudaMemcpy(A, hA, sizeof(ftype) * N, cudaMemcpyHostToDevice));
        CHECK_CUDA(cudaMemcpy(B, hB, sizeof(ftype) * N, cudaMemcpyHostToDevice));

        DoMatmul(A, B, N, handle);
        CHECK_CUDA(cudaDeviceSynchronize());

        CHECK_CUDA(cudaMemcpy(hA, A, sizeof(ftype) * N, cudaMemcpyDeviceToHost));
        CHECK_CUDA(cudaMemcpy(hB, B, sizeof(ftype) * N, cudaMemcpyDeviceToHost));
    }

    for (unsigned i = 0; i < REPEATS; i++) {
        auto starta = std::chrono::high_resolution_clock::now();

        CHECK_CUDA(cudaMemcpy(A, hA, sizeof(ftype) * N, cudaMemcpyHostToDevice));
        CHECK_CUDA(cudaMemcpy(B, hB, sizeof(ftype) * N, cudaMemcpyHostToDevice));

        auto start = std::chrono::high_resolution_clock::now();
        DoMatmul(A, B, N, handle);
        CHECK_CUDA(cudaDeviceSynchronize());
        auto end = std::chrono::high_resolution_clock::now();

        CHECK_CUDA(cudaMemcpy(hA, A, sizeof(ftype) * N, cudaMemcpyDeviceToHost));
        CHECK_CUDA(cudaMemcpy(hB, B, sizeof(ftype) * N, cudaMemcpyDeviceToHost));

        auto enda = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end - start;
        std::chrono::duration<double> durationa = enda - starta;

        std::cout << "Time computation: " << duration.count() << " seconds\n";
        std::cout << "Time all: " << durationa.count() << " seconds\n";
    }

    CHECK_CUBLAS(cublasDestroy(handle));
    CHECK_CUDA(cudaFree(A));
    CHECK_CUDA(cudaFree(B));

    delete[] hA;
    delete[] hB;
}
