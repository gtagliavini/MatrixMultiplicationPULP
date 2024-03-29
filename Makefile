APP = matrixMul


APP_SRCS = main.c cluster.c matmul_kernels.c
APP_CFLAGS += -O3 -fno-tree-vectorize #-mno-pulp-vect  #--param iv-consider-all-candidates-bound=2

ifeq ($(findstring gcc,$(PULP_CC)),gcc)
	#GCC
	APP_CFLAGS += #-mno-memcpy -mhwloopalign
else
	#Clang
	APP_CFLAGS += 
endif	


ifdef BOARD
APP_CFLAGS += -DBOARD
endif

ifdef UNROLL
APP_CFLAGS += -DUNROLL_INNER_LOOP
endif


CORES ?= 1

APP_CFLAGS += -DNUM_CORES=${CORES}
APP_LDFLAGS +=


#PULP_ARCH_CFLAGS = -march=rv32imc_zfinx_xpulpv3
#PULP_ARCH_LDFLAGS = -march=rv32imc_zfinx_xpulpv3

# STATISTICS
APP_CFLAGS += -DSTATS

# CHECK RESULTS
APP_CFLAGS += -DDEBUG


include $(RULES_DIR)/pmsis_rules.mk
