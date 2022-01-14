TARGET = faad
C++ = g++
CC = gcc
AR := ar
RANLIB := ranlib
INCLUDE_FLAG = -include
INCPATH = -I./ -I./include -I./libfaad
CCFLAGS += -Wall -fPIC -D_GNU_SOURCE -fvisibility=hidden -DNDEBUG -O2 -ffast-math -fprefetch-loop-arrays -DFAAD_HAVE_CONFIG_H

C++FLAGS = $(CCFLAGS)

CFILES = ./libfaad/bits.c \
		./libfaad/cfft.c \
		./libfaad/decoder.c \
		./libfaad/drc.c \
		./libfaad/drm_dec.c \
		./libfaad/error.c \
		./libfaad/filtbank.c \
		./libfaad/ic_predict.c \
		./libfaad/is.c \
		./libfaad/lt_predict.c \
		./libfaad/mdct.c \
		./libfaad/mp4.c \
		./libfaad/ms.c \
		./libfaad/output.c \
		./libfaad/pns.c \
		./libfaad/ps_dec.c \
		./libfaad/ps_syntax.c \
		./libfaad/pulse.c \
		./libfaad/specrec.c \
		./libfaad/syntax.c \
		./libfaad/tns.c \
		./libfaad/hcr.c \
		./libfaad/huffman.c \
		./libfaad/rvlc.c \
		./libfaad/ssr.c \
		./libfaad/ssr_fb.c \
		./libfaad/ssr_ipqf.c \
		./libfaad/common.c \
		./libfaad/sbr_dct.c \
		./libfaad/sbr_e_nf.c \
		./libfaad/sbr_fbt.c \
		./libfaad/sbr_hfadj.c \
		./libfaad/sbr_hfgen.c \
		./libfaad/sbr_huff.c \
		./libfaad/sbr_qmf.c \
		./libfaad/sbr_syntax.c \
		./libfaad/sbr_tf_grid.c \
		./libfaad/sbr_dec.c

CCFILES = 

CPPFILES = 

all: libfaad.a

libfaad.a: $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)
	$(AR)  -r libfaad.a $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)
	$(RANLIB) libfaad.a

install: 
	install -m 664 libfaad.a ../../out/Linux

clean:
	rm -f libfaad.a $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp $(INCPATH)

.cc.o:
	$(CC) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc $(INCPATH)

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c $(INCPATH)



