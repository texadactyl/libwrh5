ifndef INC_DIR_LIBHDF5
$(info voyager.mk: *** INC_DIR_LIBHDF5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef LINK_LIBHDF5
$(info voyager.mk: *** LINK_LIBHDF5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef INC_DIR_LIBWRH5
$(info voyager.mk: *** INC_DIR_LIBWRH5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef LINK_LIBWRH5
$(info voyager.mk: *** LINK_LIBWRH5 was not found.)
$(error Execute make at the root level only.)
endif

OBJECTS= theodore.o

# --- All targets. Default action.
all:	theodore

# --- Test program executables.
theodore:	$(OBJECTS)
	gcc -o theodore theodore.o $(LINK_LIBWRH5)

# --- Remove binaries and data files in testdata subdirectory.
clean:
	rm -f theodore $(OBJECTS)

# --- Store important suffixes in the .SUFFIXES macro.
.SUFFIXES:	.o .c	

# --- Generate anyfile.o from anyfile.c.
%.o:    %.c voyager.mk $(INCDIR_WRH5)
	gcc $(CFLAGS) -I. -I $(INC_DIR_LIBWRH5) -I $(INC_DIR_LIBHDF5) $<

