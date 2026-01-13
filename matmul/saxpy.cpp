#include <hip/hip_runtime_api.h>
#include <rocblas/rocblas.h>
#include <iostream>

#define ftype double

#define CHECK_HIP(X) \
  do { \
    hipError_t rstatus = X; \
    if (rstatus != hipSuccess) { \
      fprintf(stderr, "hip error: %s (%d) %s %d", hipGetErrorString(rstatus), \
          rstatus, __FILE__, __LINE__); \
      abort(); \
    } \
  } while (0)

#define CHECK_ROCBLAS(X) \
  do { \
    rocblas_status rstatus = X; \
    if (rstatus != rocblas_status_success) { \
      fprintf(stderr, "rocblas error: %s (%d) %s %d", \
          rocblas_status_to_string(rstatus), rstatus, __FILE__, __LINE__); \
      abort(); \
    } \
  } while (0)

static inline void DoMatmul(ftype *A, ftype *B, unsigned N, rocblas_handle handle) {
    double hAlpha = 1;
    CHECK_ROCBLAS(rocblas_daxpy(handle, N, &hAlpha, A, 1, B, 1));
}

int main() {
  rocblas_handle handle;
  CHECK_ROCBLAS(rocblas_create_handle(&handle));
  CHECK_ROCBLAS(rocblas_set_pointer_mode(handle, rocblas_pointer_mode_host));

  // unsigned N = 12800;
  unsigned N = 20480 * 20480;
  ftype *A, *B;
  CHECK_HIP(hipMalloc(&A, sizeof(ftype) * N));
  CHECK_HIP(hipMalloc(&B, sizeof(ftype) * N));

  ftype *hA, *hB;
  hA = new ftype[N];
  hB = new ftype[N];

  unsigned REPEATS = 50;
  unsigned WARMUPS = 5;
  for (unsigned i = 0; i < WARMUPS; i++) {
      CHECK_HIP(hipMemcpy(A, hA, N * sizeof(ftype), hipMemcpyHostToDevice));
      CHECK_HIP(hipMemcpy(B, hB, N * sizeof(ftype), hipMemcpyHostToDevice));
      DoMatmul(A, B, N, handle);
      CHECK_HIP(hipDeviceSynchronize());
      CHECK_HIP(hipMemcpy(hA, A, N * sizeof(ftype), hipMemcpyDeviceToHost));
      CHECK_HIP(hipMemcpy(hB, B, N * sizeof(ftype), hipMemcpyDeviceToHost));
  }
  for (unsigned i = 0; i < REPEATS; i++) {
      auto starta = std::chrono::high_resolution_clock::now();
      CHECK_HIP(hipMemcpy(A, hA, N * sizeof(ftype), hipMemcpyHostToDevice));
      CHECK_HIP(hipMemcpy(B, hB, N * sizeof(ftype), hipMemcpyHostToDevice));
      auto start = std::chrono::high_resolution_clock::now();
      DoMatmul(A, B, N, handle);
      CHECK_HIP(hipDeviceSynchronize());
      auto end = std::chrono::high_resolution_clock::now();
      CHECK_HIP(hipMemcpy(hA, A, N * sizeof(ftype), hipMemcpyDeviceToHost));
      CHECK_HIP(hipMemcpy(hB, B, N * sizeof(ftype), hipMemcpyDeviceToHost));
      auto enda = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> duration = end - start;
      std::chrono::duration<double> durationa = enda - starta;
      auto nanoseconds_duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
      auto nanoseconds_durationa = std::chrono::duration_cast<std::chrono::seconds>(enda - starta);
      std::cout << "Time computation: " << duration.count() << " seconds" << std::endl;
      std::cout << "Time all: " << durationa.count() << " seconds" << std::endl;
  }

  CHECK_ROCBLAS(rocblas_destroy_handle(handle));
}
