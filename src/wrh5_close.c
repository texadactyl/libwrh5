/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * wrh5_close.c                                                                *
 * ----------                                                                  *
 * Close an FBH5 writing session:                          .                   *
 * Dataspace, Dataset, and File (in that order).                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "wrh5_defs.h"
#define MILLION 1000000.0


/***
	Main entry point.
***/
int wrh5_close(wrh5_context_t * p_wrh5_ctx, int debugging) {
    herr_t      status;         // Status from HDF5 function call
    hsize_t     sz_store;       // Storage size
    double      MiBstore;       // sz_store converted to MiB
    double      MiBlogical;     // sz_store converted to MiB
    
    // Even if this function fails, mark the fbh5 context unusable.
    p_wrh5_ctx->usable = 0;

    // Compute some stats while the dataset is still open.
    sz_store = H5Dget_storage_size(p_wrh5_ctx->dataset_id);
    MiBlogical = (double) p_wrh5_ctx->tint_size * (double) p_wrh5_ctx->offset_dims[0] / MILLION;
    
    /*
     * Attach "dimension scale" labels.
     */
    wrh5_set_ds_label(p_wrh5_ctx, "time", 0, debugging);
    wrh5_set_ds_label(p_wrh5_ctx, "feed_id", 1, debugging);
    wrh5_set_ds_label(p_wrh5_ctx, "frequency", 2, debugging);

    /*
     * Close dataspace.
     */
    status = H5Sclose(p_wrh5_ctx->dataspace_id);
    if(status != 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_close H5Sclose dataspace FAILED\n");
        wrh5_show_context("wrh5_close", p_wrh5_ctx);
        return 1;
    }
        
    /*
     * Close dataset.
     */
    status = H5Dclose(p_wrh5_ctx->dataset_id);
    if(status != 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_close H5Dclose dataset 'data' FAILED\n");
        wrh5_show_context("wrh5_close", p_wrh5_ctx);
         return 1;
    }
        
    /*
     * Close file.
     */
    status = H5Fclose(p_wrh5_ctx->file_id);
    if(status != 0) {
        wrh5_error(__FILE__, __LINE__, "wrh5_close H5Fclose FAILED\n");
        wrh5_show_context("wrh5_close", p_wrh5_ctx);
        return 1;
    }

    /*
     * Closing statistics.
     */
    if(debugging) {
        wrh5_info("wrh5_close: Context closed.\n");
        wrh5_info("wrh5_close: %ld dumps processed.\n", p_wrh5_ctx->dump_count);
        wrh5_info("wrh5_close: %lld time integrations processed.\n", p_wrh5_ctx->offset_dims[0]);
        MiBstore = (double) sz_store / MILLION;
        wrh5_info("wrh5_close: Compressed %.2f MiB --> %.2f MiB\n", MiBlogical, MiBstore);
    }

    /*
     * Bye-bye.
     */
    return 0;
}
