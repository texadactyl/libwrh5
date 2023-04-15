#--- Build parameters ---

# libwrh5 artifacts
cur_dir = ${shell pwd}
export H_LIBWRH5 = $(cur_dir)/src/wrh5_defs.h
export SO_LIBWRH5 = $(cur_dir)/lib/libwrh5.so

# libhdf directories
export INC_DIR_LIBHDF5 = /usr/include/hdf5/serial/ 
export SO_DIR_LIBHDF5 = -L /usr/lib/x86_64-linux-gnu/hdf5/serial/

#libhdf5 .so files
SO_LIBHDF5 = :libhdf5.so
SO_LIBHDF5_HL = :libhdf5_hl.so

# For use in a link step
export LINK_LIBHDF5 = ${SO_DIR_LIBHDF5} -l $(SO_LIBHDF5) -l $(SO_LIBHDF5_HL)

# gcc flags
ifndef CC
	export CC := gcc
endif
export CFLAGS = -c -fPIC -I $(INC_DIR_LIBHDF5)

# Parameters for try
export LD_LIBRARY_PATH = ${shell pwd}/lib

#--- Install parameters ---

# Can be pre-empted by an environment variable, "PREFIX"
ifndef PREFIX
	PREFIX := /usr/local
endif

# Install target directories
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# --- Actions

# Compile and link edit
all:
	cd src && $(MAKE) -f build.mk
	cd test && $(MAKE) -f build.mk

# System installation - super user access
install: $(SO_LIBWRH5) $(H_LIBWRH5)
	mkdir -p $(INCDIR)
	cp -p $(H_LIBWRH5) $(INCDIR)
	mkdir -p $(LIBDIR)
	cp -p $(SO_LIBWRH5) $(LIBDIR)

# System uninstallation - super user access
uninstall:
	rm $(INCDIR)/$(H_LIBWRH5)
	rm $(LIBDIR)/$(SO_LIBWRH5)

# Get rid of all made artifacts
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

