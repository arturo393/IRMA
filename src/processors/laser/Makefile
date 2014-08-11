#############################################################################
#
#	Makefile for building the laser program
#
#############################################################################

PC = true
#PC = flase

#ifeq ($(DEBUG),y)
#	CFLAGS += -O -g		# -O is need to expand inlines
#else
#	CFLAGS += -O2
#endif

# Dependencias del laser
#CPP
top_builddir = urg-0.8.18
top_srcdir = urg-0.8.18
OBJEXT = o		#OK
SHELL = /bin/bash	#OK
CXX = g++
CPPFLAGS = 
LDFLAGS = 
DEFS = -DHAVE_CONFIG_H
SDL_CFLAGS = -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
CXXFLAGS = -g -O2 -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT 
am__mv = mv -f
DEPDIR = .deps
AM_CXXFLAGS = $(SDL_CFLAGS)
CPP_INCLUDES = -I${top_srcdir}/include/cpp  -I${top_srcdir}/src/cpp/system -I${top_srcdir}/src/cpp/common -I${top_srcdir}/src/cpp/connection -I${top_srcdir}/src/cpp/geometry -I${top_srcdir}/src/cpp/coordinate -I${top_srcdir}/src/cpp/urg -I${top_srcdir}/src/cpp/connection/sdl -I${top_srcdir}/src/cpp/monitor
INCLUDES = $(CPP_INCLUDES)
DEFAULT_INCLUDES = -I. -I$(top_builddir)
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
SDL_CFLAGS = -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT   #OK
CXXFLAGS = -g -O2 -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT #OK
INSTALL_LIBS = -lurg_monitor -lurg -lurg_coordinate -lurg_geometry -lurg_connection -lurg_connection_sdl -lurg_common -lurg_system -lm	#OK
SAMPLES_LIBS_PATH = -L${top_builddir}/src/c/urg -L${top_builddir}/src/c/connection -L${top_builddir}/src/c/system -L${top_builddir}/src/cpp/urg -L${top_builddir}/src/cpp/connection -L${top_builddir}/src/cpp/common -L${top_builddir}/src/cpp/system -L${top_builddir}/src/cpp/monitor -L${top_builddir}/src/cpp/coordinate -L${top_builddir}/src/cpp/geometry -L${top_builddir}/src/cpp/connection/sdl #OK
LIBS = $(SAMPLES_LIBS_PATH) $(INSTALL_LIBS) #OK
gdScan_LDADD = $(LDADD)			#OK
AM_CXXFLAGS = $(SDL_CFLAGS)		#OK
am_gdScan_OBJECTS = laser.$(OBJEXT) 	#OK
gdScan_OBJECTS = $(am_gdScan_OBJECTS)	#OK
LIBTOOL = $(SHELL) $(top_builddir)/libtool  #OK	
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --tag=CXX \
	--mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) \
	$(LDFLAGS)-o $@

#SAMPLES_LIBS_PATH = -L${top_builddir}/src/c/urg -L${top_builddir}/src/c/connection -L${top_builddir}/src/c/system -L${top_builddir}/src/cpp/urg -L${top_builddir}/src/cpp/connection -L${top_builddir}/src/cpp/common -L${top_builddir}/src/cpp/system -L${top_builddir}/src/cpp/monitor -L${top_builddir}/src/cpp/coordinate -L${top_builddir}/src/cpp/geometry -L${top_builddir}/src/cpp/connection/sdl
#SAMPLES_LIBS_PATH_SO = ${top_builddir}/src/c/urg/.libs/libc_urg.so -L/usr/lib/i386-linux-gnu ${top_builddir}/src/c/connection/.libs/libc_urg_connection.so ${top_builddir}/src/c/system/.libs/libc_urg_system.so
#C_INSTALL_LIBS = -lSDL -lSDL_net -lm

#CC2 = gcc
#CCLD = $(CC2)
#CFLAGS = -g -O2
#LIBTOOL = $(SHELL) $(top_builddir)/libtool
#LIBS = $(SAMPLES_LIBS_PATH) $(C_INSTALL_LIBS)
#LINK = $(LIBTOOL) --tag=CC  \
	--mode=link $(CCLD) $(CFLAGS) \
	-o gd_scan
#fin dependencias del laser

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
PSM_DIR = polar_matching/polar_matching
vpath %.c  %.cpp $(COMMON) $(SHARED)

CPPFLAGS += -I . -I $(COMMON) -I $(SHARED) 
CFLAGS 	 += -Wall 


ifeq ($(PC), true)
CPPFLAGS += -DPC -m32 -ggdb
CFLAGS   += -DPC -m32 -ggdb
CC = g++
CXX = g++

OBJS = \
	../../common/configFile/configFile.o		\
	ekf.o \
	calculos.o \
	laser.o 			\
	view.o \
	matlabfile.o

else				# else
TARGET_ARCH=-Os -march=armv5te -mtune=xscale -Wa,-mcpu=xscale
CC = $(CROSS_COMPILE)g++
CXX = $(CROSS_COMPILE)g++

OBJS = \
	../../common/configFile/configFile.o		\
	ekf.o 	\
	calculos.o \
	view.o\
	matlabfile.o


endif				# endif
#laserprocess.o:
#	g++ -DHAVE_CONFIG_H -I. -I../.. -I../../include/cpp  -I../../src/cpp/system -I../../src/cpp/common -I../../src/cpp/connection -I../../src/cpp/geometry -I../../src/cpp/coordinate -I../../src/cpp/urg -I../../src/cpp/connection/sdl -I../../src/cpp/monitor   -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -g -O2 -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -MT gdScan.o -MD -MP -MF .deps/gdScan.Tpo -c -o gdScan.o gdScan.cpp
#	mv -f .deps/gdScan.Tpo .deps/gdScan.Po

all: laser
	
PSM_LIB	=	psm
PSM_LINK   =  -lstdc++ -lX11 -lm -L/usr/lib -L/lib -l$(PSM_LIB) -lrt -L. # -pg #uncomment -pg for profiling.
PSM_OBJ_LIB    =  $(PSM_DIR)/draw.o $(PSM_DIR)/polar_match.o 
PSM_C_OPTS =  -c -g -Wall -I$(PSM_DIR) #-O2 # -pg #if not debugging uncomment the optimization switch -O2
CIMG_LINK =   -lpthread -L/usr/X11R6/lib
lib$(PSM_LIB).a : $(PSM_OBJ_LIB) 
	ar r lib$(PSM_LIB).a $(PSM_OBJ_LIB)	

$(PSM_DIR)/draw.o:	
	cc -c -g -Wall -lrt -I$(PSM_DIR) $(PSM_DIR)/draw.c -o $(PSM_DIR)/draw.o
	
$(PSM_DIR)/polar_match.o :  $(PSM_DIR)/polar_match.cpp $(PSM_DIR)/polar_match.h $(PSM_DIR)/draw.o
	gcc -c -g -Wall -lrt -I$(PSM_DIR) $(PSM_DIR)/polar_match.cpp  -o $(PSM_DIR)/polar_match.o

laser.o: laser.cpp $(PSM_DIR)/draw.h $(PSM_DIR)/polar_match.h CImg.h calculos.h ekf.h view.h matlabfile.h
	gcc -c -g -Wall -I$(PSM_DIR) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -o $@ $< 
	$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

#laser: $(OBJS)
	
laser: $(gdScan_OBJECTS) lib$(PSM_LIB).a ekf.o view.o calculos.o matlabfile.o
	@rm -f laserprocess
	$(CXXLINK) $(gdScan_OBJECTS) ekf.o view.o calculos.o $(gdScan_LDADD) $(LIBS) $(PSM_LINK) $(CIMG_LINK) 

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions $(PSM_DIR)/*.o

depend .depend dep: 
	@echo "Creating dependencies ..."
	$(CXX) $(CFLAGS) $(CPPFLAGS) -M *.c *.cpp $(COMMON)/*.c source/*.cpp > .depend

FORCE:

.PHONY: FORCE

PREPROCESS.c = $(CXX) $(CPPFLAGS) $(TARGET_ARCH) -E -Wp,-C,-dD,-dI

%.pp : %.c  FORCE
	$(PREPROCESS.c) $< > $@

ifeq ($(strip $(filter clean, $(MAKECMDGOALS))),)
-include .depend
endif

