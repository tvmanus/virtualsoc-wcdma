############## Name of output executable file
ifdef EXENAME
EXE=o-optimize/$(EXENAME).exe
else
EXE=o-optimize/app.exe
endif

#################### COMPILATION SETTINGS ##########################
## Default compiler and possible option sets
CC        = arm-elf-gcc
LD        = arm-elf-ld
AS        = arm-elf-as
OBJCOPY   = arm-elf-objcopy
OBJDUMP   = arm-elf-objdump

## Paths
LIBGOMP_PATH      = ${VSOC_APP_DIR}/libgomp
VSOC_CONFIG_PATH  = ${LIBGOMP_PATH}/config/vsoc
SUPPORT_PATH	  = ${VSOC_APP_DIR}/support/simulator

## Library FLAGS
# Enables safe memory allocation check
LIBGOMP_FLAGS	+= -DMEMCHECK_MALLOCS
LIBGOMP_FLAGS	+= -DSTACK_IN_SHARED
LIBGOMP_FLAGS	+= -DSINGLE_WS

## Other FLags
INCDIR       	= \
				-I. -I.. \
				-I${SUPPORT_PATH} \
				-I${LIBGOMP_PATH} \
				-I${VSOC_CONFIG_PATH} \
				-I${VSOC_SRC_DIR}/core \
				-I${VSOC_SRC_DIR} \
				-I${VSOC_APP_DIR}/support \
				-I${VSOC_APP_DIR}/gomp \
				-I${VSOC_APP_DIR}/gomp/config/vsoc
COMMON_CFLAGS 	= $(INCDIR)  -msoft-float -mcpu=arm7 -Winline
CFLAGS	 	= -O2 -g $(COMMON_CFLAGS) $(LIBGOMP_FLAGS)
ASFLAGS    	= -mfpu=softfpa
#OMP_CFLAGS   	= -O2 -Wall ${COMMON_CFLAGS} -fopenmp -DSIZE320
LDFLAGS	    	+= -L${VSOC_APP_DIR}/support/lib -lgcc


#################### FILES ##########################
#CSRCS  += ${LIBGOMP_PATH}/root.c
CSRCS  += ${VSOC_APP_DIR}/support/simulator/appsupport.c
CSRCS  += ${VSOC_APP_DIR}/support/simulator/accsupport.c
CSRCS  += ${VSOC_APP_DIR}/support/simulator/countersupport.c

OMP_CSRCS  = hw_acceleration.c

ASSRCS = ${SUPPORT_PATH}/boot.s


#################### OBJECTS ##########################
COBJS  		= $(CSRCS:.c=.o)
ASOBJS 		= $(ASSRCS:.s=.o)
OMP_COBJS  	= $(OMP_CSRCS:.c=.o)
OBJS   		= $(COBJS) $(ASOBJS)


#################### BUILD ##########################
all: $(EXE)

$(EXE): $(OBJS)
	test -d o-optimize || mkdir o-optimize
	$(CC) $(CFLAGS) -c -o $(OMP_COBJS) $(OMP_CSRCS)
	$(LD) -T ${SUPPORT_PATH}/vsoc.ld -o linked.o $(OBJS) $(OMP_COBJS) $(LDFLAGS)
	$(OBJCOPY) -O binary linked.o $@
	ln -sf o-optimize/app.exe TargetMem_1.mem

run:
	vsoc.x --tb=32 -c 2 --intc=c --ics=16

run_stat:
	vsoc.x --tb=32 -c 2 --intc=c --ics=16 -s

.PHONY : clean disassemble

clean:
	rm -f *.o *~
	rm -f ${LIBGOMP_PATH}/*.o
	rm -f ${SUPPORT_PATH}/*.o
	rm -rf o-optimize
	rm -f TargetMem* stats*

disassemble: $(EXE)
	$(OBJDUMP) -SD linked.o > linked.dis

