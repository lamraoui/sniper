# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../..

check_PROGRAMS = expression_test

expression_test_LDADD    = $(LEVEL)/utils/unittest/googletest/libgtest.a
expression_test_CXXFLAGS = -I$(LEVEL)/utils/unittest/googletest/include

expression_test_SOURCES  = ExpressionTest.cpp

TESTS = expression_test