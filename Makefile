APP = matrixMul


APP_SRCS = main.c cluster.c matmul_kernels.c
APP_CFLAGS += -O3 -g3 

ifeq ($(findstring gcc,$(PULP_CC)),gcc)
	#GCC
	APP_CFLAGS += -mno-memcpy -mhwloopalign
else
	#Clang
	APP_CFLAGS += 
endif	


ifdef BOARD
APP_CFLAGS += -DBOARD
endif

CORES ?= 1

APP_CFLAGS += -DNUM_CORES=${CORES}
APP_LDFLAGS +=


# STATISTICS
APP_CFLAGS += -DSTATS

# CHECK RESULTS
APP_CFLAGS += -DDEBUG


include $(RULES_DIR)/pmsis_rules.mk
