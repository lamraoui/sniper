# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../..

check_PROGRAMS = formula_test

formula_test_CXXFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS) -I$(LEVEL)/utils/unittest/googletest/include -I$(LEVEL)/src/
formula_test_SOURCES  = FormulaTest.cpp
formula_test_LDADD    = $(LLVM_LDADD) -lyices $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Expression.o $(LEVEL)/src/Logic/Formula.o

TESTS = formula_test