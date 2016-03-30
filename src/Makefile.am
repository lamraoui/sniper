# Makefile.am
#
# ----------------------------------------------------------------------
#                SNIPER : Automatic Fault Localization 
#
# Copyright (C) 2016 Si-Mohamed LAMRAOUI
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program (see LICENSE.TXT).  
# If not, see <http://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------
#
# \author Si-Mohamed LAMRAOUI
# \date   30 March 2016


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
		Backends/SniperBackend/FaultLocalization.cpp \
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
