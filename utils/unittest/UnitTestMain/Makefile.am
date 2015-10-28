# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../../..

noinst_LIBRARIES = libgtest_main.a
BUILD_ARCHIVE = 1
REQUIRES_RTTI = 1

libgtest_main_a_CPPFLAGS = -I$(LEVEL)/utils/unittest/googletest/include
libgtest_main_a_CPPFLAGS += -DGTEST_HAS_RTTI=0
# libstdc++'s TR1 <tuple> header depends on RTTI and uses C++'0x features not
# supported by Clang, so force googletest to use its own tuple implementation.
libgtest_main_a_CPPFLAGS += -DGTEST_USE_OWN_TR1_TUPLE
libgtest_main_a_CPPFLAGS += $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS)

# TODO: use llvm-config
libgtest_main_a_LIBADD  = $(LEVEL)/utils/unittest/googletest/libgtest.a \
							-L/usr/local/lib -lLLVMSupport


NO_INSTALL = 1

libgtest_main_a_SOURCES = TestMain.cpp