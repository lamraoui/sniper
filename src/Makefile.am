# Makefile.am
# Copyright (C) 2013 by National Institute of Informatics

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS)
AM_CXXFLAGS = $(LLVM_CXXFLAGS) -fexceptions
#-Wall -O3 -fno-rtti -fexceptions -stdlib=libc++ -mmacosx-version-min=10.7    #-std=c++11 -stdlib=libc++ -stdlib=libstdc++
#AM_LDFLAGS = -lyices

sniper_LDADD = $(LLVM_LDADD) -lyices

bin_PROGRAMS = sniper
sniper_SOURCES = Main.cpp \
		Options.cpp \
		Backends/SniperBackend/EncoderPass.cpp \
		Backends/SniperBackend/IterationAlgorithm.cpp \
		Backends/SniperBackend/SniperBackend.cpp \
		Encoder/Context.cpp \
		Encoder/Encoder.cpp \
		ExecutionEngine/ConcolicModule.cpp \
		ExecutionEngine/Executor.cpp \
		ExecutionEngine/IRInstrumentor.cpp \
		ExecutionEngine/ConcolicProfiler.cpp \
		ExecutionEngine/IRRunner.cpp \
		ExecutionEngine/Symbol.cpp \
		ExecutionEngine/SymbolicPath.cpp \
		ExecutionEngine/SymbolMap.cpp \
        	Frontend/Frontend.cpp \
        	Frontend/FullFunctionInliningPass.cpp \
		Frontend/GlobalVariables.cpp \
		Frontend/LocalVariables.cpp \
		Frontend/LoopInfoPass.cpp \
		Logic/BMC.cpp \
		Logic/Combine.cpp \
		Logic/Expression.cpp \
		Logic/Formula.cpp \
		Logic/YicesSolver.cpp \
		Profile/ProgramProfile.cpp \
		Profile/ProgramTrace.cpp \
		Utils/Utils.cpp

#dist_noinst_SCRIPTS = autogen.sh
