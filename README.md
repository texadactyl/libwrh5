# libwrh5

#### Overview

This git project constitutes a Filterbank HDF5 file writing library with accompanying test programs that also serve as examples.  The library has been successfully built and tested on Raspberry Pi OS and Ubuntu.  It should run on other POSIX OSes and MacOS.  No GPUs are required.

#### Licensing

This is *NOT* commercial software; instead, usage is covered by the GNU General Public License version 3 (2007).  In a nutshell, please feel free to use the project and share it as you will but please don't sell it.  Thanks!

See the LICENSE file for the GNU licensing information.

#### Prerequisites

You will need the HDF5 library and some utilities are helpful.  On a debian-ish system, get these apt packages:
* libhdf5-dev : run-time library
* hdf5-tools : HDF5 runtime tools

I'll assume that you already have various GNU development tools.  See ```dependencies.txt``` for the complete list.

#### Getting Started

Subfolders:

* src
    - C-language source code (*.c)
    - Makefile
    - wrh5_defs.h : function and parameter definitions
    - wrh5_version.h : software version
* lib - Upon completion of the ```make``` utility, this folder holds the shared object library file.
* test 
    - simon : default chunking and caching, user-defined nfpc value.
    - alvin : user-specified chunking and caching, no nfpc value provided (0). 
* test_data - test HDF5 data.

See ```API.md``` for the API.

#### Build Library and Test

In the highest-level directory,
* ```make```
* ```make try```

#### Installation and Uninstallation

By default, the target installation directory root is /usr/local.  This is indicated by the absence of a PREFIX environment variable at the time of the ```make install``` or ```make uninstall``` execution. 

If one wishes to use a different target installation directory root, specify ```export PREFIX=/your/chosen/root``` prior to running ```sudo -E make install``` or ```sudo -E make uninstall```.  Note that when using the PREFIX environment variable, it is important that the ```sudo``` command receive the ```-E``` parameter in order to preserve the value of PREFIX.

During installation, files are copied as follows:
* Header files (*.h) --> ```include``` subdirectory
* libwr5.so --> ```lib``` subdirectory

During uninstallation, those files are removed from the target installation subdirectories.

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
