/**
 * \file Main.cpp
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 * \author Si-Mohamed Lamraoui
 * \date   1 March 2016
 */

#include <string>
#include <algorithm>
#include <ctime>
#include <sys/time.h>
#include <cerrno>
#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/CommandLine.h"

#include "Options.h"
#include "Frontend/Frontend.h"
#include "Backends/SniperBackend/SniperBackend.h"

using namespace llvm;

/**
 * Entry point.
 * See Option for command-line options.
 */
int main(int argc, char **argv) {
    
	try {
        // Tool options
        Options *opt = new Options(argc, argv);
        
        if (opt->verbose()) {
            std::cout <<  "SNIPER  Copyright (C) 2016  Si-Mohamed LAMRAOUI\n";
            std::cout <<  "This program comes with ABSOLUTELY NO WARRANTY.\n";
            std::cout <<  "This is free software, and you are welcome to redistribute it\n";
            std::cout <<  "under certain conditions; see LICENSE.TXT for details.\n\n";
        }

        // Tell user if NDEBUG is defined and do assert
        if(opt->dbgMsg()) {
#if defined(NDEBUG)
            std::cout << "NDEBUG is defined. Assert disabled,\n";
#else
            std::cout << "NDEBUG is not defined. Assert enabled.\n";
#endif
        }
        
        MSTimer timer;
        if(opt->printDuration()) {
            timer.start();
        }
        
		sys::PrintStackTraceOnErrorSignal();
        
        if(opt->dbgMsg()) {
            std::cout << std::endl;
            std::cout << "Loading bitcode file\n";
        }
        
        // Lot of copy-paste from lli.cpp
        LLVMContext &Context = getGlobalContext();
        
        // If we have a native target, initialize it to ensure  
		// it is linked in and usable by the JIT.
        InitializeNativeTarget();
        
        // Load the bitcode...
        MSTimer timer2;
        if(opt->printDuration()) {
            timer2.start();
        }
        Module *llvmMod = NULL;
        SMDiagnostic Err;
        llvmMod = ParseIRFile(opt->getInputIRFilename(), Err, Context);
        if (!llvmMod) {
            Err.print(argv[0], errs());
            return 1;
        } 
        if(opt->printDuration()) {
            timer2.stop("Bitcode Loading Time");
        }
        if(opt->dbgMsg())
            std::cout << "Bitcode file loaded\n";
    
        // Run frontend
        if(opt->dbgMsg())
            std::cout << "Launching frontend\n";
        Frontend *fe = new Frontend(llvmMod, opt);
        fe->run();
        
        // Run SNIPER
        if(opt->dbgMsg())
            std::cout << "Launching backend\n";
        SniperBackend *be = new SniperBackend(fe, opt);
        be->run();
        
        if(opt->printDuration()) {
            timer.stop("Time");
        }
        delete fe;
        delete be;
        delete opt;
        
    } catch(const std::string &msg) {
		errs() << argv[0] << ": " << msg << "\n";
	} catch(...) {
		errs() << argv[0] << ": Unexpected unknown exception occurred.\n";
	}
}

