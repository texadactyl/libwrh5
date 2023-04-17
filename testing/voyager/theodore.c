/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * theodore.c                                                           *
 * ----------                                                                  *
 * Write a Voyager 1 HDF5 file all in one go.              .                   *
 *                                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "theodore.h"


/***
	Report bad news and exit to O/S
***/
void oops(int linenum, char *msg) {
	fprintf(stderr, "\n*** OOPS, theodore: fatal error detected at line %d !!!\n%s\n\n", linenum, msg);
	exit(1);
}


/***
	Main entry point
***/
int main(int argc, const char **argv) {

    size_t sz_alloc = 0;	// size of data matrix to allocate from the heap
    char wstr[256];			// sprintf target
    float * p_data;			// pointer to allocated heap
    time_t time1, time2;	// elapsed time calculation (seconds)
    char path_in_hdr[100];	// path of input hdr file
    char path_in_data[100];	// path of input data file
    char path_out_h5[100];	// path of output h5 file
    wrh5_hdr_t header;		// Filterbank header struct
    wrh5_context_t ctx;		// libwrh5 context
    int debugging = 0;		// libwrh5 debugging
    
    if(argc != 5) {
    	printf("\n argc = %d\n", argc);
    	printf("Usage:  %s  {input-hdr-file}  {input-data-file}  {output-h5-file}   {debugging = 0 or 1}\n\n", argv[0]);
    	return 1;
    }

    /*
     * Collect command line arguments.
     */
    printf("DEBUG theodore: begin\n");
    strncpy(path_in_hdr, argv[1], sizeof path_in_hdr);
    strncpy(path_in_data, argv[2], sizeof path_in_data);
    strncpy(path_out_h5, argv[3], sizeof path_out_h5);
    debugging = atoi(argv[4]);
    printf("theodore: Header file = %s\n", path_in_hdr);
    printf("theodore: Data file = %s\n", path_in_data);
    printf("theodore: HDF5 file = %s\n", path_out_h5);

    /*
     * Get header struct.
     */
    FILE *file_in_hdr;
    int count;
    char key[80], value[80];
    file_in_hdr = fopen(path_in_hdr, "r");
    if(file_in_hdr == NULL) {
        sprintf(wstr, "Cannot open %s !\n", path_in_hdr);
        oops(__LINE__, wstr);
    }
    printf("theodore: Scanning metadata file .....\n");
    while(1) {
        count = fscanf(file_in_hdr, "%s %s", key, value);
        if(count != 2)
            break;
        HDRFLD_INT(machine_id, "machine_id")
        HDRFLD_INT(telescope_id, "telescope_id")
        HDRFLD_INT(data_type, "data_type")
        HDRFLD_INT(barycentric, "barycentric")
        HDRFLD_INT(pulsarcentric, "pulsarcentric")
        HDRFLD_DBL(src_raj, "src_raj")
        HDRFLD_DBL(src_dej, "src_dej")
        HDRFLD_DBL(az_start, "az_start")
        HDRFLD_DBL(za_start, "za_start")
        HDRFLD_DBL(fch1, "fch1")
        HDRFLD_DBL(foff, "foff")
        HDRFLD_INT(nchans, "nchans")
        HDRFLD_INT(nbeams, "nbeams")
        HDRFLD_INT(ibeam, "ibeam")
        HDRFLD_INT(nbits, "nbits")
        HDRFLD_DBL(tstart, "tstart")
        HDRFLD_DBL(tsamp, "tsamp")
        HDRFLD_INT(nifs, "nifs")
        HDRFLD_STR(rawdatafile, "rawdatafile")
        HDRFLD_STR(source_name, "source_name")
    }
    fclose(file_in_hdr);
    header.nfpc = 0;
    printf("theodore: Header file processing completed\n");

    /*
     * Compute the byte size of the input data file.
     */
    FILE *file_in_data;
    file_in_data = fopen(path_in_data, "rb");
    if(file_in_data == NULL) {
        sprintf(wstr, "Cannot fopen %s !\n", path_in_data);
        oops(__LINE__, wstr);
    }
    if(fseek(file_in_data, 0, SEEK_END)) {
        sprintf(wstr, "Cannot fseek/SEEK_END %s !\n", path_in_data);
        oops(__LINE__, wstr);
    }
    sz_alloc = ftell(file_in_data);

    /*
     * Allocate enough heap for the entire data matrix.
     */
    if(fseek(file_in_data, 0, SEEK_SET)) {
        sprintf(wstr, "Cannot ftell/SEEK_SET %s !\n", path_in_data);
        oops(__LINE__, wstr);
    }
    p_data = malloc(sz_alloc);
    if(p_data == NULL) {
        sprintf(wstr, "main malloc(%ld) FAILED", (long)sz_alloc);
        oops(__LINE__, wstr);
    }
    printf("theodore: Data space allocated of size  = %ld\n", (long) sz_alloc);

    /*
     * Get spectra data.
     */
    int nbytes = fread(p_data, 1, sz_alloc, file_in_data);
    if(nbytes != sz_alloc) {
    	free(p_data);
        sprintf(wstr, "Cannot read a full allocation from %s !\n\tnbytes = %d, sz_alloc=%ld\n", 
                      path_in_data, nbytes, sz_alloc);
        oops(__LINE__, wstr);
    }
    fclose(file_in_data);
    printf("theodore: Data file processing completed\n");

    /*
     * Open FBH5 session.
     */
    time(&time1);
    wrh5_open(&ctx, &header, path_out_h5, NULL, NULL, debugging);

    /*
     * Write data.
     */
    wrh5_write(&ctx, &header, p_data, sz_alloc, debugging);

    /*
     * Close FBH5 session.
     */
    wrh5_close(&ctx, debugging);

    /*
     * Compute elapsed time.    
     */
    time(&time2);
    free(p_data);
    printf("theodore: End, e.t. = %.2f seconds.\n", difftime(time2, time1));
 	        
    /*
     * Bye-bye.
     */
    return 0;
}

