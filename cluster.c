#include "pmsis.h"

#include "stats.h"

//#define DEBUG

#define M1 32
#define N1 32
#define K1 32

typedef int Scalar;
typedef int ScalarResult;

PI_L1 Scalar matA[M1*K1] __attribute__ ((aligned (4)));
PI_L1 Scalar matB[K1*N1] __attribute__ ((aligned (4)));
PI_L1 ScalarResult  matC[M1*N1] __attribute__ ((aligned (4)));


void __attribute__ ((noinline)) matMul(int * __restrict__ pSrcA, int  * __restrict__ pSrcB, int * __restrict__ pDstC, int M, int N, int O);

// Helper functions

void __attribute__ ((noinline)) matrix_init(Scalar * __restrict__ A, Scalar * __restrict__ B, ScalarResult * __restrict__ C) {
  for (int i = 0; i < M1; i++) {
    for (int j = 0; j < K1; j++) {
      A[i*K1+j] = (int)(j+1);
    }
  }

  for (int i = 0; i <K1; i++) {
    for (int j = 0; j < N1; j++) {
      B[i*N1+j] = i+1;
    }
  }

  for (int i = 0; i < M1; i++) {
    for (int j = 0; j < N1; j++) {
      C[i*N1+j] = 0;
    }
  }

}

unsigned int __attribute__ ((noinline)) matrix_check(ScalarResult * __restrict__ C) {
  unsigned int errors = 0;
  // check
  for (int i = 0; i < M1; i++) {
    for (int j = 0; j < N1; j++) {
      ScalarResult ref_val = ((K1)*(K1+1)*(2*(K1)+1))/6;
      if (C[i*N1+j] != ref_val) {
        printf("Error at index (%d,%d): %d instead of %d\n", i, j, C[i*N1+j], ref_val);
        errors++;
      }
    }
  }
  return errors;
}


// Cluster entry point

void main_fn() {

  volatile int m = M1;
  volatile int n = N1;
  volatile int k = K1;

  matrix_init(matA, matB, matC);

  INIT_STATS();

  ENTER_STATS_LOOP();

  START_STATS();

  matMul(matA, matB, matC, m, n, k);


  STOP_STATS();


#ifdef STATS
  if(_k==HOTTING+REPEAT-1) printf("MACs=%d\n", (M1*K1*N1));
#endif
  EXIT_STATS_LOOP();
  //printf("[%d] MAC/cycle: %.4f\n", pi_core_id(), (double)(M1*K1*N1)/(_cycles/REPEAT));

#ifdef DEBUG  
  matrix_check(matC);
#endif  

}
