/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * wrh5_write.c                                                                *
 * ------------                                                                *
 * Write a Filterbank HDF5 dump (multiple time integrations).                  *       .                   *
 *                                                                             *
 * HDF 5 library functions used:                                               *
 * - H5Dset_extent        - Define new file size to include this new dump      *
 * - H5Dget_space         - Get a space handle for writing                     *
 * - H5Sselect_hyperslab  - Define hyperslab offset and length in write        *
 * - H5Dwrite             - Write the hyperslab                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "wrh5_defs.h"

/***
	Main entry point.
***/
int wrh5_write(wrh5_context_t * p_wrh5_ctx, 
               wrh5_hdr_t * p_wrh5_hdr, void * p_buffer, 
               size_t bufsize, 
               int debugging) {
    herr_t      status;          // Status from HDF5 function call
    size_t      ntints;          // Number of time integrations in the current dump
    hid_t       filespace_id;    // Identifier for a copy of the dataspace 
    hsize_t     selection[3];    // Current selection
    clock_t     clock_1;         // Debug time measurement
    double      cpu_time_used;   // Debug time measurement

    /*
     * Initialise write loop.
     */
    if(debugging)
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
    ntints = bufsize / p_wrh5_ctx->tint_size;  // Compute the number of time integrations in the current dump.
    p_wrh5_ctx->dump_count += 1;               // Bump the dump count.

    /*
     * Bump the count of time integrations.
     * One was already accounted for at open time - required by HDF5 library.
     * So, 
     *    If this is the very first write, bump by (ntints - 1);
     *    Else, bumpy by ntints.
     */
    if(p_wrh5_ctx->offset_dims[0] > 0)
        p_wrh5_ctx->filesz_dims[0] += ntints;
    else
        p_wrh5_ctx->filesz_dims[0] += (ntints - 1);

    /*
     * Define the current slab selection in terms of its shape.
     */
    selection[0] = ntints;
    selection[1] = p_wrh5_hdr->nifs;
    selection[2] = p_wrh5_hdr->nchans;

    if(debugging) {
        wrh5_info("wrh5_write: dump %ld, offset=(%lld, %lld, %lld), selection=(%lld, %lld, %lld), filesize=(%lld, %lld, %lld)\n",
               p_wrh5_ctx->dump_count,
               p_wrh5_ctx->offset_dims[0], 
               p_wrh5_ctx->offset_dims[1], 
               p_wrh5_ctx->offset_dims[2],
               selection[0], 
               selection[1], 
               selection[2],
               p_wrh5_ctx->filesz_dims[0], 
               p_wrh5_ctx->filesz_dims[1], 
               p_wrh5_ctx->filesz_dims[2]);
        clock_1 = clock();
     }

    /*
     * Extend dataset.
     */
    status = H5Dset_extent(p_wrh5_ctx->dataset_id,    // Dataset handle
                           p_wrh5_ctx->filesz_dims);  // New dataset shape
    if(status < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_write: H5Dset_extent/dataset_id FAILED");
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
        p_wrh5_ctx->usable = 0;
        return 1;
    }

    /*
     * Reset dataspace extent to match current slab selection.
     */
    status = H5Sset_extent_simple(p_wrh5_ctx->dataspace_id, // Dataspace handle
                                  NDIMS,                    // Repeat rank from previous API calls
                                  selection,                // New dataspace size shape
                                  p_wrh5_ctx->filesz_dims); // Max dataspace dimensions
    if(status < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_write: H5Dset_extent/dataset_id FAILED");
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
        p_wrh5_ctx->usable = 0;
        return 1;
    }

    /*
     * Get filespace.
     */
    filespace_id = H5Dget_space(p_wrh5_ctx->dataset_id);    // Dataset handle
    if(filespace_id < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_write: H5Dget_space FAILED");
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
        p_wrh5_ctx->usable = 0;
        return 1;
    }

    /*
     * Select the filespace hyperslab.
     */
    status = H5Sselect_hyperslab(filespace_id,              // Filespace handle
                                 H5S_SELECT_SET,            // Replace preexisting selection
                                 p_wrh5_ctx->offset_dims,   // Starting offset dimensions of first element
                                 NULL,                      // Not "striding"
                                 selection,                 // Selection dimensions
                                 NULL);                     // Block parameter : default value
    if(status < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_write: H5Sselect_hyperslab/filespace FAILED");
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
        p_wrh5_ctx->usable = 0;
        return 1;
    }

    /*
     * Write out current time integration to the hyperslab.
     */
    status = H5Dwrite(p_wrh5_ctx->dataset_id,   // Dataset handle
                      p_wrh5_ctx->elem_type,    // HDF5 element type
                      p_wrh5_ctx->dataspace_id, // Dataspace handle
                      filespace_id,             // Filespace_id
                      H5P_DEFAULT,              // Default data transfer properties
                      p_buffer);                // Buffer holding the data
    if(status < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_write: H5Dwrite FAILED");
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
        p_wrh5_ctx->usable = 0;
        return 1;
    }

    /*
     * Point ahead for the next call to wrh5_write.
     */
    p_wrh5_ctx->offset_dims[0] += ntints;

    /*
     * Close temporary filespace handle.
     */
    status = H5Sclose(filespace_id);
    if(status < 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_close H5Sclose/filespace_id FAILED\n");
        wrh5_show_context("wrh5_write", p_wrh5_ctx);
        p_wrh5_ctx->usable = 0;
        return 1;
    }
    if(debugging) {
        cpu_time_used = ((double) (clock() - clock_1)) / CLOCKS_PER_SEC;
        wrh5_info("wrh5_write: dump %ld E.T. = %.3f s\n", p_wrh5_ctx->dump_count, cpu_time_used);
    }

    /*
     * Bump counters. Mark context active.
     */
    p_wrh5_ctx->byte_count += bufsize;
    p_wrh5_ctx->usable = 1;

    /*
     * Bye-bye.
     */
    return 0;
}
