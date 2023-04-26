#--- Build parameters ---

# libwrh5 artifacts
CURDIR = ${shell pwd}
export INC_DIR_LIBWRH5 = $(CURDIR)/src
export LIB_DIR_LIBWRH5 = $(CURDIR)/lib
export SO_FILE_LIBWRH5 = libwrh5.so
export LINK_LIBWRH5 = -L ${LIB_DIR_LIBWRH5} -l :$(SO_FILE_LIBWRH5)

# libhdf5 artifacts
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

# Help (default action)
help:
	@echo
	@echo 'make build : Create lib/libwr5.so. Compile the unit tests (simon, alvin) and the Voyager 1 test (theodore).'
	@@echo 'make install : Copy lib/libwr5.so to $(PREFIX)/lib and src/*.h to $(PREFIX)/include.'
	@echo 'make uninstall : Reverse the effects of make install.'
	@echo 'make clean : Remove src/*.o, the lib directory, and the test_data directory.'
	@echo 'make try: Run unit tests simon and alvin.'
	@echo '          * Simon creates a Filterbank HDF5 file using the default caching and chunking parameters.'
	@echo '          * Alvin creates a Filterbank HDF5 file with specified caching and chunking parameters.'
	@echo 'make voya: Run theodore which uses Voyager 1 Filterbank file (.fil) data.'
	@echo '           * Download the Voyager 1 .fil file.'
	@echo '           * Scrape the header fields and the binary data into 2 separate files.'
	@echo '           * Theodore reads both scrapings and creates the corresponding Filterbank HDF5 file.'
	@echo

# Compile and link edit (default action)
build:
	cd src && $(MAKE) -f src.mk
	cd $(UNIT_TESTS) && $(MAKE) -f unit_tests.mk
	cd $(VOYAGER) && $(MAKE) -f voyager.mk

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
	cd src && $(MAKE) -f src.mk clean
	cd $(UNIT_TESTS) && $(MAKE) -f unit_tests.mk clean
	cd $(VOYAGER) && $(MAKE) -f voyager.mk clean
	rm -rf $(LIB_DIR_LIBWRH5)
	rm -rf $(TEST_DATA)

# Try the unit test programs
try:
	mkdir -p $(TEST_DATA)
	cd $(UNIT_TESTS) && pwd && bash run_unit_tests.sh $(TEST_DATA)

# Try the Voyager 1 file
voya:
	mkdir -p $(TEST_DATA)
	cd $(VOYAGER) && pwd && bash run_voya.sh $(TEST_DATA)

