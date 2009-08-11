AC_PREREQ(2.52)

dnl initialize autoconf
AC_INIT([OMX_TI_AUDIO_DECODE], [1.0.0], [http://www.ti.com])

dnl define PACKAGE_VERSION_* variables
dnl AS_VERSION

dnl can autoconf find the source ?
AC_CONFIG_SRCDIR([base/src/omx_audiodecoder.c])

dnl set the autotools aux directory
AC_CONFIG_AUX_DIR([config])

dnl AM_MAINTAINER_MODE only provides the option to configure to enable it
AM_MAINTAINER_MODE

dnl sets host_*, target_ variables
AC_CANONICAL_SYSTEM

dnl initialize automake
AM_INIT_AUTOMAKE([foreign])

dnl find a compiler
AC_PROG_CC
AM_PROG_AS
AM_PROG_LIBTOOL

dnl *** autotools stuff ****    
dnl allow for different autotools
dnl AS_AUTOTOOLS_ALTERNATE

dnl *** checks for platform ***
AC_DEFINE_UNQUOTED(CPU_`echo $target_cpu | tr a-z- A-Z_`,[1])
AC_DEFINE_UNQUOTED(OS_` echo $target_os  | tr a-z- A-Z_`,[1])

dnl *** checks for programs ***
AC_PROG_INSTALL

dnl Check for pkgconfig first
AC_CHECK_PROG([HAVE_PKGCONFIG], [pkg-config], [yes], [no])

dnl *** checks for libraries ***
dnl Check for OMX Core:
PKG_CHECK_MODULES([OMXCORE], [libOMX_Core])

dnl Check for OMX Base:
PKG_CHECK_MODULES([OMXBASE], [libOMX_Base])

dnl Check for OMX mm_osal
PKG_CHECK_MODULES([MMOSAL], [libmmosal])

dnl Check for LCML:
PKG_CHECK_MODULES([LCML], [libLCML])

dnl *** checks for header files ***
dnl check if we have ANSI C header files
AC_HEADER_STDC

dnl *** checks for types/defines ***

dnl *** checks for structures ***

dnl *** checks for compiler characteristics ***

dnl *** checks for library functions ***

dnl *** checks for dependancy libraries ***

dnl *** set variables based on configure arguments ***
dnl check for enabled testapp build
AC_ARG_ENABLE(tests,
        AC_HELP_STRING([--enable-tests], [Build test app(s) (default: disabled)]),
        [case "${enableval}" in
              yes) enable_tests=true ;;
              no)  enable_tests=false ;;
              *) AC_MSG_ERROR(bad value ${enableval} for --enable-tests)
                ;;
        esac], [enable_tests=false])
if test "x$enable_tests" = "xtrue"; then
    PKG_CHECK_MODULES([ALSA], [alsa], [], [
        echo "*** ALSA not found, but is required for testapp.. disabling testapp"
        enable_tests=false
    ])
fi
AM_CONDITIONAL([BUILD_TESTS], test "x$enable_tests" = "xtrue")

AC_CONFIG_FILES([
libOMX.TI.AUDIO.DECODE.pc
Makefile
base/Makefile
base/src/Makefile
aac_dec/Makefile
aac_dec/src/Makefile
mp3_dec/Makefile
mp3_dec/src/Makefile
tests/Makefile
])
AC_OUTPUT
