ifndef INC_DIR_LIBHDF5
$(info src.mk: *** INC_DIR_LIBHDF5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef LIB_DIR_LIBWRH5
$(info src.mk: *** LIB_DIR_LIBWRH5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef LINK_LIBHDF5
$(info src.mk: *** LINK_LIBHDF5 was not found.)
$(error Execute make at the root level only.)
endif

ifndef CFLAGS
$(info src.mk: *** CFLAGS was not found.)
$(error Execute make at the root level only.)
endif

ifndef SO_FILE_LIBWRH5
$(info src.mk: *** SO_FILE_LIBWRH5 was not found.)
$(error Execute make at the root level only.)
endif

all:	$(LIB_DIR_LIBWRH5)/$(SO_FILE_LIBWRH5)

$(LIB_DIR_LIBWRH5)/$(SO_FILE_LIBWRH5): wrh5_open.o wrh5_close.o wrh5_write.o wrh5_util.o
	mkdir -p $(LIB_DIR_LIBWRH5)
	gcc -shared -o $@ $^ ${LINK_LIBHDF5}

# --- Generate anyfile.o from anyfile.c
%.o:	%.c wrh5_defs.h src.mk
	gcc $(CFLAGS) -I . -I $(INC_DIR_LIBHDF5) $<

clean:
	rm -rf *.o $(LIB_DIR_LIBWRH5)

