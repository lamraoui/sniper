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

LEVEL = ../../..

noinst_LIBRARIES = libgtest_main.a
BUILD_ARCHIVE = 1
REQUIRES_RTTI = 1

libgtest_main_a_CPPFLAGS = -I$(LEVEL)/utils/unittest/googletest/include
libgtest_main_a_CPPFLAGS += -DGTEST_HAS_RTTI=0
# libstdc++'s TR1 <tuple> header depends on RTTI and uses C++'0x features not
# supported by Clang, so force googletest to use its own tuple implementation.
libgtest_main_a_CPPFLAGS += -DGTEST_USE_OWN_TR1_TUPLE
libgtest_main_a_CPPFLAGS += $(EXTRA_CXXFLAGS) $(LLVM_CPPFLAGS)

# TODO: use llvm-config
libgtest_main_a_LIBADD  = $(LEVEL)/utils/unittest/googletest/libgtest.a \
							-L/usr/local/lib -lLLVMSupport


NO_INSTALL = 1

libgtest_main_a_SOURCES = TestMain.cpp