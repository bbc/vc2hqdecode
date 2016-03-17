AC_PREREQ([2.68])
AC_INIT([VC2HQDECODE],[0.1.0.2])

AC_CANONICAL_HOST
case $host in
x86_64-*)
        TARGET="x86_64"
        ;;
*)
        TARGET="unknown"
        ;;
esac
if test "$TARGET" != "x86_64" ; then
   AC_MSG_ERROR([Only x86_64 architecture is supported])
fi;

AS_NANO(VC2HQDECODE_CVS=no,VC2HQDECODE_CVS=yes)

AM_INIT_AUTOMAKE([1.11 silent-rules subdir-objects])
AM_SILENT_RULES([yes])

# Version number that will be appended to the library filename
VC2HQDECODE_MAJORMINOR=0.1
AC_SUBST(VC2HQDECODE_MAJORMINOR)

AC_CONFIG_HEADERS([config.h])

# CURRENT, REVISION, AGE
# - library source changed but interface the same -> increment REVISION
# - interfaces added/removed/changed -> increment CURRENT, set REVISION = 0
# - interfaces added -> increment AGE also
# - interfaces removed -> set AGE = 0
#    (AGE is the number of previous values of CURRENT that are compatible)
VC2HQDECODE_LIBVERSION="0:0:0"
AC_SUBST(VC2HQDECODE_LIBVERSION)

LT_PREREQ([2.2.6])
LT_INIT(disable-static win32-dll)

AC_PROG_CXX
AM_PROG_CC_C_O

AC_CXX_HAVE_SSTREAM

AC_CONFIG_SRCDIR([vc2hqdecode/vc2hqdecode.h])

AC_CONFIG_MACRO_DIR([m4])
ACLOCAL_AMFLAGS="-I m4 $ACLOCAL_AMFLAGS"
AC_SUBST(ACLOCAL_AMFLAGS)

DX_DOXYGEN_FEATURE(ON)
DX_INIT_DOXYGEN(vc2hqdecode, doxygen.cfg)

# Check if we should enable debug support  (./configure --enable-debug)
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging
					   [default=$debug_default]],, enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
#		 VC2HQDECODE_CFLAGS="$VC2HQDECODE_CFLAGS -g -O0"
		 VC2HQDECODE_CFLAGS="$VC2HQDECODE_CFLAGS -g -Og -DDEBUG"
		 CXXFLAGS="$CXXFLAGS -O0"
     VC2HQDECODE_LDFLAGS="$VC2HQDECODE_LDFLAGS -pg"
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
		 VC2HQDECODE_CFLAGS="-g -O3 -funroll-loops"
		 CXXFLAGS="-g -O3 -funroll-loops -Wall"
     VC2HQDECODE_LDFLAGS=""
fi

AX_CXX_COMPILE_STDCXX_11

# pthreads is required for mutex
#
######### CHECK!
#
AX_PTHREAD(HAVE_PTHREADS=yes, HAVE_PTHREADS=no)
if test "x${HAVE_PTHREADS}" != xyes ; then
  AC_MSG_ERROR(pthreads is required)
fi

# rt is required shm functions
#
######### CHECK!
#
##AC_SEARCH_LIBS([shm_open], [rt], HAVE_DL=yes, HAVE_DL=no)
##if test "x${HAVE_DL}" != xyes ; then
##  AC_MSG_ERROR(dl is required)
##fi


# Check for pkg-config
PKG_PROG_PKG_CONFIG([0.26])

NUMA_LIBS=""
RT_LIBS=""
IS_LINUX=no
AC_CANONICAL_HOST
case $host_os in
  linux*)
    IS_LINUX=yes

    #Add hidden visibility by default
    CXXFLAGS="$CXXFLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
    LDFLAGS="$LDFLAGS -fvisibility=hidden -fvisibility-inlines-hidden"

    #Check for librt
    #dnl librt is required
    AC_CHECK_LIB(rt, clock_gettime, [RT_LIBS="-lrt"], [AC_MSG_ERROR([librt is required])])
    AC_ARG_WITH([numa],
        AS_HELP_STRING([--without-numa], [Ignore presence of numa and disable it]))

    AS_IF([test "x$with_numa" != "xno"],
      [AC_CHECK_LIB(numa, numa_available, [have_numa=yes], [have_numa=no])],
      [have_numa=no])

    AS_IF([test "x$have_numa" = "xyes"],
      [NUMA_LIBS="-lnuma"
       AC_DEFINE([VC2_USE_NUMA], [], [Use NUMA library for processor affinity])],
      [AS_IF([ test "x$with_numa" = "xyes" ],
        [AC_MSG_ERROR([libnuma is required])
      ])
    ])
    ;;
esac
AM_CONDITIONAL(IS_LINUX, test x${IS_LINUX} = xyes)
AC_SUBST(IS_LINUX)
AC_SUBST(NUMA_LIBS)
AC_SUBST(RT_LIBS)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AX_CREATE_STDINT_H([vc2hqdecode/vc2hqdecode-stdint.h])

AC_APPEND_SUPPORTED_CFLAGS(VC2HQDECODE_CFLAGS, [-Wall])
if test "x$VC2HQDECODE_CVS" = "xyes"
then
  AC_APPEND_SUPPORTED_CFLAGS(VC2HQDECODE_CFLAGS, [-Werror -Wextra])
fi

VC2HQDECODE_CFLAGS="$VC2HQDECODE_CFLAGS -I\$(top_srcdir)"
AC_SUBST(VC2HQDECODE_CFLAGS)

VC2HQDECODE_LDFLAGS="$VC2HQDECODE_LDFLAGS"
AC_APPEND_SUPPORTED_LDFLAGS(VC2HQDECODE_LDFLAGS, [-Wl,--no-undefined])
if test "x$IS_LINUX" = "xno"
then
  AC_APPEND_SUPPORTED_LDFLAGS(VC2HQDECODE_LDFLAGS, [-Qunused-arguments])
fi
AC_SUBST(VC2HQDECODE_LDFLAGS)

VC2HQDECODE_LIBS="\$(top_builddir)/vc2hqdecode/libvc2hqdecode-$VC2HQDECODE_MAJORMINOR.la"
AC_SUBST(VC2HQDECODE_LIBS)

AC_SUBST(VC2HQDECODE_PKG_DEPS)

pkgconfigdir="\$(libdir)/pkgconfig"
AC_SUBST(pkgconfigdir)

vc2hqdecodebindir="\$(libdir)/vc2hqdecode/"
AC_SUBST(vc2hqdecodebindir)

configfiledir="\$(sysconfdir)/vc2hqdecode"
AC_SUBST(configfiledir)

SSE4_2_FLAGS=" -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2"
AVX_FLAGS=" -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mavx"
AVX2_FLAGS=" -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mavx -mavx2"
AC_SUBST(SSE4_2_FLAGS)
AC_SUBST(AVX_FLAGS)
AC_SUBST(AVX2_FLAGS)

REDIST_CFLAGS=" -I\$(top_srcdir)/redist/ "
AC_SUBST(REDIST_CFLAGS)

AC_CONFIG_FILES([
Makefile
vc2hqdecode-uninstalled.pc
vc2hqdecode.pc
vc2hqdecode/Makefile
testprogs/Makefile
tools/Makefile
vc2inversetransform_c/Makefile
vc2inversetransform_sse4_2/Makefile
testsuite/Makefile
redist/Makefile
])
AC_OUTPUT
