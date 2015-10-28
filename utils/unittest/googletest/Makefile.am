# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../../..

noinst_LIBRARIES = libgtest.a
BUILD_ARCHIVE = 1
REQUIRES_RTTI = 1

# Note that these flags are duplicated when building individual tests in
# unittests/Makefile.unittest and ../UnitTestMain/Makefile; ensure that any
# changes are made to both.
libgtest_a_CPPFLAGS = -I$(LEVEL)/utils/unittest/googletest/include
libgtest_a_CPPFLAGS += -I$(LEVEL)/utils/unittest/googletest
libgtest_a_CPPFLAGS += -DGTEST_HAS_RTTI=0
# libstdc++'s TR1 <tuple> header depends on RTTI and uses C++'0x features not
# supported by Clang, so force googletest to use its own tuple implementation.
libgtest_a_CPPFLAGS += -DGTEST_USE_OWN_TR1_TUPLE

# TODO
#ifeq ($(HOST_OS),MingW)
#  libgtest_a_CPPFLAGS += -DGTEST_OS_WINDOWS=1
#endif

NO_INSTALL = 1

libgtest_a_SOURCES = src/gtest-all.cc
