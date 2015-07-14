# If we have CPPUNIT, build the tests
if HAVE_CPPUNIT
check_PROGRAMS=tests_dbg tests_opt
TESTS=tests_dbg tests_opt
else
check_PROGRAMS=
TESTS=
endif

# The headers and source files for the unit tests
noinst_HEADERS=                 \
 Support/TestHelpers.H          \
 ADTs/VecTests.H                \
 ADTs/ListTests.H               \
 ADTs/HashTableTests.H          \
 ADTs/EdgeTests.H               \
 ADTs/NodeTests.H               \
 Support/BitReaderTests.H       \
 Support/BitWriterTests.H       \
 ADTs/LitVectorTests.H          \
 ADTs/CnfExpTests.H             \
 NiceSatTests.H                 \
 NamedNiceSatTests.H

AM_CXXFLAGS=-Wall -g -O0 @CPPUNIT_CFLAGS@ -I@top_srcdir@/src

tests_dbg_LDADD=@top_builddir@/src/libnice_dbg.a @CPPUNIT_LIBS@
tests_dbg_SOURCES=              \
 Support/TestHelpers.C          \
 ADTs/VecTests.C                \
 ADTs/ListTests.C               \
 ADTs/HashTableTests.C          \
 ADTs/EdgeTests.C               \
 ADTs/NodeTests.C               \
 Support/BitReaderTests.C       \
 Support/BitWriterTests.C       \
 ADTs/LitVectorTests.C          \
 ADTs/CnfExpTests.C             \
 NiceSatTests.C                 \
 NiceSatTests_construction.C    \
 NamedNiceSatTests.C            \
 Main.C

tests_opt_LDADD=@top_builddir@/src/libnice_opt.a @CPPUNIT_LIBS@
tests_opt_SOURCES=$(tests_dbg_SOURCES)
