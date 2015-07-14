# SETUP generation of docs directory
EXTRA_DIST=doxy.conf.in mainpage.doc
.PHONY: doxygen-doc
all: doxygen-doc

DOCS_DIR=@top_builddir@/apidocs
doxygen-doc: $(DOCS_DIR)/nicesat.tag
$(DOCS_DIR)/nicesat.tag: doxy.conf $(nobase_include_HEADERS) $(mainpage_DOCS) \
	                       $(libnice_dbg_a_SOURCES) $(nicesat_dbg_SOURCES)
	mkdir -p $(DOCS_DIR)
if HAVE_DOXYGEN
	@DOXYGEN@ doxy.conf
endif

dist-hook: doxygen-doc
	cp -pr $(DOCS_DIR) $(distdir)/apidocs

clean-local:
	rm -fr $(DOCS_DIR)

lib_LTLIBRARIES=libnice_dbg.la libnice_opt.la
bin_PROGRAMS=nicesat_dbg nicesat_opt

mainpage_DOCS=mainpage.doc

# The headers and source files for the library
nobase_include_HEADERS=        \
 Support/Shared.H              \
 ADTs/List.H                   \
 ADTs/Vec.H                    \
 ADTs/HashFuncs.H              \
 ADTs/HashTable.H              \
 ADTs/Sort.H                   \
 ADTs/Node.H                   \
 ADTs/Edge.H                   \
 ADTs/GlobalTable.H            \
 Support/BitReader.H           \
 Support/BitWriter.H           \
 Support/Reader.H              \
 Input/AigReader.H             \
 Passes/StatsPass.H            \
 Passes/GraphVizPass.H         \
 ADTs/LitVector.H              \
 ADTs/CnfExp.H                 \
 Solvers/Solver.H              \
 Solvers/NullSolver.H          \
 Solvers/FileSolver.H          \
 Solvers/PicoSatSolver.H       \
 Passes/CountAndExpandPass.H   \
 Passes/CnfPass.H              \
 NiceSat.H                     \
 Support/Options.H             \
 NamedNiceSat.H

libnice_dbg_la_SOURCES=        \
 Support/Shared.C              \
 ADTs/HashFuncs.C              \
 ADTs/Node.C                   \
 ADTs/GlobalTable.C            \
 NiceSat_construction.C        \
 Support/BitReader.C           \
 Support/BitWriter.C           \
 Support/Reader.C              \
 Input/AigReader.C             \
 Passes/StatsPass.C            \
 ADTs/LitVector.C              \
 ADTs/CnfExp.C                 \
 Solvers/FileSolver.C          \
 Solvers/PicoSatSolver.C       \
 Passes/CountAndExpandPass.C   \
 Passes/CnfPass.C              \
 Passes/CnfPass_conjunction.C  \
 Passes/CnfPass_disjunction.C  \
 NiceSat.C
libnice_opt_la_SOURCES=$(libnice_dbg_la_SOURCES)

nicesat_dbg_SOURCES=           \
 Support/Options.C             \
 Main.C
nicesat_opt_SOURCES=$(nicesat_dbg_SOURCES)

WARN_CFLAGS=-Wall -Werror -Wwrite-strings -Wno-sign-compare
WARN_CXXFLAGS=$(WARN_CFLAGS) -Woverloaded-virtual
INC_CFLAGS=-I@top_srcdir@/libs/picosat-913
OPT_CFLAGS=-DNDEBUG -O3 -fomit-frame-pointer -finline-limit-10000

libnice_dbg_la_CXXFLAGS=$(WARN_CXXFLAGS) $(INC_CFLAGS) -g -O0

nicesat_dbg_CXXFLAGS=$(WARN_CXXFLAGS) $(INC_CFLAGS) -g -O0
nicesat_dbg_LDADD=libnice_dbg.la @top_builddir@/libs/picosat-913/libpicosat.la

libnice_opt_la_CXXFLAGS=$(WARN_CFLAGS) $(INC_CFLAGS) $(OPT_CFLAGS)

nicesat_opt_CXXFLAGS=$(WARN_CXXFLAGS) $(INC_CFLAGS) $(OPT_CFLAGS)
nicesat_opt_CFLAGS=$(WARN_CFLAGS) $(INC_CFLAGS) $(OPT_CFLAGS)
nicesat_opt_LDADD=libnice_opt.la @top_builddir@/libs/picosat-913/libpicosat.la
