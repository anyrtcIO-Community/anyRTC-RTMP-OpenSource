AC_PREREQ(2.50)
AC_INIT(FAAC, 1.28, faac-dev@lists.sourceforge.net)
AC_CONFIG_AUX_DIR(.)
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

AC_ARG_WITH( mp4v2,  [  --with-mp4v2            compile libmp4v2],WITHMP4V2=$withval, WITHMP4V2=yes)
AC_ARG_ENABLE( drm,  [  --enable-drm            Digital Radio Mondiale support], enable_drm=$enableval, enable_drm=no)

AC_DEFUN([MY_DEFINE], [ AC_DEFINE($1, 1, [define if needed]) ])

CFLAGS=${CFLAGS:-"-O2 -Wall"}

AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL

AC_CHECK_HEADERS(getopt.h)

AC_CHECK_TYPES(u_int32_t)
AC_CHECK_TYPES(u_int16_t)
AC_CHECK_TYPES(int32_t)
AC_CHECK_TYPES(int16_t)

AC_CHECK_DECL(strcasecmp, MY_DEFINE(HAVE_STRCASECMP))

AC_CHECK_LIB(gnugetopt, getopt_long)

AM_CONDITIONAL(WITH_MP4V2, false)

AC_CHECK_DECLS([MP4Create, MP4MetadataDelete],
               AC_CHECK_LIB(mp4v2, MP4MetadataDelete, external_mp4v2=yes,
                            external_mp4v2=no, -lstdc++),
               external_mp4v2=no, [#include <mp4.h>])

if test x$external_mp4v2 = xyes; then
  AC_MSG_NOTICE([*** Building with external mp4v2 ***])
else
  if test x$WITHMP4V2 = xyes; then
     AC_MSG_NOTICE([*** Building with internal mp4v2 ***])
     AM_CONDITIONAL(WITH_MP4V2, true)
     AC_CONFIG_LINKS(common/mp4v2/mpeg4ip_config.h:config.h)
     MY_DEFINE(HAVE_LIBMP4V2)
  else
     AC_MSG_NOTICE([*** Building WITHOUT mp4v2 ***])
  fi
fi

dnl Check for DRM mode
if test "x$enable_drm" = "xyes"; then
    AC_DEFINE(DRM, 1, [Define if you want to encode for DRM])
    AM_CONDITIONAL(USE_DRM, true)
      CFLAGS="$CFLAGS -DDRM"
else
    AM_CONDITIONAL(USE_DRM, false)
fi

AC_C_BIGENDIAN

dnl Checks for header files required for mp4.h
AC_HEADER_STDC
AC_SYS_LARGEFILE
AC_CHECK_HEADERS(stdint.h inttypes.h)
AC_CHECK_HEADERS(mathf.h)
AC_CHECK_HEADERS(float.h)
AC_CHECK_DECL(strchr, MY_DEFINE(HAVE_STRCHR))
AC_CHECK_DECL(memcpy, MY_DEFINE(HAVE_MEMCPY))
AC_CHECK_DECL(strsep, MY_DEFINE(HAVE_STRSEP))
AC_CHECK_HEADERS(sys/time.h)
AC_HEADER_TIME
AC_TYPE_OFF_T
AC_CHECK_TYPES([in_port_t, socklen_t], , , 
	[#include <sys/types.h>
	 #include <sys/socket.h>
	 #include <netinet/in.h>])
AC_CHECK_MEMBERS(fpos_t.__pos,,, [#include <stdio.h>])

AC_OUTPUT([
common/Makefile
common/mp4v2/Makefile
libfaac/Makefile
frontend/Makefile
include/Makefile
Makefile])
