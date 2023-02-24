#include "pmsis.h"
#include "config.h"

#pragma GCC push_options
#pragma GCC optimize ("-fivopts")
void __attribute__ ((noinline)) matMul(Scalar * __restrict__ pSrcA, Scalar * __restrict__ pSrcB, ScalarResult * __restrict__ pDstC, int M, int N, int O) {


    int i; // loop counter for M
    int j; // loop counter for N
    int k; // loop counter for O

    int core_id = pi_core_id();

    if(M<=0 || N<=0 || O<=0) return;

    for (k = core_id; k < O/2; k += NUM_CORES) {

        for (i = 0; i < M/2; i++) {

            ScalarResult sum00 = 0;
            ScalarResult sum01 = 0;
            ScalarResult sum10 = 0;
            ScalarResult sum11 = 0;

#ifdef UNROLL_INNER_LOOP
            for (j = 0; j < N/2; j++) {
                Scalar AVal0 = pSrcA[i * 2 * N + (j*2)];
                Scalar AVal1 = pSrcA[i * 2 * N + N + (j*2)];
                Scalar BVal0 = pSrcB[(j*2) * O + (k * 2)];
                Scalar BVal1 = pSrcB[(j*2) * O + (k * 2 + 1)];

                Scalar AVal2 = pSrcA[i * 2 * N + (j*2+1)];
                Scalar AVal3 = pSrcA[i * 2 * N + N + (j*2+1)];
                Scalar BVal2 = pSrcB[(j*2+1) * O + (k * 2)];
                Scalar BVal3 = pSrcB[(j*2+1) * O + (k * 2 + 1)];

                sum00 = sum00 + AVal0 * BVal0 + AVal2 * BVal2;
                sum01 = sum01 + AVal0 * BVal1 + AVal2 * BVal3;
                sum10 = sum10 + AVal1 * BVal0 + AVal3 * BVal2;
                sum11 = sum11 + AVal1 * BVal1 + AVal3 * BVal3;	        	
            }
	    
#else	   
	     
            for (j = 0; j < N; j++) {
                Scalar AVal0 = pSrcA[i * 2 * N + (j)];
                Scalar AVal1 = pSrcA[i * 2 * N + N + (j)];

                Scalar BVal0 = pSrcB[j * O + (k * 2)];
                Scalar BVal1 = pSrcB[j * O + (k * 2 + 1)];

                sum00 = sum00 + (ScalarResult) AVal0 * (ScalarResult) BVal0;
                sum01 = sum01 + (ScalarResult) AVal0 * (ScalarResult) BVal1;
                sum10 = sum10 + (ScalarResult) AVal1 * (ScalarResult) BVal0;
                sum11 = sum11 + (ScalarResult) AVal1 * (ScalarResult) BVal1;
            }
#endif	    

            pDstC[(i * 2) * O + k * 2] = sum00;
            pDstC[(i * 2) * O + k * 2 + 1] = sum01;
	    pDstC[(i * 2 + 1) * O + k * 2] = sum10;
            pDstC[(i * 2 + 1) * O + k * 2 + 1] = sum11;

        } // i 
    } // k
    // clean up code
    i = i * 2;
#ifdef UNROLL_INNER_LOOP    
    j = j * 2;
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

#pragma GCC pop_options
