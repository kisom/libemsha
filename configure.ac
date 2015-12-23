# autoconf version 2.68 and automake version 1.11 seem to be the latest
# versions that can be used with Travis right now.
AC_PREREQ([2.68])
AC_INIT([libemsha],
	[1.0.1],
	[coder@kyleisom.net],
	[libemsha],
	[https://kyleisom.net/projects/libemsha/])
AM_INIT_AUTOMAKE([1.11 foreign])

AC_CONFIG_SRCDIR([src/emsha/sha256.hh])
AC_CONFIG_FILES([Makefile src/Makefile doc/source/conf.py doc/source/header.rst])
AC_CONFIG_FILES([do-release], [chmod +x do-release])
AC_CONFIG_MACRO_DIR([m4])

AC_CHECK_HEADERS

LT_INIT
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_CC_C_O

AC_CHECK_PROGS([SPHINX], [sphinx-build])
if test -z "$SPHINX";
then
       	AC_MSG_WARN([Sphinx not found - continuing without Sphinx support])
fi

AC_ARG_ENABLE([hexstring],
    AS_HELP_STRING([--disable-hexstring], [Don't build with support for hex string output (default enabled)]),
    [:], [enable_hexstring=check])

AC_ARG_ENABLE([hexlut],
    AS_HELP_STRING([--disable-hexlut], [Don't build with the larger LUT for building hex strings (saves ~1KB of memory). This has no effect if hex strings are disabled.]),
    [:], [enable_hexlut=check])

AC_ARG_ENABLE([selftest],
    AS_HELP_STRING([--disable-selftest], [Don't build with support for internal self-tests (saves some memory that is used by the test vectors.)]),
    [:], [enable_selftest=check])

# The default for the ARG_ENABLE options is to have them enabled.
AS_IF([test "x$enable_hexstring" == "xno"], [
	AC_MSG_NOTICE([disabling hexstrings.])
	AC_DEFINE(EMSHA_NO_HEXSTRING)
])

AS_IF([test "x$enable_hexlut" == "xno"], [
	AC_MSG_NOTICE([disabling the large hexstring LUT.])
	AC_DEFINE(EMSHA_NO_HEXLUT)      
])

AS_IF([test "x$enable_selftest" == "xno"], [
       AC_MSG_NOTICE([disabling the internal self tests.])
	AC_DEFINE(EMSHA_NO_SELFTEST)
])


AM_CONDITIONAL([HAVE_SPHINX], 
	       [test -n "$SPHINX"])

AC_OUTPUT
