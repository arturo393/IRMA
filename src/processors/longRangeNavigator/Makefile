#############################################################################
#
#	Makefile for building the Long Range Navigator program
#
#############################################################################

PC = true
#PC = flase

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
#CROSS_COMPILE = /home/irmabot/gumstix/gumstix-oe/tmp/cross/arm-angstrom-linux-gnueabi/bin/
CROSS_COMPILE = /home/babbage/marbotRelease/src/processors/longRangeNavigator/arm/arm-angstrom-linux-gnueabi/bin/
COMMON			= ../../common/
SHARED			= Shared
I32 = /usr/include/x86_64-linux-gnu/c++/4.7/bits/


#ifeq ($(strip $(CROSS_COMPILE)),)
#	$(error Unable to detect C++ Cross Compiler)
#endif

vpath %.c  %.cpp $(COMMON) $(SHARED)

CPPFLAGS   +=  -I . -I $(COMMON) -I $(SHARED) -I$(I32)
CFLAGS 	   += -Wall -I$(I32)
LDFLAGS    += -m32 -ggdb
#CFLAGS     += `pkg-config --cflags mrpt-base mrpt-slam`
#DFLAGS    += `pkg-config --libs mrpt-base mrpt-slam`
 
ifeq ($(PC), true)
CPPFLAGS += -DPC -m32 -ggdb
CFLAGS   += -DPC -m32 -ggdb
CC = g++
CXX = g++

OBJS = longRangeNavigator.o GALRN.o InternalMap.o MandamiFuzzyModel.o MembershipFunctions_1D.o Utils.o ../../common/configFile/configFile.o	

else				# else
TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale
CC = $(CROSS_COMPILE)g++
CXX = $(CROSS_COMPILE)g++

OBJS = \
   longRangeNavigator.o			\
	GALRN.o							\
	InternalMap.o					\
	MandamiFuzzyModel.o			\
	MembershipFunctions_1D.o	\
	Utils.o							\
	../../common/configFile/configFile.o
	

endif				# endif 

all: longRangeNavigator
	
install:
	cp ./longRangeNavigator ../../../bin

longRangeNavigator: $(OBJS)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions longRangeNavigator

depend .depend dep:
	@echo "Creating dependencies ..."
	$(CXX) $(CFLAGS) $(CPPFLAGS) -M  *.cpp  > .depend

FORCE:

.PHONY: FORCE

PREPROCESS.c = $(CXX) $(CPPFLAGS) $(TARGET_ARCH) -E -Wp,-C,-dD,-dI

%.pp : %.c  FORCE
	$(PREPROCESS.c) $< > $@

ifeq ($(strip $(filter clean, $(MAKECMDGOALS))),)
-include .depend
endif
