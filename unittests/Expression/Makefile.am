# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../..

check_PROGRAMS = expression_test

expression_test_CXXFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS) -I$(LEVEL)/utils/unittest/googletest/include -I$(LEVEL)/src/
expression_test_SOURCES  = ExpressionTest.cpp
expression_test_LDADD    = $(LLVM_LDADD) -lyices $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Expression.o

TESTS = expression_test