# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = yuv
C++ := g++
CC := gcc
AR := ar
RANLIB := ranlib
INCLUDE_FLAG = -include
INCPATH = -I./ -I./include
LINK = $(LINKER)
CCFLAGS += -Wall -fPIC -D_GNU_SOURCE -fvisibility=hidden -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS
CCFLAGS += $(INCPATH)

C++FLAGS = $(CCFLAGS)

YUV_FILES = ./source/compare.cc \
      ./source/compare_common.cc \
      ./source/compare_gcc.cc \
      ./source/compare_win.cc \
      ./source/convert.cc \
      ./source/convert_argb.cc \
      ./source/convert_from.cc \
      ./source/convert_from_argb.cc \
      ./source/convert_jpeg.cc \
      ./source/convert_to_argb.cc \
      ./source/convert_to_i420.cc \
      ./source/cpu_id.cc \
      ./source/mjpeg_decoder.cc \
      ./source/mjpeg_validate.cc \
      ./source/planar_functions.cc \
      ./source/rotate.cc \
      ./source/rotate_any.cc \
      ./source/rotate_common.cc \
      ./source/rotate_gcc.cc \
      ./source/rotate_argb.cc \
      ./source/rotate_mips.cc \
      ./source/rotate_win.cc \
      ./source/row_any.cc \
      ./source/row_common.cc \
      ./source/row_mips.cc \
      ./source/row_gcc.cc \
      ./source/row_win.cc \
      ./source/scale.cc \
      ./source/scale_argb.cc \
      ./source/scale_any.cc \
      ./source/scale_common.cc \
      ./source/scale_mips.cc \
      ./source/scale_gcc.cc \
      ./source/scale_win.cc \
      ./source/video_common.cc \

all: libyuv.a 

libyuv.a: $(YUV_FILES:.cc=.o) 
	$(AR)  -r libyuv.a $(YUV_FILES:.cc=.o)
	$(RANLIB) libyuv.a
	

install: 
	install -m 664 libyuv.a ../../out/Linux
	
clean:
	rm -f libyuv.a $(YUV_FILES:.cc=.o)

.SUFFIXES: .cc .cpp .c .o

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc
	
.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c
