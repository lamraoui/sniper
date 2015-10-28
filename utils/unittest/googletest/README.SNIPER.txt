SNIPER notes
----------

This directory contains Google Test 1.7.0, with all elements removed except for
the actual source code, to minimize the addition to the SNIPER distribution.

Cleaned up as follows:

# Remove all the unnecessary files and directories
$ rm -f CHANGES CMakeLists.txt CONTRIBUTORS README LICENSE Makefile.am configure.ac
$ rm -rf build-aux codegear m4 msvc scripts test cmake make samples xcode 
$ rm -f `find . -name \*\.pump`
$ rm -f src/gtest_main.cc