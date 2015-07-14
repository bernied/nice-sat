EXTRA_DIST = NiceSat.I

CLEANFILES = *.lisp libcffinice.so NiceSat_wrap.*

# messy stuff

NiceSat_wrap.cxx: NiceSat.I # missing NiceSat and other deps
	swig -c++ -cffi NiceSat.I

NiceSat_wrap.o: NiceSat_wrap.cxx
	$(CXX) -fPIC -c NiceSat_wrap.cxx -o NiceSat_wrap.o \  
	    -I ../src/ -I ../libs/picosat-913/  

libcffinice.so: NiceSat_wrap.o
	$(CC) -shared -Wl,-soname,cffinice NiceSat_wrap.o -o libcffinice.so