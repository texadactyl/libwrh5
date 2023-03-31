# Expose the libwrh5 shared object folder:
export LD_LIBRARY_PATH=./lib

# Run simon and dump the output header:
./test/simon ./test_data/simon.h5
h5dump -A ./test_data/simon.h5

# Run alvin and dump the output header:
./test/alvin -v ./test_data/alvin.h5
h5dump -A ./test_data/alvin.h5

