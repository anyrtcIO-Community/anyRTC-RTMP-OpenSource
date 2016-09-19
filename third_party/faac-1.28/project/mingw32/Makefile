# Makefile for project faac


# Edit this path to let it point to the location of the source on your HDD
SRCDIR=e:\faac

# Edit this path to let it point to the location of your MinGW (or Dev-C++) installation
CCDIR=c:\MinGW

# No editing needed after this point

CFLAGS=-mconsole -O3 -DWIN32 -I$(CCDIR)\include -I$(SRCDIR)\include -I$(SRCDIR)\common\getopt -I$(SRCDIR)\common\libsndfile\win32 -I$(SRCDIR)\common\libsndfile\src 
CC=$(CCDIR)\bin\gcc.exe
LINKER=$(CCDIR)\bin\gcc.exe
OBJS=\
	getopt.o \
	aiff.o \
	alaw.o \
	au.o \
	au_g72x.o \
	common.o \
	g721.o \
	g723_16.o \
	g723_24.o \
	g723_40.o \
	g72x.o \
	add.o \
	code.o \
	decode.o \
	gsm_create.o \
	gsm_decode.o \
	gsm_destroy.o \
	gsm_encode.o \
	gsm_option.o \
	long_term.o \
	lpc.o \
	preprocess.o \
	rpe.o \
	short_term.o \
	table.o \
	newpcm.o \
	paf.o \
	pcm.o \
	raw.o \
	sndfile.o \
	svx.o \
	ulaw.o \
	wav.o \
	wav_float.o \
	wav_gsm610.o \
	wav_ima_adpcm.o \
	wav_ms_adpcm.o \
	main.o \
	aacquant.o \
	backpred.o \
	bitstream.o \
	channels.o \
	fft.o \
	filtbank.o \
	frame.o \
	huffman.o \
	joint.o \
	ltp.o \
	psych.o \
	tns.o \
	util.o

LIBS=

faac.exe:	$(OBJS) Makefile
	$(LINKER) -s -o $(SRCDIR)\project\faac.exe $(OBJS) $(LIBS)

# Build GETOPT.C
GETOPT_C=\
	$(SRCDIR)\common\getopt\getopt.h\

getopt.o: $(GETOPT_C) $(SRCDIR)\common\getopt\getopt.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\getopt\getopt.c

# Build AIFF.C
AIFF_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\

aiff.o: $(AIFF_C) $(SRCDIR)\common\libsndfile\src\aiff.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\aiff.c

# Build ALAW.C
ALAW_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

alaw.o: $(ALAW_C) $(SRCDIR)\common\libsndfile\src\alaw.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\alaw.c

# Build AU.C
AU_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\
	$(SRCDIR)\common\libsndfile\src\au.h\
	$(SRCDIR)\common\libsndfile\src\ulaw.h\
	$(SRCDIR)\common\libsndfile\src\alaw.h\

au.o: $(AU_C) $(SRCDIR)\common\libsndfile\src\au.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\au.c

# Build AU_G72X.C
AU_G72X_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\au.h\
	$(SRCDIR)\common\libsndfile\src\G72x\g72x.h\

au_g72x.o: $(AU_G72X_C) $(SRCDIR)\common\libsndfile\src\au_g72x.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\au_g72x.c

# Build COMMON.C
COMMON_C=\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

common.o: $(COMMON_C) $(SRCDIR)\common\libsndfile\src\common.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\common.c

# Build G721.C
G721_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g721.o: $(G721_C) $(SRCDIR)\common\libsndfile\src\g72x\g721.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g721.c

# Build G723_16.C
G723_16_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g723_16.o: $(G723_16_C) $(SRCDIR)\common\libsndfile\src\g72x\g723_16.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g723_16.c

# Build G723_24.C
G723_24_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g723_24.o: $(G723_24_C) $(SRCDIR)\common\libsndfile\src\g72x\g723_24.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g723_24.c

# Build G723_40.C
G723_40_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g723_40.o: $(G723_40_C) $(SRCDIR)\common\libsndfile\src\g72x\g723_40.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g723_40.c

# Build G72X.C
G72X_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g72x.o: $(G72X_C) $(SRCDIR)\common\libsndfile\src\g72x\g72x.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g72x.c

# Build ADD.C
ADD_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

add.o: $(ADD_C) $(SRCDIR)\common\libsndfile\src\gsm610\add.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\add.c

# Build CODE.C
CODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\config.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

code.o: $(CODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\code.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\code.c

# Build DECODE.C
DECODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

decode.o: $(DECODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\decode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\decode.c

# Build GSM_CREATE.C
GSM_CREATE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\config.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_create.o: $(GSM_CREATE_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_create.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_create.c

# Build GSM_DECODE.C
GSM_DECODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_decode.o: $(GSM_DECODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_decode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_decode.c

# Build GSM_DESTROY.C
GSM_DESTROY_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\config.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_destroy.o: $(GSM_DESTROY_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_destroy.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_destroy.c

# Build GSM_ENCODE.C
GSM_ENCODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_encode.o: $(GSM_ENCODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_encode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_encode.c

# Build GSM_OPTION.C
GSM_OPTION_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_option.o: $(GSM_OPTION_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_option.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_option.c

# Build LONG_TERM.C
LONG_TERM_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

long_term.o: $(LONG_TERM_C) $(SRCDIR)\common\libsndfile\src\gsm610\long_term.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\long_term.c

# Build LPC.C
LPC_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

lpc.o: $(LPC_C) $(SRCDIR)\common\libsndfile\src\gsm610\lpc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\lpc.c

# Build PREPROCESS.C
PREPROCESS_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

preprocess.o: $(PREPROCESS_C) $(SRCDIR)\common\libsndfile\src\gsm610\preprocess.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\preprocess.c

# Build RPE.C
RPE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

rpe.o: $(RPE_C) $(SRCDIR)\common\libsndfile\src\gsm610\rpe.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\rpe.c

# Build SHORT_TERM.C
SHORT_TERM_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

short_term.o: $(SHORT_TERM_C) $(SRCDIR)\common\libsndfile\src\gsm610\short_term.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\short_term.c

# Build TABLE.C
TABLE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\

table.o: $(TABLE_C) $(SRCDIR)\common\libsndfile\src\gsm610\table.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\table.c

# Build NEWPCM.C
NEWPCM_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\

newpcm.o: $(NEWPCM_C) $(SRCDIR)\common\libsndfile\src\newpcm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\newpcm.c

# Build PAF.C
PAF_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\

paf.o: $(PAF_C) $(SRCDIR)\common\libsndfile\src\paf.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\paf.c

# Build PCM.C
PCM_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\

pcm.o: $(PCM_C) $(SRCDIR)\common\libsndfile\src\pcm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\pcm.c

# Build RAW.C
RAW_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\

raw.o: $(RAW_C) $(SRCDIR)\common\libsndfile\src\raw.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\raw.c

# Build SNDFILE.C
SNDFILE_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

sndfile.o: $(SNDFILE_C) $(SRCDIR)\common\libsndfile\src\sndfile.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\sndfile.c

# Build SVX.C
SVX_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\

svx.o: $(SVX_C) $(SRCDIR)\common\libsndfile\src\svx.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\svx.c

# Build ULAW.C
ULAW_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

ulaw.o: $(ULAW_C) $(SRCDIR)\common\libsndfile\src\ulaw.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\ulaw.c

# Build WAV.C
WAV_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\pcm.h\
	$(SRCDIR)\common\libsndfile\src\ulaw.h\
	$(SRCDIR)\common\libsndfile\src\alaw.h\
	$(SRCDIR)\common\libsndfile\src\wav.h\

wav.o: $(WAV_C) $(SRCDIR)\common\libsndfile\src\wav.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\wav.c

# Build WAV_FLOAT.C
WAV_FLOAT_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\wav.h\

wav_float.o: $(WAV_FLOAT_C) $(SRCDIR)\common\libsndfile\src\wav_float.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\wav_float.c

# Build WAV_GSM610.C
WAV_GSM610_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\wav.h\
	$(SRCDIR)\common\libsndfile\src\GSM610\gsm.h\

wav_gsm610.o: $(WAV_GSM610_C) $(SRCDIR)\common\libsndfile\src\wav_gsm610.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\wav_gsm610.c

# Build WAV_IMA_ADPCM.C
WAV_IMA_ADPCM_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\wav.h\

wav_ima_adpcm.o: $(WAV_IMA_ADPCM_C) $(SRCDIR)\common\libsndfile\src\wav_ima_adpcm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\wav_ima_adpcm.c

# Build WAV_MS_ADPCM.C
WAV_MS_ADPCM_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\
	$(SRCDIR)\common\libsndfile\src\wav.h\

wav_ms_adpcm.o: $(WAV_MS_ADPCM_C) $(SRCDIR)\common\libsndfile\src\wav_ms_adpcm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\wav_ms_adpcm.c

# Build MAIN.C
MAIN_C=\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\getopt\getopt.h\
	$(SRCDIR)\include\faac.h\

main.o: $(MAIN_C) $(SRCDIR)\frontend\main.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\frontend\main.c

# Build AACQUANT.C
AACQUANT_C=\
	$(SRCDIR)\libfaac\aacquant.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\huffman.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\util.h\

aacquant.o: $(AACQUANT_C) $(SRCDIR)\libfaac\aacquant.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\aacquant.c

# Build BACKPRED.C
BACKPRED_C=\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\backpred.h\

backpred.o: $(BACKPRED_C) $(SRCDIR)\libfaac\backpred.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\backpred.c

# Build BITSTREAM.C
BITSTREAM_C=\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\huffman.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\ltp.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\util.h\

bitstream.o: $(BITSTREAM_C) $(SRCDIR)\libfaac\bitstream.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\bitstream.c

# Build CHANNELS.C
CHANNELS_C=\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\util.h\

channels.o: $(CHANNELS_C) $(SRCDIR)\libfaac\channels.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\channels.c

# Build FFT.C
FFT_C=\
	$(SRCDIR)\libfaac\fft.h\
	$(SRCDIR)\libfaac\util.h\

fft.o: $(FFT_C) $(SRCDIR)\libfaac\fft.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\fft.c

# Build FILTBANK.C
FILTBANK_C=\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\filtbank.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\fft.h\
	$(SRCDIR)\libfaac\util.h\

filtbank.o: $(FILTBANK_C) $(SRCDIR)\libfaac\filtbank.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\filtbank.c

# Build FRAME.C
FRAME_C=\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\joint.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\filtbank.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\aacquant.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\util.h\
	$(SRCDIR)\libfaac\huffman.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\tns.h\
	$(SRCDIR)\libfaac\ltp.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\backpred.h\

frame.o: $(FRAME_C) $(SRCDIR)\libfaac\frame.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\frame.c

# Build HUFFMAN.C
HUFFMAN_C=\
	$(SRCDIR)\libfaac\huffman.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\util.h\
	$(SRCDIR)\libfaac\hufftab.h\

huffman.o: $(HUFFMAN_C) $(SRCDIR)\libfaac\huffman.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\huffman.c

# Build JOINT.C
JOINT_C=\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\util.h\

joint.o: $(JOINT_C) $(SRCDIR)\libfaac\joint.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\joint.c

# Build LTP.C
LTP_C=\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\ltp.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\tns.h\
	$(SRCDIR)\libfaac\filtbank.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\util.h\

ltp.o: $(LTP_C) $(SRCDIR)\libfaac\ltp.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\ltp.c

# Build PSYCH.C
PSYCH_C=\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\fft.h\
	$(SRCDIR)\libfaac\util.h\

psych.o: $(PSYCH_C) $(SRCDIR)\libfaac\psych.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\psych.c

# Build TNS.C
TNS_C=\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\psych.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\bitstream.h\
	$(SRCDIR)\libfaac\frame.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\tns.h\
	$(SRCDIR)\libfaac\util.h\

tns.o: $(TNS_C) $(SRCDIR)\libfaac\tns.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\tns.c

# Build UTIL.C
UTIL_C=\
	$(SRCDIR)\libfaac\util.h\

util.o: $(UTIL_C) $(SRCDIR)\libfaac\util.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\util.c

link:
	$(LINKER) -s -o $(SRCDIR)\project\faac.exe $(OBJS) $(LIBS)

