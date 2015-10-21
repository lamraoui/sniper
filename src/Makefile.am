# Makefile.am
# Copyright (C) 2013 by National Institute of Informatics

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS)
AM_CXXFLAGS = -Wall -O3 -fno-rtti -std=c++0x
#AM_LDFLAGS = -lyices

sniper_LDADD = $(LLVM_LDADD) -lyices

bin_PROGRAMS = sniper
sniper_SOURCES = Main.cpp \
		Options.cpp \
		Backends/SniperBackend/EncoderLightPass.cpp \
		Backends/SniperBackend/EncoderPass.cpp \
		Backends/SniperBackend/IterationAlgorithm.cpp \
		Backends/SniperBackend/SniperBackend.cpp \
		Benchmarks/TCAS.cpp \
		Encoder/Context.cpp \
		Encoder/Encoder.cpp \
		Encoder/EncoderLight.cpp \
		ExecutionEngine/ConcolicModule.cpp \
		ExecutionEngine/Executor.cpp \
		ExecutionEngine/IRInstrumentor.cpp \
		ExecutionEngine/PTFConcolic.cpp \
		ExecutionEngine/PTFRunner.cpp \
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
		Logic/GenPath.cpp \
		Logic/YicesSolver.cpp \
		Profile/ProgramProfile.cpp \
		Profile/ProgramTrace.cpp \
		Utils/HittingSet.cpp \
		Utils/Utils.cpp

#dist_noinst_SCRIPTS = autogen.sh
