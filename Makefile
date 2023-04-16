#--- Build parameters ---

# libwrh5 artifacts
CURDIR = ${shell pwd}
export INC_DIR_LIBWRH5 = $(CURDIR)/src
export LIB_DIR_LIBWRH5 = $(CURDIR)/lib
export SO_FILE_LIBWRH5 = libwrh5.so
export LINK_LIBWRH5 = -L ${LIB_DIR_LIBWRH5} -l :$(SO_FILE_LIBWRH5)

# libhdf artifacts
export INC_DIR_LIBHDF5 = /usr/include/hdf5/serial/ 
export SO_DIR_LIBHDF5 = /usr/lib/x86_64-linux-gnu/hdf5/serial/
SO_LIBHDF5 := :libhdf5.so
SO_LIBHDF5_HL := :libhdf5_hl.so

# For use in a link step
export LINK_LIBHDF5 = -L ${SO_DIR_LIBHDF5} -l $(SO_LIBHDF5) -l $(SO_LIBHDF5_HL)

# gcc flags
export CFLAGS = -c -fPIC

# Parameters for install/uninstall
PREFIX ?= /usr/local
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# Parameters for testing
TEST_DATA = $(CURDIR)/test_data
UNIT_TESTS = $(CURDIR)/testing/unit_tests
VOYAGER = $(CURDIR)/testing/voyager

# Parameters for try
export LD_LIBRARY_PATH = ${shell pwd}/lib

# --- Actions ---

# Compile and link edit
all:
	cd src && $(MAKE) -f build.mk
	cd $(UNIT_TESTS) && $(MAKE) -f build.mk

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
	cd $(UNIT_TESTS) && $(MAKE) -f build.mk clean
	rm -f lib/*.so
	rm -f ./test_data/*.h5

# Try the basic unit test programs
try:
	cd $(UNIT_TESTS)
	# Run simon and dump the output header:
	$(UNIT_TESTS)/simon $(TEST_DATA)/simon.h5
	h5dump -A $(TEST_DATA)/simon.h5
	# Run alvin and dump the output header:
	$(UNIT_TESTS)/alvin $(TEST_DATA)/alvin.h5
	h5dump -A $(TEST_DATA)/alvin.h5

