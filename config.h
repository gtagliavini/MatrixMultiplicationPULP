#ifndef _CONFIG_MATMUL_
#define _CONFIG_MATMUL_

// Matrix Multiplication: C = A*B where A is M1*K1, B is K1*N1
#define M1 32
#define K1 32
#define N1 32

// 16 means float16
#define DATA_TYPE 16

// Floating-point values
#define IS_FLOAT 1

#ifdef FABRIC
#define DATA_LOCATION
#else
#define DATA_LOCATION PI_L1
#endif


#if IS_FLOAT


#if DATA_TYPE == 16
typedef float16alt Scalar;
#undef USE_INTRINSICS
typedef float16alt ScalarResult;
#else
typedef float Scalar;
#undef USE_INTRINSICS
typedef float ScalarResult;
#endif

#else
typedef signed int ScalarResult;

#if DATA_TYPE == 16
typedef signed short Scalar;
typedef signed short Vector __attribute__((vector_size (4)));
#define SumDotp(a, b, c) __builtin_pulp_sdotsp2(a, b, c)
#define Dotp(a, b)       __builtin_pulp_dotsp2(a, b)
#define SHIFT_FACTOR 1
#elif DATA_TYPE == 8
typedef signed char Scalar;
typedef signed char Vector __attribute__((vector_size (4)));
#define SumDotp(a, b, c) __builtin_pulp_sdotsp4(a, b, c)
#define Dotp(a, b)       __builtin_pulp_dotsp4(a, b)
#define SHIFT_FACTOR 2
#else
typedef signed int Scalar;
#undef USE_INTRINSICS
#endif

#endif //IS_FLOAT

void  matMul(Scalar* __restrict__ A, Scalar * __restrict__ B, ScalarResult * __restrict__ C, int N, int M, int K);

#endif
