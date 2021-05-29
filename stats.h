#ifndef _STATS_H
#define _STATS_H

#define HOTTING 2
#define REPEAT  5

#ifdef BOARD

#include "stats_board.h"

#else

#ifdef STATS

#define INIT_STATS()  

#define ENTER_STATS_LOOP()  \
    unsigned long _cycles = 0; \
    unsigned long _instr = 0; \
    unsigned long _active = 0; \
    unsigned long _ldext = 0; \
    unsigned long _stext = 0; \
    unsigned long _tcdmcont = 0; \
    unsigned long _ldstall = 0; \
    unsigned long _imiss = 0; \
    unsigned long _jrstall = 0; \
    unsigned long _branch = 0; \
    for(int _k=0; _k<HOTTING+REPEAT; _k++) { \
      pi_perf_conf((1<<PI_PERF_CYCLES) | (1<<PI_PERF_INSTR) | (1<<PI_PERF_ACTIVE_CYCLES) | (1<<PI_PERF_LD_EXT) | (1<<PI_PERF_TCDM_CONT) | (1<<PI_PERF_LD_STALL) | (1<<PI_PERF_IMISS)  | (1<<PI_PERF_ST_EXT) | (1<<PI_PERF_JR_STALL) | (1<<PI_PERF_BRANCH) );


#define START_STATS()  \
    pi_perf_reset(); \
    pi_perf_start();

#define STOP_STATS() \
     pi_perf_stop(); \
     if (_k >= HOTTING) \
      { \
        _cycles   += pi_perf_read (PI_PERF_CYCLES); \
        _instr    += pi_perf_read (PI_PERF_INSTR); \
    	_active   += pi_perf_read (PI_PERF_ACTIVE_CYCLES); \
        _ldext    += pi_perf_read (PI_PERF_LD_EXT); \
        _stext    += pi_perf_read (PI_PERF_ST_EXT); \
   	_tcdmcont += pi_perf_read (PI_PERF_TCDM_CONT); \
    	_ldstall  += pi_perf_read (PI_PERF_LD_STALL); \
        _imiss    += pi_perf_read (PI_PERF_IMISS); \
        _jrstall  += pi_perf_read (PI_PERF_JR_STALL); \
        _branch   += pi_perf_read (PI_PERF_BRANCH); \
      }

#define EXIT_STATS_LOOP()  \
    } \
    printf("[%d] num_cycles: %lu\n", pi_core_id(), _cycles/REPEAT); \
    printf("[%d] num_instr_miss: %lu\n", pi_core_id(), _imiss/REPEAT); \
    printf("[%d] num_ext_load: %lu\n", pi_core_id(), _ldext/REPEAT); \
    printf("[%d] num_ext_store: %lu\n", pi_core_id(), _stext/REPEAT); \
    printf("[%d] num_tcdm_contentions: %lu\n", pi_core_id(), _tcdmcont/REPEAT); \
    printf("[%d] num_instrs: %lu\n", pi_core_id(), _instr/REPEAT); \
    printf("[%d] num_active_cycles: %lu\n", pi_core_id(), _active/REPEAT); \
    printf("[%d] num_load_stalls: %lu\n", pi_core_id(), _ldstall/REPEAT); \
    printf("[%d] num_jumpr_stalls: %lu\n", pi_core_id(), _jrstall/REPEAT); \
    printf("[%d] num_branch: %lu\n", pi_core_id(), _branch/REPEAT);

#else // STATS

#define INIT_STATS()
#define ENTER_STATS_LOOP()
#define START_STATS()
#define STOP_STATS()
#define EXIT_STATS_LOOP()

#endif  // STATS


#endif // WOLFE

#endif
