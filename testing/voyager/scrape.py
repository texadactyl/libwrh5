"""
Scrape a SIGPROC Filterbank file (.fil), producing:
    * header file (text)
    * data matrix file (binary)
"""

import os
import struct
from argparse import ArgumentParser
import numpy as np
from blimpy import Waterfall


header_keyword_types = {
    'telescope_id' : '<l',
    'machine_id'   : '<l',
    'data_type'    : '<l',
    'barycentric'  : '<l',
    'pulsarcentric': '<l',
    'nbits'        : '<l',
    'nsamples'     : '<l',
    'nchans'       : '<l',
    'nifs'         : '<l',
    'nbeams'       : '<l',
    'ibeam'        : '<l',
    'rawdatafile'  : 'str',
    'source_name'  : 'str',
    'az_start'     : '<d',
    'za_start'     : '<d',
    'tstart'       : '<d',
    'tsamp'        : '<d',
    'fch1'         : '<d',
    'foff'         : '<d',
    'refdm'        : '<d',
    'period'       : '<d',
    'src_raj'      : 'angle',
    'src_dej'      : 'angle',
}


def fil_double_to_angle(angle):
    """ 
    Reads a little-endian double in ddmmss.s (or hhmmss.s) format and then
    converts to Float degrees (or hours).  This is primarily used to read
    src_raj and src_dej header values. 
    """

    negative = angle < 0.0
    angle = np.abs(angle)

    dd = np.floor((angle / 10000))
    angle -= 10000 * dd
    mm = np.floor((angle / 100))
    ss = angle - 100 * mm
    dd += mm/60.0 + ss/3600.0

    if negative:
        dd *= -1

    return dd


def read_next_header_element(fh):
    """
    Get the next header element.

    Parameters
    ----------
    fh : file handle
        Used to read the file.

    Returns
    -------
    keyword : string
        Name of the header field.
    value : multiple variable types
        Value associated with the header field.
    """

    n_bytes = np.frombuffer(fh.read(4), dtype='uint32')[0]

    if n_bytes > 255:
        n_bytes = 16

    keyword = fh.read(n_bytes).decode('ascii')

    if keyword in ('HEADER_START', 'HEADER_END'):
        return keyword, 0
    dtype = header_keyword_types[keyword]
    if dtype == '<l':
        value = struct.unpack(dtype, fh.read(4))[0]
    if dtype == 'str':
        str_len = np.frombuffer(fh.read(4), dtype='uint32')[0]
        value = fh.read(str_len).decode('ascii')
    if dtype == '<d':
        value = struct.unpack(dtype, fh.read(8))[0]
    if dtype == 'angle':
        value = fil_double_to_angle(struct.unpack('<d', fh.read(8))[0])
    return keyword, value


def read_header(filename):
    """
    Read the entire .fil file header.

    Parameters
    ----------
    filename : string
        O/S path of file.

    Returns
    -------
    hdr : Python dict object
        Populated header.

    """
    hdr = {}

    with open(filename, 'rb') as fh:
        # Check this is a blimpy file
        keyword, value = read_next_header_element(fh)
        assert keyword == 'HEADER_START'
        while True:
            keyword, value = read_next_header_element(fh)
            if keyword == 'HEADER_END':
                break
            hdr[keyword] = value
    return hdr


def scrape_header(infile_path, outhdr_path):
    """
    Scrape the header.

    Parameters
    ----------
    infile_path : string
        O/S path of input .fil file.
    outhdr_path : string
        O/S path of output header file as an O/S text file.

    Returns
    -------
    None.

    """
    hdr = read_header(infile_path)
    with open(outhdr_path, "w", encoding="utf-8") as outh:
        for (key, value) in hdr.items():
            outh.write(f"{key}     {value}\n")


def scrape_data(infile_path, outdata_path):
    """
    Scrape the data matrix.

    Parameters
    ----------
    infile_path : string
        O/S path of input .fil file.
    outhdr_path : string
        O/S path of output data file as a binary file.

    Returns
    -------
    None.

    """
    wf = Waterfall(infile_path)
    with open(outdata_path, "wb") as outh:
        outh.write(wf.data)


def main(args=None):
    parser = ArgumentParser(description="Scrape a Filterbank file, producing a header file and a data file.")

    parser.add_argument("--in_fil", required=True, type=str,
                        help="Input Filterank file path (.fil)")
    parser.add_argument("--out_hdr", required=True, type=str,
                        help="Output header file path (text)")
    parser.add_argument("--out_data", required=True, type=str,
                        help="Output data matrix file path (binary)")

    if args is None:
        args = parser.parse_args()
    else:
        args = parser.parse_args(args)

    abs_in_fil = os.path.abspath(args.in_fil)
    abs_out_hdr = os.path.abspath(args.out_hdr)
    abs_out_data = os.path.abspath(args.out_data)

    print("Writing header file: " + abs_out_hdr)
    scrape_header(abs_in_fil, abs_out_hdr)
    print("Header done")

    print("Writing data file: " + abs_out_data)
    scrape_data(abs_in_fil, abs_out_data)
    print("Data done")


if __name__ == "__main__":
    main()
