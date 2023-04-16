#--- Build parameters ---

# libwrh5 artifacts
CURDIR = ${shell pwd}
export H_LIBWRH5 = $(CURDIR)/src/wrh5_defs.h
export SO_LIBWRH5 = $(CURDIR)/lib/libwrh5.so

# libhdf directories
export INC_DIR_LIBHDF5 = /usr/include/hdf5/serial/ 
export SO_DIR_LIBHDF5 = -L /usr/lib/x86_64-linux-gnu/hdf5/serial/

#libhdf5 .so files
SO_LIBHDF5 := :libhdf5.so
SO_LIBHDF5_HL := :libhdf5_hl.so

# For use in a link step
export LINK_LIBHDF5 = ${SO_DIR_LIBHDF5} -l $(SO_LIBHDF5) -l $(SO_LIBHDF5_HL)

# gcc flags
export CC := gcc
export CFLAGS = -c -fPIC -I $(INC_DIR_LIBHDF5)

# Parameters for install/uninstall
PREFIX ?= /usr/local
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# Parameters for try
export LD_LIBRARY_PATH = ${shell pwd}/lib

# --- Actions ---

# Compile and link edit
all:
	cd src && $(MAKE) -f build.mk
	cd test && $(MAKE) -f build.mk

# System installation - super user access
install:
	@echo PREFIX=$(PREFIX)
	mkdir -p $(INCDIR)
	cp -p ./src/*.h $(INCDIR)
	mkdir -p $(LIBDIR)
	cp -p $(SO_LIBWRH5) $(LIBDIR)

# System uninstallation - super user access
uninstall:
	rm $(INCDIR)/wrh5*.h
	rm $(LIBDIR)/libwrh5.so

# Get rid of make all & try artifacts
clean:
	cd src && $(MAKE) -f build.mk clean
	cd test && $(MAKE) -f build.mk clean
	rm -f ./test_data/*.h5

# Try test programs
try:
	# Run simon and dump the output header:
	./test/simon ./test_data/simon.h5
	h5dump -A ./test_data/simon.h5

	# Run alvin and dump the output header:
	./test/alvin -v ./test_data/alvin.h5
	h5dump -A ./test_data/alvin.h5

blabber:
	@echo PREFIX=$(PREFIX)
	@echo INCDIR=$(INCDIR)
	@echo LIBDIR=$(LIBDIR)

