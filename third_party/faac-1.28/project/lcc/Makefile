# Makefile for project faac


# Edit this path to let it point to the location of the source on your HDD
SRCDIR=d:\aac\cvsrep\faac

# Edit this path to let it point to the location of your LCC installation
LCCDIR=c:\lcc

# No editing needed after this point

CFLAGS=-I$(LCCDIR)\include -I$(SRCDIR)\include -I$(SRCDIR)\common\getopt -I$(SRCDIR)\common\libsndfile\win32 -I$(SRCDIR)\common\libsndfile\src  -O 
CC=$(LCCDIR)\bin\lcc.exe
LINKER=$(LCCDIR)\bin\lcclnk.exe
OBJS=\
	getopt.obj \
	aiff.obj \
	alaw.obj \
	au.obj \
	au_g72x.obj \
	common.obj \
	g721.obj \
	g723_16.obj \
	g723_24.obj \
	g723_40.obj \
	g72x.obj \
	add.obj \
	code.obj \
	decode.obj \
	gsm_create.obj \
	gsm_decode.obj \
	gsm_destroy.obj \
	gsm_encode.obj \
	gsm_option.obj \
	long_term.obj \
	lpc.obj \
	preprocess.obj \
	rpe.obj \
	short_term.obj \
	table.obj \
	newpcm.obj \
	paf.obj \
	pcm.obj \
	raw.obj \
	sndfile.obj \
	svx.obj \
	ulaw.obj \
	wav.obj \
	wav_float.obj \
	wav_gsm610.obj \
	wav_ima_adpcm.obj \
	wav_ms_adpcm.obj \
	main.obj \
	aacquant.obj \
	backpred.obj \
	bitstream.obj \
	channels.obj \
	fft.obj \
	filtbank.obj \
	frame.obj \
	huffman.obj \
	joint.obj \
	ltp.obj \
	psych.obj \
	tns.obj \
	util.obj

LIBS=

faac.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\faac.exe $(OBJS) $(LIBS)

# Build GETOPT.C
GETOPT_C=\
	$(SRCDIR)\common\getopt\getopt.h\

getopt.obj: $(GETOPT_C) $(SRCDIR)\common\getopt\getopt.c
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

aiff.obj: $(AIFF_C) $(SRCDIR)\common\libsndfile\src\aiff.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\aiff.c

# Build ALAW.C
ALAW_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

alaw.obj: $(ALAW_C) $(SRCDIR)\common\libsndfile\src\alaw.c
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

au.obj: $(AU_C) $(SRCDIR)\common\libsndfile\src\au.c
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

au_g72x.obj: $(AU_G72X_C) $(SRCDIR)\common\libsndfile\src\au_g72x.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\au_g72x.c

# Build COMMON.C
COMMON_C=\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

common.obj: $(COMMON_C) $(SRCDIR)\common\libsndfile\src\common.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\common.c

# Build G721.C
G721_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g721.obj: $(G721_C) $(SRCDIR)\common\libsndfile\src\g72x\g721.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g721.c

# Build G723_16.C
G723_16_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g723_16.obj: $(G723_16_C) $(SRCDIR)\common\libsndfile\src\g72x\g723_16.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g723_16.c

# Build G723_24.C
G723_24_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g723_24.obj: $(G723_24_C) $(SRCDIR)\common\libsndfile\src\g72x\g723_24.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g723_24.c

# Build G723_40.C
G723_40_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g723_40.obj: $(G723_40_C) $(SRCDIR)\common\libsndfile\src\g72x\g723_40.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g723_40.c

# Build G72X.C
G72X_C=\
	$(SRCDIR)\common\libsndfile\src\g72x\g72x.h\
	$(SRCDIR)\common\libsndfile\src\g72x\private.h\

g72x.obj: $(G72X_C) $(SRCDIR)\common\libsndfile\src\g72x\g72x.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\g72x\g72x.c

# Build ADD.C
ADD_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

add.obj: $(ADD_C) $(SRCDIR)\common\libsndfile\src\gsm610\add.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\add.c

# Build CODE.C
CODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\config.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

code.obj: $(CODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\code.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\code.c

# Build DECODE.C
DECODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

decode.obj: $(DECODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\decode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\decode.c

# Build GSM_CREATE.C
GSM_CREATE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\config.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_create.obj: $(GSM_CREATE_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_create.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_create.c

# Build GSM_DECODE.C
GSM_DECODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_decode.obj: $(GSM_DECODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_decode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_decode.c

# Build GSM_DESTROY.C
GSM_DESTROY_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\config.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_destroy.obj: $(GSM_DESTROY_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_destroy.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_destroy.c

# Build GSM_ENCODE.C
GSM_ENCODE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_encode.obj: $(GSM_ENCODE_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_encode.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_encode.c

# Build GSM_OPTION.C
GSM_OPTION_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

gsm_option.obj: $(GSM_OPTION_C) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_option.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\gsm_option.c

# Build LONG_TERM.C
LONG_TERM_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

long_term.obj: $(LONG_TERM_C) $(SRCDIR)\common\libsndfile\src\gsm610\long_term.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\long_term.c

# Build LPC.C
LPC_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

lpc.obj: $(LPC_C) $(SRCDIR)\common\libsndfile\src\gsm610\lpc.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\lpc.c

# Build PREPROCESS.C
PREPROCESS_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

preprocess.obj: $(PREPROCESS_C) $(SRCDIR)\common\libsndfile\src\gsm610\preprocess.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\preprocess.c

# Build RPE.C
RPE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

rpe.obj: $(RPE_C) $(SRCDIR)\common\libsndfile\src\gsm610\rpe.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\rpe.c

# Build SHORT_TERM.C
SHORT_TERM_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\

short_term.obj: $(SHORT_TERM_C) $(SRCDIR)\common\libsndfile\src\gsm610\short_term.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\gsm610\short_term.c

# Build TABLE.C
TABLE_C=\
	$(SRCDIR)\common\libsndfile\src\gsm610\private.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\proto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\unproto.h\
	$(SRCDIR)\common\libsndfile\src\gsm610\gsm.h\

table.obj: $(TABLE_C) $(SRCDIR)\common\libsndfile\src\gsm610\table.c
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

newpcm.obj: $(NEWPCM_C) $(SRCDIR)\common\libsndfile\src\newpcm.c
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

paf.obj: $(PAF_C) $(SRCDIR)\common\libsndfile\src\paf.c
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

pcm.obj: $(PCM_C) $(SRCDIR)\common\libsndfile\src\pcm.c
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

raw.obj: $(RAW_C) $(SRCDIR)\common\libsndfile\src\raw.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\raw.c

# Build SNDFILE.C
SNDFILE_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\sfendian.h\
	$(SRCDIR)\common\libsndfile\win32\config.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

sndfile.obj: $(SNDFILE_C) $(SRCDIR)\common\libsndfile\src\sndfile.c
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

svx.obj: $(SVX_C) $(SRCDIR)\common\libsndfile\src\svx.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\svx.c

# Build ULAW.C
ULAW_C=\
	$(SRCDIR)\common\libsndfile\win32\unistd.h\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\libsndfile\src\common.h\

ulaw.obj: $(ULAW_C) $(SRCDIR)\common\libsndfile\src\ulaw.c
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

wav.obj: $(WAV_C) $(SRCDIR)\common\libsndfile\src\wav.c
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

wav_float.obj: $(WAV_FLOAT_C) $(SRCDIR)\common\libsndfile\src\wav_float.c
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

wav_gsm610.obj: $(WAV_GSM610_C) $(SRCDIR)\common\libsndfile\src\wav_gsm610.c
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

wav_ima_adpcm.obj: $(WAV_IMA_ADPCM_C) $(SRCDIR)\common\libsndfile\src\wav_ima_adpcm.c
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

wav_ms_adpcm.obj: $(WAV_MS_ADPCM_C) $(SRCDIR)\common\libsndfile\src\wav_ms_adpcm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\libsndfile\src\wav_ms_adpcm.c

# Build MAIN.C
MAIN_C=\
	$(SRCDIR)\common\libsndfile\src\sndfile.h\
	$(SRCDIR)\common\getopt\getopt.h\
	$(SRCDIR)\include\faac.h\

main.obj: $(MAIN_C) $(SRCDIR)\frontend\main.c
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

aacquant.obj: $(AACQUANT_C) $(SRCDIR)\libfaac\aacquant.c
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

backpred.obj: $(BACKPRED_C) $(SRCDIR)\libfaac\backpred.c
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

bitstream.obj: $(BITSTREAM_C) $(SRCDIR)\libfaac\bitstream.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\bitstream.c

# Build CHANNELS.C
CHANNELS_C=\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\util.h\

channels.obj: $(CHANNELS_C) $(SRCDIR)\libfaac\channels.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\channels.c

# Build FFT.C
FFT_C=\
	$(SRCDIR)\libfaac\fft.h\
	$(SRCDIR)\libfaac\util.h\

fft.obj: $(FFT_C) $(SRCDIR)\libfaac\fft.c
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

filtbank.obj: $(FILTBANK_C) $(SRCDIR)\libfaac\filtbank.c
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

frame.obj: $(FRAME_C) $(SRCDIR)\libfaac\frame.c
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

huffman.obj: $(HUFFMAN_C) $(SRCDIR)\libfaac\huffman.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\huffman.c

# Build JOINT.C
JOINT_C=\
	$(SRCDIR)\libfaac\channels.h\
	$(SRCDIR)\libfaac\coder.h\
	$(SRCDIR)\libfaac\util.h\

joint.obj: $(JOINT_C) $(SRCDIR)\libfaac\joint.c
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

ltp.obj: $(LTP_C) $(SRCDIR)\libfaac\ltp.c
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

psych.obj: $(PSYCH_C) $(SRCDIR)\libfaac\psych.c
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

tns.obj: $(TNS_C) $(SRCDIR)\libfaac\tns.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\tns.c

# Build UTIL.C
UTIL_C=\
	$(SRCDIR)\libfaac\util.h\

util.obj: $(UTIL_C) $(SRCDIR)\libfaac\util.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaac\util.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\faac.exe $(OBJS) $(LIBS)

