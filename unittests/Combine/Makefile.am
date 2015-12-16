# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../..

check_PROGRAMS = combine_test

combine_test_CXXFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS) -I$(LEVEL)/utils/unittest/googletest/include -I$(LEVEL)/src/
combine_test_SOURCES  = CombineTest.cpp
combine_test_LDADD    = $(LLVM_LDADD) -lyices $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Expression.o $(LEVEL)/src/Logic/Formula.o $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Combine.o

TESTS = combine_test