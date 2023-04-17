# libwrh5 API

### OVERVIEW

The following are the high-level Filterbank HDF5 writing functions: 

* wrh5_open - Initialize writing to a new HDF5 file or one to be replaced. Optional user-specified chunking and caching parameters may be provided.
* wrh5_write - Present a buffer to be written.
* wrh5_close - Finalize the HDF5 file.

### FUNCTIONS

All functions return either 0 (success) or 1 (failure).  In the case of a failure, error logging will appear with supporting detail.

The context, header, user-chunking, and user-caching structures are defined in file src/wrh5_defs.h.

The output-path (char *) is an operating system absolute or relative path for specifying where to store the output HDF5 file.

The debug-flag (int) has 2 values:
* 0 : The libwrh5 functions will perform no logging unless a warning or error condition should arise.
* 1 : Many informational details that are useful for debugging will be logged.

#### NFPC - Number of Fine Channels per Coarse Channel

When populating the header structure, there is one optional field: nfpc.  If the value is unknown, set it to 0.  This will cause the library to omit the nfpc from the output HDF5 header.

#### wrh5_open(context, header, output-path, user-chunking or NULL, user-caching or NULL, debug-flag)

* context : address of a struct defined in wrh5_defs.h that will be initialized by the wrh5_open process.
* header : address of a struct defined in wrh5_defs.h that is populated prior to the call to wrh5_open.
* output-path : O/S path to write the HDF5 file.  Note that any preexisting file is replaced.
* user-chunking : If not NULL, this is the address of a struct defined in wrh5_defs.h which holds the 3-dimensional chunking parameters supplied by the caller.  If not provided (NULL), the default is to provide blimpy-style chunking.
* user-caching : If not NULL, this is the address of a struct defined in wrh5_defs.h which holds the caching parameters supplied by the caller.  If not provided (NULL), the libhdf5 library will provide default caching. See https://portal.hdfgroup.org/display/HDF5/H5P_SET_CACHE for a description of libhdf5 caching and the individual caching fields.
* debug-flag : If set to nonzero, detailed logging is provided.

#### wrh5_write(context, header, buffer-address, buffer-size, debug-flag)

* context : address of the current context struct that was previously initialized by the wrh5_open process.  Note that the context is updated by this function during the processing of the caller's request.
* header : address of the header that was passed in the call to wrh5_open (unchanged).
* buffer-address : contains the data to be written.
* buffer-length : size of the data to be written.
* debug-flag : If set to nonzero, detailed logging is provided.

#### wrh5_close(context-pointer, buffer-address, buffer-size, debug-flag)

* context : address of the current context struct that was initialized by the wrh5_open process and updated during wrh5_write processing.
* debug-flag : If set to nonzero, detailed logging is provided.

### BLIMPY CHUNKING

This is the Green Bank Telescope (GBT) algorithm to calculate the HDF5 chunk dimensions, depending on the perceived file category.  If the user does not provide a chunking parameter structure (NULL), this algorithm is used to provide a default.

Original source code (Python3): [blimpy](https://github.com/UCBerkeleySETI/blimpy) waterfall.py _get_chunk_dimensions().

C source code: wrh5_util.c wrh5_blimpy_chunking().

Process:
* If High Frequency Resolution data i.e. the fine channel offset < 1.0e-5 MHz, then use (1, 1, 1048576)
* else if High Time Resolution data i.e. the time sampling interval is < 1.0e-3 sec, then use (2048, 1, 512)
* else if Intermediate Frequency Resolution data i.e. the fine channel offset is in the interval {1.0e-5 MHz : 1.0e-2 MHz}, then use (10, 1, 65536)
* else use (1, 1, 512)

### SAMPLE APPLICATIONS

See ```simon``` (default chunking and caching) and ```alvin``` (user-specified chunking and caching) in folder ```test```.
