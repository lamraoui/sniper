# Makefile.am
# Copyright (C) 2015 by Hosei University

LEVEL = ../..

check_PROGRAMS = encoder_test

encoder_test_CXXFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS) -I$(LEVEL)/utils/unittest/googletest/include -I$(LEVEL)/src/
encoder_test_SOURCES  = EncoderTest.cpp
encoder_test_LDADD    = $(LLVM_LDADD) -lyices $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Expression.o $(LEVEL)/src/Logic/Formula.o $(LEVEL)/src/Options.o $(LEVEL)/src/Utils/Utils.o $(LEVEL)/src/Frontend/LocalVariables.o $(LEVEL)/src/Frontend/LoopInfoPass.o $(LEVEL)/src/Encoder/Context.o $(LEVEL)/src/Encoder/Encoder.o

TESTS = encoder_test