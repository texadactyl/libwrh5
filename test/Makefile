# --- macros
CC= gcc
CFLAGS= -c -Wall -Werror -fpic
LFLAGS= -Wall 
OBJECTS= alvin.o simon.o

# --- hdf5 include directory:
INCDIR_H5 = /usr/include/hdf5/serial/

# --- WRH5 include directory:
INCWRH5= ../src

# --- WRH5 library directory:
LIBWRH5_DIR= -L ../lib
WRH5_SO= -l :libwrh5.so

# --- All targets. Default action.
all:	alvin simon

# --- Test program executables.
alvin:	$(OBJECTS)
	$(CC) $(LFLAGS) -o alvin alvin.o -lm $(LIBWRH5_DIR) $(WRH5_SO)
simon:	$(OBJECTS)
	$(CC) $(LFLAGS) -o simon simon.o -lm $(LIBWRH5_DIR) $(WRH5_SO)

# --- Remove binaries and data files in testdata subdirectory.
clean:
	rm -f alvin simon $(OBJECTS)

# --- Store important suffixes in the .SUFFIXES macro.
.SUFFIXES:	.o .c	

# --- Generate anyfile.o from anyfile.c.
%.o:    %.c Makefile $(INCWRH5)
	$(CC) $(CFLAGS) $< -I. -I $(INCWRH5) -I ${INCDIR_H5}

