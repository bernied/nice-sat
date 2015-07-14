AC_PREREQ(2.59)
AC_INIT(NICE-SAT, 0.2, eigold@ccs.neu.edu)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/Main.C])
AC_CONFIG_HEADERS([config.h])

################################################################################
# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LIBTOOL

################################################################################
# Clear the flags
CXXFLAGS=
CPPFLAGS=
LFLAGS=

################################################################################
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/time.h])

################################################################################
# Define SIZEOF_UINT32_T and SIZEOF_UINTPTR_T for use in macros
AC_CHECK_SIZEOF(uintptr_t)
AC_CHECK_SIZEOF(uint32_t)

################################################################################
# Check for CPPUNIT
AM_PATH_CPPUNIT("1.11.6")
AM_CONDITIONAL([HAVE_CPPUNIT], [test "x$no_cppunit" = x])

################################################################################
AC_MSG_CHECKING([Variable polarity heuristic])
AC_ARG_ENABLE([polarity],
              AC_HELP_STRING([--disable-polarity],
                             [turn on/off the polarity heuristic]),[
  case "${enableval}" in
    "yes"|"")
      ax_enable_polarity="yes"
      ;;
    "no")
      ax_enable_polarity="no"
      ;;
    *)
      AC_MSG_ERROR([bad value '${enableval}' for polarity -- expected yes or no])
      ;;
   esac
], [ax_enable_polarity="yes"])

AH_TEMPLATE([POLARITY], [Define to enable the polarity heuristic])
AS_IF([test "x$ax_enable_polarity" = xyes],[ dnl
  AC_MSG_RESULT([enabled])
  AC_DEFINE([POLARITY], [])
],[dnl
  AC_MSG_RESULT([disabled])
])

################################################################################
# Configure for merging
AC_MSG_CHECKING([Merging])
AC_ARG_ENABLE([merging],
              AC_HELP_STRING([--enable-merging=k], 
	                     [turn on/off k-merging & duplicate detection]),[
  case "${enableval}" in
    "yes"|"")
      ax_enable_merging="5"
      ;;
    "no")
      ax_enable_merging="0"
      ;;
    *)
      AS_IF([test "${enableval}" -eq "${enableval}" 2> /dev/null],
            [ax_enable_merging="${enableval}"],
            [AC_MSG_ERROR([bad value '${enableval}' for merging -- expected yes, no or integer])])
      ;;
  esac
], [ax_enable_merging="5"])

AH_TEMPLATE([MERGING], [Define to enable merging up to length MERGING])
AC_DEFINE_UNQUOTED([MERGING], $ax_enable_merging)
AS_IF([test "$ax_enable_merging" -ne 0],[ dnl
	AC_MSG_RESULT([enabled to length $ax_enable_merging])
],[ dnl
	AC_MSG_RESULT([disabled])
])

################################################################################
# Enable (or disable) the integration tests
AC_MSG_CHECKING([Integration tests])
AC_ARG_ENABLE([integration],
              AC_HELP_STRING([--disable-integration],
                             [turn on/off the integration tests]),[
  case "${enableval}" in
    "yes"|"")
      ax_enable_integration="yes"
      ;;
    "no")
      ax_enable_integration="no"
      ;;
    *)
      AC_MSG_ERROR([bad value '${enableval}' for integration -- expected yes or no])
      ;;
  esac
], [ax_enable_integration="yes"])
AM_CONDITIONAL([RUN_INTEGRATION_TESTS], [test x$ax_enable_integration = xyes])
AS_IF([test x$ax_enable_integration = xyes],[ dnl
	AC_MSG_RESULT([enabled])
],[
	AC_MSG_RESULT([disabled])
])

################################################################################
# PicoSat interface
AC_DEFINE_UNQUOTED([PICOSAT_CC], "$CXX", [C Compiler used for building PicoSat])
AC_DEFINE([PICOSAT_VERSION], ["913"], [Version of PicoSat being used])
AC_DEFINE_UNQUOTED([PICOSAT_CFLAGS], "$CFLAGS", [CFLAGS used to compile PicoSat])
################################################################################
# Check for doxygen and dot
AC_PATH_PROG([DOXYGEN], [doxygen])
AC_ARG_VAR([DOXYGEN], ["The path to the doxygen executable"])
AM_CONDITIONAL([HAVE_DOXYGEN], [test x$DOXYGEN != x])

AC_PATH_PROG([DOT],    [dot])
AC_ARG_VAR([DOT],      ["The path to the dot executable"])
AC_ARG_VAR([HAVE_DOT], ["Set to yes or no depending on whether DOT was found"])
AS_IF([test x$DOT = x],[ dnl
  HAVE_DOT="NO"
],[
  HAVE_DOT="YES"
])
DOT_PATH=$(dirname $DOT)
AC_ARG_VAR([DOT_PATH], ["The path containing the dot executable"])

################################################################################
# Files to be produced from templates
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
                 tests/unit/Makefile
                 tests/integration/Makefile
                 libs/Makefile
                 swig/Makefile
                 libs/picosat-913/Makefile
                 src/doxy.conf])
AC_CONFIG_FILES([tests/integration/run-tests.sh], [chmod u+x tests/integration/run-tests.sh])
AC_OUTPUT
