set -e
nargs=$#

if [ $nargs -ne 1 ]; then
	echo \*\*\* Number of arguments must be 1; observed $nargs \!\!\!
	exit 1
fi
if [ ${LINK_LIBHDF5}xx == xx ]; then
	echo \*\*\* Must be run from the root directory Makefile \!\!\!
	exit 1
fi

TEST_DATA=$1

FILFILE=$TEST_DATA/voya.fil
HDR_FILE=$TEST_DATA/voya_header.txt
DATA_FILE=$TEST_DATA/voya_data.bin
H5_FILE=$TEST_DATA/voya.h5

if [ ! -f $FILFILE ]; then
	curl --url "http://blpd0.ssl.berkeley.edu/Voyager_data/Voyager1.single_coarse.fine_res.fil"  -o $FILFILE
fi

python3 scrape.py --in_fil $FILFILE --out_hdr $HDR_FILE --out_data $DATA_FILE

set -x
theodore  $HDR_FILE  $DATA_FILE  $H5_FILE  0

watutil -i $H5_FILE
