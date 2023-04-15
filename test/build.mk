ifndef INC_DIR_LIBHDF5
    echo '*** Execute make at the root level only.'
    exit 1
endif

OBJECTS= alvin.o simon.o

# --- WRH5 include directory:
INCDIR_WRH5 = ../src

# --- WRH5 library directory:
LIBDIR_WRH5 = -L ../lib
SO_WRH5 = -l :libwrh5.so

# --- All targets. Default action.
all:	alvin simon

# --- Test program executables.
alvin:	$(OBJECTS)
	$(CC) -o alvin alvin.o -lm $(LIBDIR_WRH5) $(SO_WRH5) ${LINK_LIBHDF5}
simon:	$(OBJECTS)
	$(CC) -o simon simon.o -lm $(LIBDIR_WRH5) $(SO_WRH5) ${LINK_LIBHDF5}

# --- Remove binaries and data files in testdata subdirectory.
clean:
	rm -f alvin simon $(OBJECTS)

# --- Store important suffixes in the .SUFFIXES macro.
.SUFFIXES:	.o .c	

# --- Generate anyfile.o from anyfile.c.
%.o:    %.c build.mk $(INCDIR_WRH5)
	$(CC) $(CFLAGS) $< -I. -I $(INCDIR_WRH5) -I ${INC_DIR_LIBHDF5}

