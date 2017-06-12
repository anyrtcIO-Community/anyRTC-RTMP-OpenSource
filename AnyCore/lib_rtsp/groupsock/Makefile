INCLUDES = -Iinclude -I../UsageEnvironment/include
PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib
##### Change the following for your environment:
#-------------------------------------------------------------------------------------------------
#
# 	Create BY zhang zhiwei 2014.07.03
# 	sunfrank2012@gmail.com
#
#-------------------------------------------------------------------------------------------------
# NDK ROOT
ANDROID_NDK_ROOT=/cygdrive/c/Android/NDK/android-ndk-r8e/
# For sysroot | arm-linux-androideabi-gcc can't find cygwin path
NDK_ROOT=c:/Android/NDK/android-ndk-r8e/
# For x86
PREBUILT=$(ANDROID_NDK_ROOT)/toolchains/arm-linux-androideabi-4.6/prebuilt/windows
# Fro x86_64
#PREBUILT=$(ANDROID_NDK_ROOT)/toolchains/arm-linux-androideabi-4.6/prebuilt/windows-x86_64
PLATFORM=android-14

CROSS_COMPILE=		$(PREBUILT)/bin/arm-linux-androideabi-
COMPILE_OPTS =		$(INCLUDES) -fPIC -DANDROID -std=c99 -mfpu=neon -mfloat-abi=softfp -I. -O2 -DXLOCALE_NOT_USED -DANDROID -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DNULL=0 --sysroot=${NDK_ROOT}/platforms/${PLATFORM}/arch-arm/  -I${ANDROID_NDK_ROOT}/platforms/${PLATFORM}/arch-arm/usr/include
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1 -fexceptions
OBJ =			o
LINK =			$(CROSS_COMPILE)g++ -o
LINK_OPTS = 	
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =	$(LINK_OPTS)
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION =-lc -lm -lz -L./ -lsupc++	-fexceptions -L${ANDROID_NDK_ROOT}/platforms/${PLATFORM}/arch-arm/usr/lib -lstdc++ 
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

NAME = libgroupsock
ALL = $(NAME).$(LIB_SUFFIX)
all:	$(ALL)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

GROUPSOCK_LIB_OBJS = GroupsockHelper.$(OBJ) GroupEId.$(OBJ) inet.$(OBJ) Groupsock.$(OBJ) NetInterface.$(OBJ) NetAddress.$(OBJ) IOHandlers.$(OBJ)

GroupsockHelper.$(CPP):	include/GroupsockHelper.hh
include/GroupsockHelper.hh:	include/NetAddress.hh
include/NetAddress.hh:	include/NetCommon.h
GroupEId.$(CPP):	include/GroupEId.hh
include/GroupEId.hh:	include/NetAddress.hh
inet.$(C):		include/NetCommon.h
Groupsock.$(CPP):	include/Groupsock.hh include/GroupsockHelper.hh include/TunnelEncaps.hh
include/Groupsock.hh:	include/groupsock_version.hh include/NetInterface.hh include/GroupEId.hh
include/NetInterface.hh:	include/NetAddress.hh
include/TunnelEncaps.hh:	include/NetAddress.hh
NetInterface.$(CPP):	include/NetInterface.hh include/GroupsockHelper.hh
NetAddress.$(CPP):	include/NetAddress.hh include/GroupsockHelper.hh
IOHandlers.$(CPP):	include/IOHandlers.hh include/TunnelEncaps.hh

libgroupsock.$(LIB_SUFFIX): $(GROUPSOCK_LIB_OBJS) \
    $(PLATFORM_SPECIFIC_LIB_OBJS)
	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) \
		$(GROUPSOCK_LIB_OBJS)

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

install: install1 $(INSTALL2)
install1: libgroupsock.$(LIB_SUFFIX)
	  install -d $(DESTDIR)$(PREFIX)/include/groupsock $(DESTDIR)$(LIBDIR)
	  install -m 644 include/*.hh include/*.h $(DESTDIR)$(PREFIX)/include/groupsock
	  install -m 644 libgroupsock.$(LIB_SUFFIX) $(DESTDIR)$(LIBDIR)
install_shared_libraries: libgroupsock.$(LIB_SUFFIX)
	  ln -s libgroupsock.$(LIB_SUFFIX) $(DESTDIR)$(LIBDIR)/libgroupsock.$(SHORT_LIB_SUFFIX)
	  ln -s libgroupsock.$(LIB_SUFFIX) $(DESTDIR)$(LIBDIR)/libgroupsock.so

##### Any additional, platform-specific rules come here:
