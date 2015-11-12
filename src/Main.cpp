/**
 * Main.cpp
 *
 *
 * Compile :
 * g++ -g -O3 tool.cpp `llvm-config --cppflags --ldflags --libs` -o tool
 *
 * g++ -g -O3 tool.cpp backend/Backend.cpp backend/YicesPass.cpp `llvm-config --cppflags --ldflags --libs` -o tool
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/01
 * @copyright : NII 2012
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


int main(int argc, char **argv) {
    
	try {
        // Tool options
        Options *opt = new Options(argc, argv);
        
        MSTimer timer;
        if(opt->printDuration()) {
            timer.start();
        }
        
		sys::PrintStackTraceOnErrorSignal();
        
        //===============================================
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
        //===============================================
    
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

