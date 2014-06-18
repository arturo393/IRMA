#############################################################################
#
#	Makefile for building the executive program
#
#############################################################################

PC = true
# PC = false

#verdadero indica compilar para msp
MSP = true
#falso indica compilar para avr
#MSP = false

#ifeq ($(DEBUG),y)
#	CFLAGS += -O -g		# -O is need to expand inlines
#else
#	CFLAGS += -O2
#endif

PWD       := $(shell pwd)

#GUMSTIX_BUILDROOT   = /home/irmabot/gumstix-buildroot
#BUILD_ARM		= $(GUMSTIX_BUILDROOT)/build_arm_nofpu
#CROSS_COMPILE		= $(BUILD_ARM)/staging_dir/bin/arm-linux-
BUILD_ARM           = $(wildcard $(GUMSTIX_BUILDROOT)/build_arm*)
CROSS_COMPILE       = $(patsubst %g++, %, $(wildcard $(BUILD_ARM)/staging_dir/bin/arm-linux-uclibc*-g++))
# CROSS_COMPILE = /home/irmabot/gumstix/gumstix-oe/tmp/cross/arm-angstrom-linux-gnueabi/bin/
COMMON			= Common
SHARED			= Shared

#ifeq ($(strip $(CROSS_COMPILE)),)
#	$(error Unable to detect C++ Cross Compiler)
#endif

vpath %.c  %.cpp $(COMMON) $(SHARED)

CPPFLAGS +=  -I . -I $(COMMON) -I $(SHARED) -I$(I32)
CFLAGS 	 += -Wall -I$(I32)
LDFLAGS += -m32 -ggdb
ifeq ($(PC), true)
CPPFLAGS += -DPC -m32 -ggdb
CFLAGS   += -DPC -m32 -ggdb


CC = g++ -g  
CXX = g++ -g
ifeq ($(MSP), true) 
CPPFLAGS += -DMSP_
CFLAGS   += -DMSP_
CPP =		msp.cpp \
		uart-io.cpp \
		driverSerial.cpp \
		clientdummy.cpp
OBJS = \
        executive.o 							\
		../../common/configFile/configFile.o			\
		msp.o							\
		Log.o							\
		uart-io.o \
		driverSerial.o \
		clientdummy.o
		
else
OBJS = \
        executive.o 							\
		../../common/configFile/configFile.o			\
		robostix.o						\
		Log.o							\
		i2c-io.o 		\
		i2c-api.o		\
		i2c-io-api.o		\
		AvrInfo.o		\
		BootLoader-api.o	\
		Crc8.o			\
		DumpMem.o		\
		driverSerial.o

endif		
	
else				# else
TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale
CC = $(CROSS_COMPILE)g++
CXX = $(CROSS_COMPILE)g++

OBJS = \
        executive.o 			\
		../../common/configFile/configFile.o			\
		robostix.o 		\
		i2c-io.o 		\
		i2c-api.o		\
		i2c-io-api.o		\
		AvrInfo.o		\
		BootLoader-api.o	\
		Crc8.o			\
		DumpMem.o		\
		Log.o

endif				# endif 

all: executive

executive: svn-version.h $(OBJS)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions executive
#gd_scan.o: 
#	gcc -DHAVE_CONFIG_H $(SAMPLES_LIBS_PATH) -g -O2 -MT gd_scan.o -MD -MP -MF .deps/gd_scan.Tpo -c -o gd_scan.o gd_scan.c
#	mv -f .deps/gd_scan.Tpo .deps/gd_scan.Po
#gd_scan:
#	@rm -f gd_scan$(EXEEXT)
#	$(LINK) gd_scan.o $(SAMPLES_LIBS_PATH_SO) $(LIBS) 
	
	
depend .depend dep: svn-version.h 
	@echo "Creating dependencies ..."
	$(CXX) $(CFLAGS) $(CPPFLAGS)  -M $(CPP) $(COMMON)/*.c   > .depend

FORCE:

.PHONY: FORCE

PREPROCESS.c = $(CXX) $(CPPFLAGS) $(TARGET_ARCH)  -E -Wp,-C,-dD,-dI

%.pp : %.c  FORCE
	$(PREPROCESS.c) $< > $@

include svn-version.mk

ifeq ($(strip $(filter clean, $(MAKECMDGOALS))),)
-include .depend
endif

