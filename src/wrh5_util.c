/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * wrh5_util.c                                                                 *
 * -----------                                                                 *
 * Utility functions                                       .                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "wrh5_defs.h"

#define LEN_TIMESTAMP 22


/***
    Get a timestamp.
***/
void get_timestamp(char * buffer) {
    time_t time_t_time;
    struct tm * tm_struct;

    time(&time_t_time);
    tm_struct = localtime(&time_t_time);
    strftime(buffer, LEN_TIMESTAMP, "%Y-%m-%d_%H:%M:%S ", tm_struct);
}


/***
	Report information.
***/
void wrh5_info(const char *format, ...) {
    char buffer[256];
    va_list va_array;
	va_start(va_array, format);
	get_timestamp(buffer);
	strcat(buffer, format);
	vfprintf(stdout, buffer, va_array);
	va_end(va_array);
}


/***
	Report bad news as a warning message.
***/
void wrh5_warning(char * srcfile, int linenum, char * msg) {
    char timestamp[LEN_TIMESTAMP];
    
    get_timestamp(timestamp);
	fprintf(stderr, "%sWRH5-WARNING %s line %d :: %s\n", timestamp, srcfile, linenum, msg);
}


/***
	Report bad news as an error message.
***/
void wrh5_error(char * srcfile, int linenum, char * msg) {
    char timestamp[LEN_TIMESTAMP];
    
    get_timestamp(timestamp);
	fprintf(stderr, "%sWRH5-ERROR %s line %d :: %s\n", timestamp, srcfile, linenum, msg);
}


/***
	Set a file-level or dataset-level attribute in string-value format.
***/
void wrh5_set_str_attr(hid_t file_or_dataset_id, char * tag, char * p_value, int debugging) {
    herr_t status;
    hid_t id_scalar, atype, id_attr;
    char warning[256];
   
    if(debugging)
        wrh5_info("wrh5_set_str_attr: %s = %s\n", tag, p_value);
    id_scalar  = H5Screate(H5S_SCALAR);
    atype = H5Tcopy(H5T_C_S1);  // attr type = string
    H5Tset_size(atype, strlen(p_value));  // set create the string
    H5Tset_strpad(atype, H5T_STR_NULLTERM);
    id_attr = H5Acreate(file_or_dataset_id, tag, atype, id_scalar, H5P_DEFAULT, H5P_DEFAULT);
    if(id_attr < 0) {
        sprintf(warning, "wrh5_set_str_attr/H5Acreate FAILED, key=%s, value=%s", tag, p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
        return;
    }
    status = H5Awrite(id_attr, atype, p_value); 
    if(status != 0) {
        sprintf(warning, "wrh5_set_str_attr/H5Awrite FAILED, key=%s, value=%s", tag, p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
    }
    status = H5Aclose(id_attr);    
    if(status != 0) {
        sprintf(warning, "wrh5_set_str_attr/H5Aclose FAILED, key=%s, value=%s", tag, p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
    }
}


/***
	Set a dataset-level attribute in double-value format.
***/
void wrh5_set_dataset_double_attr(hid_t dataset_id, char * tag, double * p_value, int debugging) {
    herr_t status;
    hid_t id_attr, id_scalar;
    char warning[256];
    
    if(debugging)
        wrh5_info("wrh5_set_dataset_double_attr: %s = %f\n", tag, *p_value);
    id_scalar  = H5Screate(H5S_SCALAR);
    if(id_scalar < 0) {
        sprintf(warning, "wrh5_set_dataset_double_attr/H5Screate FAILED, key=%s, value=%f", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
        return;
    }
    id_attr = H5Acreate2(dataset_id, tag, H5T_NATIVE_DOUBLE, id_scalar, H5P_DEFAULT, H5P_DEFAULT);
    if(id_attr < 0) {
        sprintf(warning, "wrh5_set_dataset_double_attr/H5Acreate2 FAILED, key=%s, value=%f", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
        return;
    }
    status = H5Awrite(id_attr, H5T_NATIVE_DOUBLE, p_value);
    if(status < 0) {
        sprintf(warning, "wrh5_set_dataset_double_attr/H5Awrite FAILED, key=%s, value=%f", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
    }
    status = H5Aclose(id_attr);
    if(status < 0) {
        sprintf(warning, "wrh5_set_dataset_double_attr/H5Aclose FAILED, key=%s, value=%f", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
    }
}


/***
	Set a dataset-level attribute in int-value format.
***/
void wrh5_set_dataset_int_attr(hid_t dataset_id, char * tag, int * p_value, int debugging) {
    herr_t status;
    hid_t id_attr, id_scalar;
    char warning[256];
    
    if(debugging)
        wrh5_info("wrh5_set_dataset_int_attr: %s = %d\n", tag, *p_value);
    id_scalar  = H5Screate(H5S_SCALAR);
    if(id_scalar < 0) {
        sprintf(warning, "wrh5_set_dataset_int_attr/H5Screate FAILED, key=%s, value=%d", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
        return;
    }
    id_attr = H5Acreate2(dataset_id, tag, H5T_NATIVE_INT, id_scalar, H5P_DEFAULT, H5P_DEFAULT);
    if(id_attr < 0) {
        sprintf(warning, "wrh5_set_dataset_int_attr/H5Acreate2 FAILED, key=%s, value=%d", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
        return;
    }
    status = H5Awrite(id_attr, H5T_NATIVE_INT, p_value);
    if(status != 0) {
        sprintf(warning, "wrh5_set_dataset_int_attr/H5Awrite FAILED, key=%s, value=%d", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
    }
    status = H5Aclose(id_attr);
    if(status != 0) {
        sprintf(warning, "wrh5_set_dataset_int_attr/H5Aclose FAILED, key=%s, value=%d", tag, *p_value);
        wrh5_warning(__FILE__, __LINE__, warning);
    }
}


/***
	Write metadata to FBH5 file dataset.
***/
void wrh5_write_metadata(hid_t dataset_id, wrh5_hdr_t *p_md, int debugging) {
    wrh5_set_dataset_int_attr(dataset_id, "machine_id", &(p_md->machine_id), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "telescope_id", &(p_md->telescope_id), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "data_type", &(p_md->data_type), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "nchans", &(p_md->nchans), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "nfpc", &(p_md->nfpc), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "nbeams", &(p_md->nbeams), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "ibeam", &(p_md->ibeam), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "nbits", &(p_md->nbits), debugging);
    wrh5_set_dataset_int_attr(dataset_id, "nifs", &(p_md->nifs), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "src_raj", &(p_md->src_raj), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "src_dej", &(p_md->src_dej), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "az_start", &(p_md->az_start), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "za_start", &(p_md->za_start), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "fch1", &(p_md->fch1), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "foff", &(p_md->foff), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "tstart", &(p_md->tstart), debugging);
    wrh5_set_dataset_double_attr(dataset_id, "tsamp", &(p_md->tsamp), debugging);
    wrh5_set_str_attr(dataset_id, "source_name", &(p_md->source_name[0]), debugging);
    wrh5_set_str_attr(dataset_id, "rawdatafile", &(p_md->rawdatafile[0]), debugging);
}

/***
	Set up a dimension scale label.
	
	* Create a secondary dataset, dscale_id.
	* Attach dscale_id to the file's primary dataset as a Dimension Scale label.
***/
void wrh5_set_ds_label(wrh5_context_t * p_wrh5_ctx, char * label, int dims_index, int debugging) {
    herr_t status;
    char wstr[256];

    if(debugging)
        wrh5_info("wrh5_set_ds_label: label = %s, dims_index = %d\n", label, dims_index);
    status = H5DSset_label(p_wrh5_ctx->dataset_id,  // Dataset ID
                              dims_index,           // Dimension index to which dscale_id applies to
                              label);               // Label
    if(status < 0) {
        sprintf(wstr, "wrh5_set_ds_label/H5DSset_label FAILED (%s)", label);
        wrh5_warning(__FILE__, __LINE__, wstr);
    }
}


/***
	Display some of the wrh5_context values.
***/
void wrh5_show_context(char * caller, wrh5_context_t * p_wrh5_ctx) {
    if(p_wrh5_ctx == NULL) {
        wrh5_info("*** wrh5_show_context: p_wrh5_ctx = NULL !!!");
        return;
    }
    wrh5_info("wrh5_show_context(%s): active = %d\n", caller, p_wrh5_ctx->usable);
    wrh5_info("wrh5_show_context(%s): elem_size = %d\n", caller, p_wrh5_ctx->elem_size);
    wrh5_info("wrh5_show_context(%s): tint_size = %ld\n", caller, p_wrh5_ctx->tint_size);
    wrh5_info("wrh5_show_context(%s): offset_dims = (%lld, %lld, %lld)\n",
           caller, p_wrh5_ctx->offset_dims[0], p_wrh5_ctx->offset_dims[1], p_wrh5_ctx->offset_dims[2]);
    wrh5_info("wrh5_show_context(%s): filesz_dims = (%lld, %lld, %lld)\n",
           caller, p_wrh5_ctx->filesz_dims[0], p_wrh5_ctx->filesz_dims[1], p_wrh5_ctx->filesz_dims[2]);
    wrh5_info("wrh5_show_context(%s): byte_count = %ld\n", caller, p_wrh5_ctx->byte_count);
    wrh5_info("wrh5_show_context(%s): dump_count = %ld\n", caller, p_wrh5_ctx->dump_count);
}


/***
    Algorithm (GBT) to calculate the chunk dimensions depending on the file type.
    Python3 source: blimpy waterfall.py _get_chunk_dimensions()

    * High frequency resolution (HFR)   --> (1,1,1048576)
    * High time resolution (HTR)        --> (2048,1,512)
    * Intermediate frequency resolution --> (10,1,65536)
    * None of the above ------------------> (1,1,512)
***/
void wrh5_blimpy_chunking(wrh5_hdr_t * p_wrh5_hdr, hsize_t * p_cdims) {

        // GBT: '.0000.' is HFR
        // GBT: 1048576 is the number of channels in a coarse channel.
        if(p_wrh5_hdr->foff < 1.0e-5) {
            *p_cdims       = 1;
            *(p_cdims + 1) = 1;
            *(p_cdims + 2) = 1048576;
            if(p_wrh5_hdr->nchans < 1048576)
                *(p_cdims + 2) = p_wrh5_hdr->nchans;
            return;
        }

        // GBT: .0001. is HTR
        // GBT: 512 is the total number of channels per single band
        if(p_wrh5_hdr->tsamp < 1.0e-3) {
            *p_cdims       = 2048;
            *(p_cdims + 1) = 1;
            *(p_cdims + 2) = 512;
            if(p_wrh5_hdr->nchans < 512)
                *(p_cdims + 2) = p_wrh5_hdr->nchans;
            return;
        }

        // GBT: .0002. is intermediate
        // GBT: 65536 is the total number of channels per single band
        if(p_wrh5_hdr->foff < 1.0e-2 && p_wrh5_hdr->foff  >= 1.0e-5) {
            *p_cdims       = 10;
            *(p_cdims + 1) = 1;
            *(p_cdims + 2) = 65536;
            if(p_wrh5_hdr->nchans < 65536)
                *(p_cdims + 2) = p_wrh5_hdr->nchans;
            return;
        }

        // None of the above.
        *p_cdims       = 1;
        *(p_cdims + 1) = 1;
        *(p_cdims + 2) = 512;
        if(p_wrh5_hdr->nchans < 512)
            *(p_cdims + 2) = p_wrh5_hdr->nchans;
}
