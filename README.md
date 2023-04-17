# libwrh5

#### Overview

This git project constitutes a Filterbank HDF5 file writing library with accompanying test programs that also serve as examples.  The library has been successfully built and tested on Raspberry Pi OS and Ubuntu.  It should run on other POSIX OSes and MacOS.  No GPUs are required.

#### Brief History

* Started as the fbh5 subset of rawspec.
* Converted to a generic library without Filterbank for a private non-github application.
* Reflavoured with Filterbank for use with telescope applications that can stream a Filterbank header and data to this library.

#### Licensing

This is *NOT* commercial software; instead, usage is covered by the GNU General Public License version 3 (2007).  In a nutshell, please feel free to use the project and share it as you will but please don't sell it.  Thanks!

See the LICENSE file for the GNU licensing information.

#### Prerequisites

You will need the HDF5 library and some utilities are helpful.  On a debian-ish system, get these apt packages:
* libhdf5-dev : run-time library
* hdf5-tools : HDF5 runtime tools

I'll assume that you already have various GNU development tools.  See ```dependencies.txt``` for the complete list.

#### Getting Started

Makefile: drives all ```make``` functions:
* build
    - Compile all library source and testing *.c files.
    - Create the library.
* try - Try the unit tests, alvin and simon.
* voya - Try the Voyager 1 data (theodore)
* install - system level installation of library file and header files (super-user access required).
* uninstall - undo system level installation (super-user access required).
* clean - remove all built objects, lib directory, and test_data directory.

Permanent subfolders:
* src
    - C-language source code (*.c)
    - wrh5_defs.h : function and parameter definitions
    - wrh5_version.h : software version
    - src.mk : ```make``` file for this subdirectory
* testing/unit_tests 
    - simon.c : default chunking and caching, user-defined nfpc value.
    - alvin.c : user-specified chunking and caching, no nfpc value provided (0). 
    - unit_tests.mk : ```make``` file for this subdirectory
* testing/voyager
    - scrape.py : Read a Voyager 1 SIGPROC Filterbank file (.fil) and produce [a} header file and [b] binary image data matrix file.
    - theodore.c : Read header file and data file; output a Filterbank HDF5 file (.h5).
    - voyager.mk : ```make``` file for this subdirectory

Dynamically-created subfolders:
* lib - libwrh5.so
* test_data - testing HDF5 data and supporting data artifacts.

See ```API.md``` for the API.

#### Build Library and Test

In the highest-level directory,
* Build library and test tools: ```make```
* Try the unit test tools: ```make try```
* Try the Voyager 1 data: ```make voya``` (requires Python and package blimpy)

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
