/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * wrh5_defs.h                                                                 *
 * -----------                                                                 *
 * Global Definitions       .                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef wrh5_DEFS_H
#define wrh5_DEFS_H

/*
 * O/S library definitions
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

/*
 * HDF5 library definitions
 */
#include "hdf5.h"
#include "H5pubconf.h"
#include "H5DSpublic.h"
#ifndef H5_HAVE_THREADSAFE
#error The installed HDF5 run-time is not thread-safe!
#endif

/*
 * HDF5 library ID of the Bitshuffle filter.
 */
#define FILTER_ID_BITSHUFFLE 32008

/*
 * Global definitions
 */
#include "wrh5_version.h"

/*
 * Global definitions
 */
#define DATASETNAME         "data"          // wrh5 dataset name to hold the data matrix
#define NDIMS               3               // # of data matrix dimensions (rank)
#define FILTERBANK_CLASS    "FILTERBANK"    // File-level attribute "CLASS"
#define FILTERBANK_VERSION  "2.0"           // File-level attribute "VERSION"

/*
 * Context definition
 */
typedef struct {
    hid_t file_id;              // File-level handle (similar to an fd)
    hid_t dataset_id;           // Dataset "data" handle
    hid_t dataspace_id;         // Dataspace handle for dataset "data"
    unsigned int elem_size;     // Byte size of one spectra element (E.g. 4 if nbits=32)
    hid_t elem_type;            // HDF5 type for all elements (derived from nbits in wrh5_open)
    size_t tint_size;           // Size of a time integration (computed in wrh5_open)
    hsize_t offset_dims[3];     // Next offset dimensions for the wrh5_write function
                                // (offset_dims[0] : time integration count)
    hsize_t filesz_dims[3];     // Accumulated file size in dimensions
    unsigned long byte_count;   // Number of bytes output so far
    unsigned long dump_count;   // Number of dumps processed so far
    int usable;                 // writes permitted: 1 (normal), else: 0 (an error occured or closed)
} wrh5_context_t;

/*
 * Header (metadata) definition
 */
typedef struct {
  // 0=fake data; 1=Arecibo; 2=Ooty... others to be added
  int machine_id;
  // 0=FAKE; 1=PSPM; 2=WAPP; 3=OOTY... others to be added
  int telescope_id;
  // 1=filterbank; 2=time series... others to be added
  int data_type;
  // 1 if barycentric or 0 otherwise (only output if non-zero)
  int barycentric;
  // 1 if pulsarcentric or 0 otherwise (only output if non-zero)
  int pulsarcentric;
  // right ascension (J2000) of source (hours)
  // will be converted to/from hhmmss.s
  double src_raj;
  // declination (J2000) of source (degrees)
  // will be converted to/from ddmmss.s
  double src_dej;
  // telescope azimuth at start of scan (degrees)
  double az_start;
  // telescope zenith angle at start of scan (degrees)
  double za_start;
  // centre frequency (MHz) of first filterbank channel
  double fch1;
  // filterbank channel bandwidth (MHz)
  double foff;
  // number of fine filterbank channels (not coarse channels)
  int nchans;
  // total number of beams
  int nbeams;
  // total number of beams
  int ibeam;
  // number of bits per time sample
  int nbits;
  // time stamp (MJD) of first sample
  double tstart;
  // time interval between samples (s)
  double tsamp;
  // number of seperate IF channels
  int nifs;
  // the name of the source being observed by the telescope
  char source_name[81];
  // the name of the original data file
  char rawdatafile[81];
  // Number of fine channels per coarse channel
  // If unknown, set nfpc=0.  This will cause the output header to omit this field.
  int nfpc;
  
} wrh5_hdr_t;

/*
 * Optional user chunking definition
 * If not supplied (NULL) by caller in wrh5_open, default chunking (blimpy) is used.
 */
typedef struct {
    size_t  n_time;       // chunk time dimension
    size_t  n_nifs;       // chunk nifs dimension
    size_t  n_fine_chan;  // chunk fine channel dimension
} user_chunking_t;

/*
 * Optional user caching definition - see reference for H5Pset_cache().
 * If not supplied (NULL) by caller in wrh5_open, default caching is used.
 */
typedef struct {
    size_t  nslots;   // Hash table slot count
    size_t  nbytes;   // File cache size in bytes
    double  policy;   // Preemptive policy
} user_caching_t;

/*
 * libwrh5 caller API functions
 */
int     wrh5_open(wrh5_context_t * p_wrh5_ctx,
			      wrh5_hdr_t * p_wrh5_hdr, 
			      char * output_path, 
                  user_chunking_t * p_user_chunking, 
                  user_caching_t * p_user_caching, 
                  int flag_debug);
int     wrh5_write(wrh5_context_t * p_wrh5_ctx,
                   wrh5_hdr_t * p_wrh5_hdr, 
                   void * buffer, 
                   size_t bufsize, 
                   int flag_debug);
int     wrh5_close(wrh5_context_t * p_wrh5_ctx, 
                   int flag_debug);

/*
 * wrh5_util.c functions
 */
void    wrh5_info(const char * format, ...);
void    wrh5_warning(char * srcfile, int linenum, char * msg);
void    wrh5_error(char * srcfile, int linenum, char * msg);
void    wrh5_set_str_attr(hid_t file_or_dataset_id, char * tag, char * value, int flag_debug);
void    wrh5_set_dataset_double_attr(hid_t dataset_id, char * tag, double * p_value, int flag_debug);
void    wrh5_set_dataset_int_attr(hid_t dataset_id, char * tag, int * p_value, int flag_debug);
void    wrh5_write_metadata(hid_t dataset_id, wrh5_hdr_t * p_metadata, int flag_debug);
void    wrh5_set_ds_label(wrh5_context_t * p_wrh5_ctx, char * label, int dims_index, int flag_debug);
void    wrh5_show_context(char * caller, wrh5_context_t * p_wrh5_ctx);
void    wrh5_blimpy_chunking(wrh5_hdr_t * p_wrh5_hdr, hsize_t * p_cdims);

// This stringification trick is from "info cpp"
// See https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define STRINGIFY1(s) #s
#define STRINGIFY(s) STRINGIFY1(s)

#endif

