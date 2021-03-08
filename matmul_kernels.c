#include "pmsis.h"


void __attribute__ ((noinline)) matMul(int * __restrict__ pSrcA, int  * __restrict__ pSrcB, int * __restrict__ pDstC, int M, int N, int O) {


    uint32_t i = 0; // loop counter for M
    uint32_t j = 0; // loop counter for N
    uint32_t k = 0; // loop counter for O

    int core_id = pi_core_id();

    // printf("core id: %i, start: %i, end: %i\n", core_id, START, END);

    for (k = core_id; k < O / 2; k += NUM_CORES) {
        for (i = 0; i < M / 2; i++) {

            int32_t sum00 = 0;
            int32_t sum01 = 0;
            int32_t sum10 = 0;
            int32_t sum11 = 0;

#ifdef UNROLL_INNER_LOOP
            for (j = 0; j < N/2; j++) {
                int32_t AVal0 = pSrcA[i * 2 * N + (j*2)];
                int32_t AVal1 = pSrcA[i * 2 * N + N + (j*2)];

                int32_t BVal0 = pSrcB[(j*2) * O + (k * 2)];
                int32_t BVal1 = pSrcB[(j*2) * O + (k * 2 + 1)];

                sum00 = sum00 + AVal0 * BVal0;
                sum01 = sum01 + AVal0 * BVal1;
                sum10 = sum10 + AVal1 * BVal0;
                sum11 = sum11 + AVal1 * BVal1;	        	

                AVal0 = pSrcA[i * 2 * N + (j*2+1)];
                AVal1 = pSrcA[i * 2 * N + N + (j*2+1)];
                BVal0 = pSrcB[(j*2+1) * O + (k * 2)];
                BVal1 = pSrcB[(j*2+1) * O + (k * 2 + 1)];
		

                sum00 = sum00 + AVal0 * BVal0;
                sum01 = sum01 + AVal0 * BVal1;
                sum10 = sum10 + AVal1 * BVal0;
                sum11 = sum11 + AVal1 * BVal1;


            }
	    
#else	    
            for (j = 0; j < N; j++) {
                int32_t AVal0 = pSrcA[i * 2 * N + (j)];
                int32_t AVal1 = pSrcA[i * 2 * N + N + (j)];

                int32_t BVal0 = pSrcB[j * O + (k * 2)];
                int32_t BVal1 = pSrcB[j * O + (k * 2 + 1)];

                sum00 = sum00 + AVal0 * BVal0;
                sum01 = sum01 + AVal0 * BVal1;
                sum10 = sum10 + AVal1 * BVal0;
                sum11 = sum11 + AVal1 * BVal1;
            }
#endif	    

            pDstC[(i * 2) * O + k * 2] = sum00;
            pDstC[(i * 2) * O + k * 2 + 1] = sum01;
            pDstC[(i * 2 + 1) * O + k * 2] = sum10;
            pDstC[(i * 2 + 1) * O + k * 2 + 1] = sum11;
        }
    }

    // clean up code
    i = i * 2;
#ifdef UNROLL_INNER_LOOP    
    j = j * 2;
#else
    j = j;
#endif
    k = k * 2;
    // check if every index is nicely finished
    if (i == M && j == N && k >= O) {

    } else {
        uint32_t iEnd = i;
        uint32_t jEnd = j;
        uint32_t kEnd = k >= O ? O : k;

        // clean up for j
        if (jEnd != N) {
            for (i = 0; i < iEnd; i++) {
                for (k = 0; k < kEnd; k += NUM_CORES) {
                    int32_t sum = 0;
                    for (j = jEnd; j < N; j++) {
                        sum += sum + pSrcA[i * N + j] * pSrcB[j * O + k];
                    }
                    pDstC[i * O + k] += sum;
                }
            }
        }

        // clean up for i
        if (iEnd != M) {
            for (k = core_id; k < kEnd; k += NUM_CORES) {
                for (i = iEnd; i < M; i++) {
                    int32_t sum = 0;
                    for (j = 0; j < N; j++) {
                        sum = sum + pSrcA[i * N + j] * pSrcB[j * O + k];
                    }
                    pDstC[i * O + k] = sum;
                }
            }
        }

        // clean up for k
        for (k = kEnd; k < O; k += NUM_CORES) {
            for (i = 0; i < M; i++) {
                int32_t sum = 0;
                for (j = 0; j < N; j++) {
                    sum = sum + pSrcA[i * N + j] * pSrcB[j * O + k];
                }
                pDstC[i * O + k] = sum;
            }
        }
    }

    pi_cl_team_barrier(0);


}

