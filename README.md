# libwrh5

#### OVERVIEW

This git project constitutes a Filterbank HDF5 file writing utility.  The library has been successfully built and tested on Raspberry Pi OS and Ubuntu.  It should run on other POSIX OSes and MacOS.  No GPUs are required.

#### LICENSING

This is *NOT* commercial software; instead, usage is covered by the GNU General Public License version 3 (2007).  In a nutshell, please feel free to use the project and share it as you will but please don't sell it.  Thanks!

See the LICENSE file for the GNU licensing information.

#### PREREQUISITES

You will need the HDF5 library and some utilities are helpful.  On a debian-ish system, get these apt packages:
* libhdf5-dev : run-time library
* hdf5-tools : HDF5 runtime tools

I'll assume that you already have various GNU development tools.  See ```dependencies.txt``` for the complete list.

#### GETTING STARTED

Subfolders:

* src - C-language source code for the library and a Makefile
* lib - Upon completion of the ```make``` utility, this folder holds the shared object library file.
* test - 2 simple test program, ```simon``` (default chunking and caching) and ```alvin``` (user-specified chunking and caching). 
* test_data - test HDF5 data.

See ```API.md``` for the API.

#### BUILD LIBRARY AND VERIFY

In the highest-level directory,
* ```make```
* ```bash run.sh```
* ```make clean``` (not required)

#### HDF5 Filter Plugins

References: \
[1] Filter identifier 32008 at https://support.hdfgroup.org/services/contributions.html \
[2] https://github.com/kiyo-masui/bitshuffle \
[3] https://support.hdfgroup.org/HDF5/doc/Advanced/DynamicallyLoadedFilters/HDF5DynamicallyLoadedFilters.pdf

There is only one filter plugin used by this project if available: Bitshuffle.  This plugin is registered with the HDF Group - see reference [1].  

As stated in reference [2], "Bitshuffle is an algorithm that rearranges typed, binary data for improving compression, as well as a python/C package that implements this algorithm within the Numpy framework".

If this filter is installed, this project will use it.  Otherwise, a warning is issued and no (compression) processing will occur.  

By default, on POSIX systems, the ```libhdf-dev``` software will look for all filters at ```/usr/local/hdf5/lib/plugin```.  However, by setting the ```HDF5_PLUGIN_PATH``` environment variable prior to program execution, one can override the default.  See reference [3].

If one installs the Python ```hdf5plugin``` package, several filters (including Bitshuffle) can be found in the ```... /site-packages/hdf5plugin/plugins/``` subdirectory.  For example, assume that (1) Python 3.10 is in use and (2) the pip packages have been installed with the pip3 ```--user``` option.  Then, the following bash export will facilitate subsequent filter processing:

```export HDF5_PLUGIN_PATH=$HOME/.local/lib/python3.10/site-packages/hdf5plugin/plugins/```


==================================================================

Feel free to open an issue for inquiries and bugs.  I'll respond as soon as I can.

Richard Elkins

Dallas, Texas, USA, 3rd Rock, Sol, ...
