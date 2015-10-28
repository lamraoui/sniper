# configure.ac
# Copyright (C) 2013 by National Institute of Informatics

AC_INIT([SNIPER], [0.2], [simo@nii.ac.jp], [sniper])

# check if the source folder is correct
AC_CONFIG_SRCDIR([src/main.cpp])

AC_PREREQ([2.59])
# Disable the -Wextra-portability option until developers can all use
# automake-1.11.2 or newer, at which point we'll use the new AM_PROG_AR instead.
AM_INIT_AUTOMAKE([1.10 -Wall no-define subdir-objects -Wno-extra-portability])
AC_CONFIG_HEADERS([config.h])

AC_GNU_SOURCE

# Compiler C++11 standard is required
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)

# Use the C++ compiler for the following checks
AC_LANG([C++])

# Checks for programs
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([memory])



# Platform specific setup
# For retrieving the type of host for compilation
AC_CANONICAL_HOST
# Check for which host we are on and setup a few things
# specifically based on the host
case $host_os in
  darwin* )
	EXTRA_CXXFLAGS=""   #"-mmacosx-version-min=10.6"
        ;;
  linux*)
	EXTRA_CXXFLAGS=""
        ;;
   *BSD*)
	EXTRA_CXXFLAGS=""
        ;;
    *)
        #Default Case
        AC_MSG_ERROR([Your platform is not currently supported ($host_os)])
        ;;
esac
AC_SUBST(EXTRA_CXXFLAGS)


# **************************************************************************

# Verify that we can find llvm
llvm_version=`llvm-config --version`
if test X${llvm_version} = X; then
	AC_MSG_ERROR([Unable to find LLVM])
fi

# Require LLVM = 3.3 
AC_MSG_CHECKING([llvm version])
if echo "$llvm_version 3.3 -p" | dc | grep > /dev/null ^-; then
	AC_MSG_ERROR([Require LLVM version = 3.3])
fi
AC_MSG_RESULT([$llvm_version])

# Verify the build mode of llvm
AC_MSG_CHECKING([llvm build mode])
llvm_mode=$(llvm-config --build-mode | tr '\n' ' ')
if test "$llvm_mode" == "Release"; then
	AC_MSG_ERROR([Require LLVM build mode: Release])
fi
AC_MSG_RESULT([$llvm_mode])

# LLVM preprocessor/link flags
AC_SUBST(LLVM_CPPFLAGS, $(llvm-config --cppflags))
AC_SUBST(LLVM_LDADD, $(llvm-config --ldflags --libs core linker ipo interpreter mcjit jit native irreader | tr '\n' ' '))


AC_CONFIG_FILES([
	Makefile 
	src/Makefile 
	utils/Makefile
	utils/unittest/Makefile
	utils/unittest/googletest/Makefile
	unittests/Makefile
	unittests/Expression/Makefile
])
AC_OUTPUT

