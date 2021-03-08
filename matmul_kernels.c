#include "pmsis.h"


void __attribute__ ((noinline)) matMul(int * __restrict__ pSrcA, int  * __restrict__ pSrcB, int * __restrict__ pDstC, int M, int N, int O) {


    uint32_t i = 0; // loop counter for M
    uint32_t j = 0; // loop counter for N
    uint32_t k = 0; // loop counter for O

    int core_id = pi_core_id();

    // printf("core id: %i, start: %i, end: %i\n", core_id, START, END);

    for (k = core_id; k < O; k += NUM_CORES) {
        for (i = 0; i < M; i++) {

            int32_t sum = 0;

            for (j = 0; j < N; j++) {
                int32_t AVal = pSrcA[i * N + j];
                int32_t BVal = pSrcB[j * O + k];

                sum = sum + AVal * BVal;
            }

            pDstC[i * O + k] = sum;
        }
    }

    pi_cl_team_barrier(0);


}

