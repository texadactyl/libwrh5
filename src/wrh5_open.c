/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * wrh5_open.c                                                                 *
 * -----------                                                                 *
 * Begin a Filterbank HDF5 writing session                 .                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "wrh5_defs.h"

/***
	Open-file entry point.
***/
int wrh5_open(wrh5_context_t * p_wrh5_ctx,
              wrh5_hdr_t * p_wrh5_hdr,
              char * output_path,
              user_chunking_t * p_user_chunking,
              user_caching_t * p_user_caching,
              int debugging) {
    hid_t       dcpl;               // Chunking handle - needed until dataset handle is produced
    hsize_t     max_dims[NDIMS];    // Maximum dataset allocation dimensions
    herr_t      status;             // Status from HDF5 function call
    char        msgstr[256];        // sprintf target
    unsigned    hdf5_majnum, hdf5_minnum, hdf5_relnum;  // Version/release info for the HDF5 library

    // Chunking parameters
    hsize_t     cdims[NDIMS];       // Chunking dimensions array
 
    // Caching parameters if user caching is specified; else default libhdf5 caching is used.
    // See libhdf5 function H5Pset_cache().
    hid_t       fapl = -1;          // File access property list identifier
    
    // Bitshuffle plugin status:
    int         bitshuffle_available = 0;     // Bitshuffle availability: 1=yes, 0=no

    // Bitshuffle options
    // Ref: def __init__ in class Bitshuffle in https://github.com/silx-kit/hdf5plugin/blob/main/src/hdf5plugin/__init__.py
    // Mimicing blimpy in https://github.com/UCBerkeleySETI/blimpy/blob/master/blimpy/io/hdf_writer.py
    unsigned bitshuffle_opts[] = {0, 2}; // nelems = 0, 2 --> LZ4

    // Clear context.
    memset(p_wrh5_ctx, 0, (size_t) sizeof(wrh5_context_t));

    /*
     * Check whether or not the Bitshuffle filter is available.
     */
    if (H5Zfilter_avail(FILTER_ID_BITSHUFFLE) <= 0)
        wrh5_warning(__FILE__, __LINE__, "fbhf_open: Plugin bitshuffle is NOT available; data will not be compressed");
    else {
        bitshuffle_available = 1;
    }
    
    /*
     * Validate wrh5_hdr: nifs, nbits, nfpc, nchans.
     */
    if((p_wrh5_hdr->nbits % 8 != 0) || (p_wrh5_hdr->nbits < 8) || (p_wrh5_hdr->nbits > 64)) {
        sprintf(msgstr, "wrh5_open: nbits must be in [8, 16, 32, 64] but I saw %d", p_wrh5_hdr->nifs);
        wrh5_error(__FILE__, __LINE__, msgstr);
        return 1;
    }
    if((p_wrh5_hdr->nifs < 1) || (p_wrh5_hdr->nifs > 4)) {
        sprintf(msgstr, "wrh5_open: nifs must be in [1, 2, 3, 4] but I saw %d", p_wrh5_hdr->nifs);
        wrh5_error(__FILE__, __LINE__, msgstr);
        return 1;
    }
    if(p_wrh5_hdr->nfpc < 0) {
        sprintf(msgstr, "wrh5_open: nfpc must be > -1 but I saw %d", p_wrh5_hdr->nfpc);
        wrh5_error(__FILE__, __LINE__, msgstr);
        return 1;
    }
    if(p_wrh5_hdr->nfpc > 0) {
        if(p_wrh5_hdr->nchans % p_wrh5_hdr->nfpc != 0) {
            sprintf(msgstr, "wrh5_open: nchans must be divisible by nfpc but I saw %d / %d", p_wrh5_hdr->nchans, p_wrh5_hdr->nfpc);
            wrh5_error(__FILE__, __LINE__, msgstr);
            return 1;
        }
    }
        
    /*
     * Initialize FBH5 context.
     */
    memset(p_wrh5_ctx, 0, sizeof(wrh5_context_t));
    p_wrh5_ctx->elem_size = p_wrh5_hdr->nbits / 8;
    p_wrh5_ctx->tint_size = p_wrh5_hdr->nifs * p_wrh5_hdr->nchans * p_wrh5_ctx->elem_size;
    p_wrh5_ctx->offset_dims[0] = 0;
    p_wrh5_ctx->offset_dims[1] = 0;
    p_wrh5_ctx->offset_dims[2] = 0;
    
    /*
     * Open HDF5 file.  Overwrite it if preexisting.
     */
    p_wrh5_ctx->file_id = H5Fcreate(output_path,    // Full path of output file
                                    H5F_ACC_TRUNC,  // Overwrite if preexisting.
                                    H5P_DEFAULT,    // Default creation property list 
                                    H5P_DEFAULT);   // Default access property list
    if(p_wrh5_ctx->file_id < 0) {
        sprintf(msgstr, "wrh5_open: H5Fcreate of '%s' FAILED", output_path);
        wrh5_error(__FILE__, __LINE__, msgstr);
        return 1;
    }

    /*
     * Write blimpy-required file-level metadata attributes.
     */
    wrh5_set_str_attr(p_wrh5_ctx->file_id, 
                      "CLASS", 
                      FILTERBANK_CLASS, 
                      debugging);
    wrh5_set_str_attr(p_wrh5_ctx->file_id, 
                      "VERSION", 
                      FILTERBANK_VERSION, 
                      debugging);

    /*
     * Get software versions and store them as file-level attributes.
     */
    wrh5_set_str_attr(p_wrh5_ctx->file_id, 
                      "LIBWRH5", 
                      VERSION_WRH5, 
                      debugging);
    H5get_libversion(&hdf5_majnum, &hdf5_minnum, &hdf5_relnum);
    sprintf(msgstr, "%d.%d.%d", hdf5_majnum, hdf5_minnum, hdf5_relnum);
    wrh5_set_str_attr(p_wrh5_ctx->file_id, 
                      "LIBHDF5", 
                      msgstr, 
                      debugging);
    
    /*
     * Store bitshuffle availability as a file-level attribute.
     */
    if(bitshuffle_available > 0)
        strcpy(msgstr, "ENABLED");
    else
        strcpy(msgstr, "DISABLED");
    wrh5_set_str_attr(p_wrh5_ctx->file_id, 
                      "BITSHUFFLE", 
                      msgstr, 
                      debugging);

    /*
     * Initialise the total file size in terms of its shape.
     */
    p_wrh5_ctx->filesz_dims[0] = 1;
    p_wrh5_ctx->filesz_dims[1] = p_wrh5_hdr->nifs;
    p_wrh5_ctx->filesz_dims[2] = p_wrh5_hdr->nchans;
    
    /*
     * Set the maximum file size in terms of its shape.
     */
    max_dims[0] = H5S_UNLIMITED;
    max_dims[1] = p_wrh5_hdr->nifs;
    max_dims[2] = p_wrh5_hdr->nchans;

    /*
     * Create a dataspace which is extensible in the time dimension.
     */
    p_wrh5_ctx->dataspace_id = H5Screate_simple(NDIMS,                           // Rank
                                                p_wrh5_ctx->filesz_dims, // initial dimensions
                                                max_dims);               // maximum dimensions
    if(p_wrh5_ctx->dataspace_id < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_open: H5Screate_simple FAILED");
        return 1;
    }
    
    /*
     * Initialise the dataset creation property list
     */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    if(dcpl < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_open: H5Pcreate/dcpl FAILED");
        return 1;
    }
         
    /*
     * Add chunking to the dataset creation property list.
     */
    if(p_user_chunking == NULL) {
        if(debugging)
            wrh5_info("Default chunking requested (blimpy)\n");
        wrh5_blimpy_chunking(p_wrh5_hdr, &cdims[0]);
    } else {
        // User supplied chunk dimensions
        cdims[0] = p_user_chunking->n_time;
        cdims[1] = p_user_chunking->n_nifs;
        cdims[2] = p_user_chunking->n_fine_chan;
    }
    status = H5Pset_chunk(dcpl, NDIMS, cdims);
    if(status != 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_open: H5Pset_chunk FAILED");
        return 1;
    }
    if(debugging)
        wrh5_info("Chunk dimensions = (%lld, %lld, %lld)\n", cdims[0], cdims[1], cdims[2]);

    /*
     * Add the Bitshuffle and LZ4 filters to the dataset creation property list.
     */
    if(bitshuffle_available) {
        status = H5Pset_filter(dcpl, FILTER_ID_BITSHUFFLE, H5Z_FLAG_MANDATORY, 2, bitshuffle_opts); // Bitshuffle Filter
        if(status < 0)
            wrh5_warning(__FILE__, __LINE__, "wrh5_open: H5Pset_filter FAILED; data will not be compressed");
    }
    
    /* 
     * Define datatype for the data in the file.
     * We will store little endian values.
     */
    switch(p_wrh5_hdr->nbits) {
        case 8:
            p_wrh5_ctx->elem_type = H5T_NATIVE_B8;
            break;
        case 16:
            p_wrh5_ctx->elem_type = H5T_NATIVE_B16;
            break;
        case 32:
            p_wrh5_ctx->elem_type = H5T_IEEE_F32LE;
            break;
        default: // 64
            p_wrh5_ctx->elem_type = H5T_IEEE_F64LE;
    }

    /*
     * Create the dataset.
     */
    p_wrh5_ctx->dataset_id = H5Dcreate(p_wrh5_ctx->file_id,       // File handle
                                       DATASETNAME,               // Dataset name
                                       p_wrh5_ctx->elem_type,     // HDF5 data type
                                       p_wrh5_ctx->dataspace_id,  // Dataspace handle
                                       H5P_DEFAULT,               // 
                                       dcpl,                      // Dataset creation property list
                                       H5P_DEFAULT);              // Default access properties
    if(p_wrh5_ctx->dataset_id < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_open: H5Dcreate FAILED");
        return 1;
    }

    /*
     * Specify file-level caching.
     */
    if(p_user_caching == NULL) {
        if(debugging)
            wrh5_info("Default libhdf5 caching requested\n");
    } else { // User caching specified
        if(debugging)
            wrh5_info("User libhdf5 caching: nslots=%d, nbytes=%d, policy=%f",
                      p_user_caching->nslots, p_user_caching->nbytes, p_user_caching->policy);
        fapl = H5Fget_access_plist(p_wrh5_ctx->file_id);
        if(fapl < 0) {
            wrh5_warning(__FILE__, __LINE__, "wrh5_open: H5Fget_access_plist FAILED; using default caching");
            fapl = -1;
        } else {
            // https://portal.hdfgroup.org/display/HDF5/H5P_SET_CACHE
            status = H5Pset_cache(fapl, 
                                  0,                         // "nelmts" is ignored
                                  p_user_caching->nslots,    // Hash table slot count
                                  p_user_caching->nbytes,    // File cache size in bytes
                                  p_user_caching->policy);   // Cache preemption policy
            if(status < 0)
                wrh5_warning(__FILE__, __LINE__, "wrh5_open: H5Pset_cache FAILED; hopefully, default caching is being used");
        }
    }
 
    /*
     * Close dcpl handle.
     */
    status = H5Pclose(dcpl);
    if(status != 0)
        wrh5_warning(__FILE__, __LINE__, "wrh5_open: H5Pclose/dcpl FAILED; ignored\n");
    
    /*
     * Close fapl handle if it was opened.
     */
    if(!(fapl < 0)) {
        status = H5Pclose(fapl);
        if(status != 0)
            wrh5_warning(__FILE__, __LINE__, "wrh5_open: H5Pclose/fapl FAILED; ignored\n");
    }

    /*
     * Write dataset metadata attributes.
     */
    wrh5_write_metadata(p_wrh5_ctx->dataset_id, // Dataset handle
                        p_wrh5_hdr,               // Metadata (SIGPROC header)
                        debugging);        // Tracing flag

    /*
     * Bye-bye.
     */
    p_wrh5_ctx->usable = 1;
    if(debugging)
        wrh5_show_context("wrh5_open", p_wrh5_ctx);
    return 0;

}
