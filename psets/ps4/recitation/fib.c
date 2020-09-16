#include <stdio.h>
#include <omp.h>

u_int32_t fib(int n) {
  if (n < 2) return n;
  else {
    return fib(n-1) + fib(n-2);
  }
}

int main(int argc, char* argv[]) {
  int i = 0;
  int k = atoi(argv[1]);

  // Use only as many threads as are available
  #ifdef _OPENMP
    omp_set_num_threads(omp_get_num_procs());
  #endif

  // Privatizes i, and lets each thread compute "round robin"-style, thus
  // preventing only one thread executing the last and heaviest block.
  #pragma omp parallel private(i)
  {
    #pragma omp for schedule(dynamic, 1)
    for (i=0; i<=k; i++) {
      printf("Fib(%d): %u\n", i, fib(i));
    }
  }
}
