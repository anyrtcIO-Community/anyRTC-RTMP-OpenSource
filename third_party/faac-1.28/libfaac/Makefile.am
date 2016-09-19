lib_LTLIBRARIES = libfaac.la

main_SOURCES = aacquant.c bitstream.c fft.c frame.c midside.c psychkni.c util.c backpred.c channels.c filtbank.c huffman.c ltp.c tns.c
if USE_DRM
drm_SOURCES = kiss_fft/kiss_fftr.c kiss_fft/kiss_fft.c
endif
libfaac_la_SOURCES = $(main_SOURCES) $(drm_SOURCES)
libfaac_la_INCLUDES = aacquant.h channels.h filtbank.h hufftab.h psych.h backpred.h coder.h frame.h midside.h tns.h bitstream.h fft.h huffman.h ltp.h util.h
libfaac_la_LIBADD = -lm

INCLUDES = -I$(top_srcdir)/include


