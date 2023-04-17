set -e
nargs=$#

if [ $nargs -ne 1 ]; then
	echo \*\*\* Number of arguments must be 1; observed $nargs \!\!\!
	exit 1
fi

TEST_DATA=$1

# Run simon and dump the output header:
./simon $TEST_DATA/simon.h5
h5dump -A $TEST_DATA/simon.h5

# Run alvin and dump the output header:
./alvin $TEST_DATA/alvin.h5
h5dump -A $TEST_DATA/alvin.h5

