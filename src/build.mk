ifndef INC_DIR_LIBHDF5
    echo '*** Execute make at the root level only.'
    exit 1
endif

all: ${SO_LIBWRH5}

${SO_LIBWRH5}: wrh5_open.o wrh5_close.o wrh5_write.o wrh5_util.o
	$(CC) -shared -o $@ $^ ${LINK_LIBHDF5}

# --- Generate anyfile.o from anyfile.c
%.o: %.c wrh5_defs.h Makefile
	$(CC) $(CFLAGS) $< 

clean:
	rm -f *.o ${SO_LIBWRH5}

