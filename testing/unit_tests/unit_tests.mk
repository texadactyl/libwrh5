ifndef INC_DIR_LIBHDF5
$(info unit_tests.mk: *** INC_DIR_LIBHDF5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef LINK_LIBHDF5
$(info unit_tests.mk: *** LINK_LIBHDF5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef INC_DIR_LIBWRH5
$(info unit_tests.mk: *** INC_DIR_LIBWRH5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef LINK_LIBWRH5
$(info unit_tests.mk: *** LINK_LIBWRH5 was not found.)
$(error Execute make at the root level only.)
endif

OBJECTS= alvin.o simon.o

# --- All targets. Default action.
all:	alvin simon

# --- Test program executables.
alvin:	$(OBJECTS)
	gcc -o alvin alvin.o $(LINK_LIBWRH5)
simon:	$(OBJECTS)
	gcc -o simon simon.o $(LINK_LIBWRH5)

# --- Remove binaries and data files in testdata subdirectory.
clean:
	rm -f alvin simon $(OBJECTS)

# --- Store important suffixes in the .SUFFIXES macro.
.SUFFIXES:	.o .c	

# --- Generate anyfile.o from anyfile.c.
%.o:    %.c unit_tests.mk $(INCDIR_WRH5)
	gcc $(CFLAGS) -I. -I $(INC_DIR_LIBWRH5) -I $(INC_DIR_LIBHDF5) $<

