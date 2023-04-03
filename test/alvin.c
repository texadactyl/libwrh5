/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * alvin.c                                                                     *
 * -------                                                                     *
 * Sample wrh5 application.                                                    *
 * User-specified caching and chunking parameters.                             *
 * nfpc = 0.                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <wrh5_defs.h>

#define NBITS           32
#define NCHANS          1048576
#define NIFS            1
#define NTINTS          16
#define NSPECTRA_PER_DUMP 1


/***
	Initialize metadata to Voyager 1 values.
***/
void make_voyager_1_metadata(wrh5_hdr_t * p_wrh5_hdr) {
    memset(p_wrh5_hdr, 0, sizeof(wrh5_hdr_t));    
    p_wrh5_hdr->az_start = 0.0;
    p_wrh5_hdr->data_type = 1;
    p_wrh5_hdr->fch1 = 8421.386717353016;       // MHz
    p_wrh5_hdr->foff = -2.7939677238464355e-06; // MHz
    p_wrh5_hdr->ibeam = 1;
    p_wrh5_hdr->machine_id = 42;
    p_wrh5_hdr->nbeams = 1;
    p_wrh5_hdr->nchans = NCHANS;            // # of fine channels
    p_wrh5_hdr->nfpc = 0;                   // unknown # of fine channels per coarse channel
    p_wrh5_hdr->nifs = NIFS;                // # of feeds (E.g. polarisations)
    p_wrh5_hdr->nbits = 32;                 // 4 bytes i.e. float32
    p_wrh5_hdr->src_raj = 171003.984;       // 17:12:40.481
    p_wrh5_hdr->src_dej = 121058.8;         // 12:24:13.614
    p_wrh5_hdr->telescope_id = 6;           // GBT
    p_wrh5_hdr->tsamp = 18.253611008;       // seconds
    p_wrh5_hdr->tstart = 57650.78209490741; // 2020-07-16T22:13:56.000
    p_wrh5_hdr->za_start = 0.0;

    strcpy(p_wrh5_hdr->source_name, "Voyager1");
    strcpy(p_wrh5_hdr->rawdatafile, "guppi_57650_67573_Voyager1_0002.0000.raw");
}


void fatal_error(int linenum, char * msg) {
    fprintf(stderr, "\n*** alvin: FATAL ERROR at line %d :: %s.\n", linenum, msg);
    exit(86);
}


/***
	Show help and then exit.
***/
void show_help(char * msg) {
    printf("\n%s\n", msg);
    printf("Usage:  alvin  [-v]  OutputHDF5File\n\n-v : verbose logging\n\n");
    exit(1);
}

/***
	Get a random float between low and high.
***/
float get_random(float low, float high) { 
    float wk = ((float)rand() / (float)RAND_MAX); 
    return low + wk * (high - low);
}

/***
	Main entry point.
***/
int main(int argc, char **argv) {

    char            path_h5[256];   // Output path
    int             verbose = -1;   // 1 : verbose logging in libwrh5 calls; 0 : default
    long            itime, jfreq;   // Loop controls for dummy spectra creation
    size_t          sz_alloc = 0;   // size of data matrix to allocate from the heap
    char            wstr[256];      // sprintf target
    float           *p_data;        // pointer to allocated heap
    float           *wfptr;         // working float pointer
    time_t          time1, time2;   // elapsed time calculation (seconds)

    // Data generation variables.
    float           low = 4.0e9, high = 9.0e9; // Element value boundaries
    unsigned long   count_elems;        // Elemount count
    wrh5_context_t  wrh5_ctx;           // wrh5 context
    wrh5_hdr_t      wrh5_hdr;           // wrh5 header
    user_chunking_t chunking;           // user chunking
    user_caching_t  caching;            // user caching

    /*
     * Parse command line.
     */
    switch(argc) {
        case 1:
            show_help("No parameters provided");
        case 2:
            strcpy(wstr, *++argv);
            if(strcmp(wstr, "-h") == 0)
                show_help("Help was requested");
            if(wstr[0] == '-')
                show_help("An option was specified but the output path spec is missing");
            strcpy(path_h5, wstr);
            verbose = 0;
            break;
        case 3:
            strcpy(wstr, *++argv);
            if(strcmp(wstr, "-v") == 0) {
                verbose = 1;
                strcpy(path_h5, *++argv);
                break;
            }
            show_help("Unrecognizable parameter or extraneous string specified");
        default:
            show_help("Too many parameters specified");
    }
    
    /*
     * Allocate enough heap for the entire data matrix.
     */
    sz_alloc = NTINTS * NIFS * NCHANS * NBITS / 8;
    p_data = malloc(sz_alloc);
    if(p_data == NULL) {
        sprintf(wstr, "main malloc(%ld) FAILED", (long)sz_alloc);
        fatal_error(__LINE__, wstr);
        exit(86);
    }
    printf("alvin: Data matrix allocated, size  = %ld\n", (long) sz_alloc);

    /*
     * Make dummy spectra matrix.
     */
    wfptr = (float *) p_data;
    count_elems = 0;
	for(itime = 0; itime < NTINTS; itime++)
        for(jfreq = 0; jfreq < NCHANS; jfreq++) {
            *wfptr++ = get_random(low, high);
            count_elems++;
        }
    printf("alvin: Matrix element count = %ld\n", count_elems);
	        
    /*
     * Create the header data.
     */
    make_voyager_1_metadata(&wrh5_hdr);
    
    
    /*
     * Create the optional user structures.
     */
    memset(&chunking, 0, sizeof(chunking)); 
    chunking.n_time = NTINTS / 2;
    chunking.n_nifs = 1;
    chunking.n_fine_chan = NCHANS / 2;
    memset(&caching, 0, sizeof(caching));
    caching.nslots = 521;
    caching.nbytes = 10000000;
    caching.policy = 0.75;

    /*
     * Create/recreate the file and store the metadata.
     */
    printf("alvin: Data and header initialisation completed.  Begin data writes .....\n");
    time(&time1);
    if(wrh5_open(&wrh5_ctx, &wrh5_hdr, &chunking, &caching, path_h5, verbose) != 0) {
        fatal_error(__LINE__, "wrh5_open failed");
        exit(86);
    }

    /*
     * Write data.
     */
    size_t wrsize = NCHANS * sizeof(float);
    wfptr = (float *) p_data;
    for(int ii = 0; ii < NTINTS; ++ii) {
        if(wrh5_write(&wrh5_ctx, &wrh5_hdr, wfptr, wrsize, verbose) != 0) {
            fatal_error(__LINE__, "wrh5_write failed");
            exit(86);
        }
        wfptr += NCHANS;
    }   

    /*
     * Close FBH5 session.
     */
    if(wrh5_close(&wrh5_ctx, verbose) != 0) {
        fatal_error(__LINE__, "wrh5_close failed");
        exit(86);
    }

    /*
     * Compute elapsed time.    
     */
    time(&time2);
    printf("alvin: End, e.t. = %.2f seconds.\n", difftime(time2, time1));
    free(p_data);
	        
    /*
     * Bye-bye.
     */
    return 0;
}
