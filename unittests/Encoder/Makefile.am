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

LEVEL = ../..

check_PROGRAMS = encoder_test

encoder_test_CXXFLAGS = $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS) -I$(LEVEL)/utils/unittest/googletest/include -I$(LEVEL)/src/
encoder_test_SOURCES  = EncoderTest.cpp
encoder_test_LDADD    = $(LLVM_LDADD) -lyices $(LEVEL)/utils/unittest/googletest/libgtest.a $(LEVEL)/src/Logic/Expression.o $(LEVEL)/src/Logic/Formula.o $(LEVEL)/src/Options.o $(LEVEL)/src/Utils/Utils.o $(LEVEL)/src/Frontend/Frontend.o $(LEVEL)/src/Frontend/FullFunctionInliningPass.o $(LEVEL)/src/Frontend/GlobalVariables.o $(LEVEL)/src/Frontend/LocalVariables.o $(LEVEL)/src/Frontend/LoopInfoPass.o $(LEVEL)/src/Encoder/Context.o $(LEVEL)/src/Encoder/Encoder.o

TESTS = encoder_test