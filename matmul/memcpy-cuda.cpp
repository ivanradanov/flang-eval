#include <cuda_runtime.h>
#include <iostream>
#include <chrono>

#define ftype double

#define CHECK_CUDA(X) \
  do { \
    cudaError_t rstatus = X; \
    if (rstatus != cudaSuccess) { \
      fprintf(stderr, "cuda error: %s (%d) %s %d", cudaGetErrorString(rstatus), \
          rstatus, __FILE__, __LINE__); \
      abort(); \
    } \
  } while (0)


static inline void DoMatmul(ftype *A, ftype *B, unsigned N) {
    double hAlpha = 1;
    CHECK_CUDA(cudaMemcpy(A, B, N * sizeof(ftype), cudaMemcpyDeviceToDevice));
}

int main() {

  // unsigned N = 12800;
  unsigned N = 20480 * 20480;
  ftype *A, *B;
  CHECK_CUDA(cudaMalloc(&A, sizeof(ftype) * N));
  CHECK_CUDA(cudaMalloc(&B, sizeof(ftype) * N));

  ftype *hA, *hB;
  hA = new ftype[N];
  hB = new ftype[N];

  unsigned REPEATS = 50;
  unsigned WARMUPS = 5;
  for (unsigned i = 0; i < WARMUPS; i++) {
      CHECK_CUDA(cudaMemcpy(A, hA, N * sizeof(ftype), cudaMemcpyHostToDevice));
      CHECK_CUDA(cudaMemcpy(B, hB, N * sizeof(ftype), cudaMemcpyHostToDevice));
      DoMatmul(A, B, N);
      CHECK_CUDA(cudaDeviceSynchronize());
      CHECK_CUDA(cudaMemcpy(hA, A, N * sizeof(ftype), cudaMemcpyDeviceToHost));
      CHECK_CUDA(cudaMemcpy(hB, B, N * sizeof(ftype), cudaMemcpyDeviceToHost));
  }
  for (unsigned i = 0; i < REPEATS; i++) {
      auto starta = std::chrono::high_resolution_clock::now();
      CHECK_CUDA(cudaMemcpy(A, hA, N * sizeof(ftype), cudaMemcpyHostToDevice));
      CHECK_CUDA(cudaMemcpy(B, hB, N * sizeof(ftype), cudaMemcpyHostToDevice));
      auto start = std::chrono::high_resolution_clock::now();
      DoMatmul(A, B, N);
      CHECK_CUDA(cudaDeviceSynchronize());
      auto end = std::chrono::high_resolution_clock::now();
      CHECK_CUDA(cudaMemcpy(hA, A, N * sizeof(ftype), cudaMemcpyDeviceToHost));
      CHECK_CUDA(cudaMemcpy(hB, B, N * sizeof(ftype), cudaMemcpyDeviceToHost));
      auto enda = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> duration = end - start;
      std::chrono::duration<double> durationa = enda - starta;
      auto nanoseconds_duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
      auto nanoseconds_durationa = std::chrono::duration_cast<std::chrono::seconds>(enda - starta);
      std::cout << "Time computation: " << duration.count() << " seconds" << std::endl;
      std::cout << "Time all: " << durationa.count() << " seconds" << std::endl;
  }
  return 0;
}
