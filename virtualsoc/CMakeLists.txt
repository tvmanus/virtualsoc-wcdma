file(GLOB HEADER
  "include/virtualsoc/*/*.h"
  "include/virtualsoc/*/*.hpp")

file(GLOB SOURCE "src/*/*.cpp")

include_directories(
  "./include"
  "./src/cluster"
  "./src/core")

add_definitions(
  -DQUIET
  -DTRACELEVEL=0
  -DMAKEFILE_DEFAULT_CURRENT_ISS=ARMv6
  -DMAKEFILE_DEFAULT_CURRENT_INTERC=SINGLE_CLUSTER
  -DMAKEFILE_DEFAULT_N_CORES=4
  -DMAKEFILE_DEFAULT_CL_ICACHE_PRIV=true
  -DMAKEFILE_DEFAULT_NSIMCYCLES=-1
  -DMAKEFILE_DEFAULT_DRAM=false
  -DMAKEFILE_DEFAULT_VCD=false
  -DMAKEFILE_DEFAULT_AUTOSTARTMEASURING=false
  -DMAKEFILE_DEFAULT_MEM_IN_WS=50
  -DMAKEFILE_DEFAULT_MEM_BB_WS=1
  -DMAKEFILE_DEFAULT_STATS=false
  -DMAKEFILE_DEFAULT_STATSFILENAME="stats.txt"
  -DMAKEFILE_DEFAULT_POWERSTATS=false
  -DMAKEFILE_DEFAULT_PTRACEFILENAME="ptrace.txt"
  -DMAKEFILE_DEFAULT_DMA=false
  -DMAKEFILE_DEFAULT_N_CORES_TILE=1
  -DMAKEFILE_DEFAULT_N_TILE=1
  -DMAKEFILE_DEFAULT_N_SHR_CACHE_BANKS=16
  -DMAKEFILE_DEFAULT_CL_ICACHE_SIZE=4*1024
  -DMAKEFILE_DEFAULT_ENABLE_CRU=false
  -DMAKEFILE_DEFAULT_CRU_DEPTH=1
  -DMAKEFILE_DEFAULT_DRAM_MC_PORTS=1
  -DMAKEFILE_DEFAULT_INT_WS=1
  -DMAKEFILE_DEFAULT_L3_MUX_STATIC_MAP=false
  -DMAKEFILE_TRACE_ISS=false -DCLUSTERBUILD)

# Build the simple version of the accelerator.
add_executable(vsoc.x ${HEADER} ${SOURCE})
target_link_libraries(vsoc.x systemc simsoc dramsim pthread)

# Build the 4 per 4 push version of the hardware accelerator.
add_executable(vsoc2.x ${HEADER} ${SOURCE})
set_target_properties(vsoc2.x PROPERTIES COMPILE_DEFINITIONS "FIR_4_PER_4_PUSH")
target_link_libraries(vsoc2.x systemc simsoc dramsim pthread)

#add_test_directory(test)

