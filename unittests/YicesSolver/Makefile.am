# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../..

check_PROGRAMS = yicessolver_test

yicessolver_test_CXXFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS) -I$(LEVEL)/utils/unittest/googletest/include -I$(LEVEL)/src/
yicessolver_test_SOURCES  = YicesSolverTest.cpp
yicessolver_test_LDADD    = $(LLVM_LDADD) -lyices $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Expression.o $(LEVEL)/src/Logic/Formula.o $(LEVEL)/src/Logic/YicesSolver.o

TESTS = yicessolver_test