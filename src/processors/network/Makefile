#############################################################################
#
#	Makefile for building the Network program
#
#############################################################################

#PC = true
PC = false

#ifeq ($(DEBUG),y)
#	CFLAGS += -O -g		# -O is need to expand inlines
#else
#	CFLAGS += -O2
#endif

PWD       := $(shell pwd)

#GUMSTIX_BUILDROOT   = /home/irmabot/gumstix-buildroot
#BUILD_ARM		= $(GUMSTIX_BUILDROOT)/build_arm_nofpu
#CROSS_COMPILE		= $(BUILD_ARM)/staging_dir/bin/arm-linux-
#BUILD_ARM           = $(wildcard $(GUMSTIX_BUILDROOT)/build_arm*)
#CROSS_COMPILE       = $(patsubst %g++, %, $(wildcard $(BUILD_ARM)/staging_dir/bin/arm-linux-uclibc*-g++))
CROSS_COMPILE = /home/irmabot/gumstix/gumstix-oe/tmp/cross/arm-angstrom-linux-gnueabi/bin/
COMMON			= Common
SHARED			= Shared

#ifeq ($(strip $(CROSS_COMPILE)),)
#	$(error Unable to detect C++ Cross Compiler)
#endif

vpath %.c  %.cpp $(COMMON) $(SHARED)

CPPFLAGS += -I . -I $(COMMON) -I $(SHARED)
CFLAGS 	 += -Wall

ifeq ($(PC), true)
CPPFLAGS += -DPC
CFLAGS   += -DPC
CC = g++
CXX = g++

OBJS = \
        network.o 			\
	
else				# else
TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale
CC = $(CROSS_COMPILE)g++
CXX = $(CROSS_COMPILE)g++

OBJS = \
        network.o
	

endif				# endif 

all: network

mapper: $(OBJS)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions network

depend .depend dep: 
	@echo "Creating dependencies ..."
	$(CXX) $(CFLAGS) $(CPPFLAGS) -M *.c *.cpp $(COMMON)/*.c > .depend

FORCE:

.PHONY: FORCE

PREPROCESS.c = $(CXX) $(CPPFLAGS) $(TARGET_ARCH) -E -Wp,-C,-dD,-dI

%.pp : %.c  FORCE
	$(PREPROCESS.c) $< > $@

ifeq ($(strip $(filter clean, $(MAKECMDGOALS))),)
-include .depend
endif

