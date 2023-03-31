PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
DATADIR = $(PREFIX)/share
CC            := gcc
CXX           ?= g++
HOST_COMPILER ?= $(CXX)

#-----------------------------------------------------------------------------
# Begin include and lib definitions

INCDIR_H5 = /usr/include/hdf5/serial/
LIBDIR_H5 = -L /usr/lib/x86_64-linux-gnu/hdf5/serial/
CFLAGS = -c -ggdb -fPIC -I $(INCDIR_H5)

LIBHDF5= :libhdf5.so
LIBHDF5_HL= :libhdf5_hl.so
LINKH5:= $(LIBDIR_H5) -l $(LIBHDF5) -l $(LIBHDF5_HL)

# End include and lib definitions
#-----------------------------------------------------------------------------

OUTLIB=../lib/libwrh5.so

all: ${OUTLIB}

${OUTLIB}: wrh5_open.o wrh5_close.o wrh5_write.o wrh5_util.o
	$(CC) -shared -o $@ $^ $(LINKH5)

install: ${OUTLIB} wrh5_defs.h
	mkdir -p $(INCDIR)
	cp -p wrh5_defs.h $(INCDIR)
	mkdir -p $(LIBDIR)
	cp -p ${OUTLIB} $(LIBDIR)

# --- Generate anyfile.o from anyfile.c
%.o: %.c wrh5_defs.h Makefile
	$(CC) $(CFLAGS) $< 

clean:
	rm -f *.o ${OUTLIB}

