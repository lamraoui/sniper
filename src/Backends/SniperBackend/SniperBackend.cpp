/**
 * SniperBackend.cpp
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/04
 * @copyright : NII 2013
 */

#include "SniperBackend.h"


void SniperBackend::run() {
    
    // Get the target module
    Module *llvmMod = frontend->getLLVMModule();
    
    // Get the target function from the bytecode
    std::string fname = options->getFunctionName();
    if (fname=="") {
        fname = "main";
    }
    Function *targetFun = frontend->getFunction(fname);
    assert(targetFun==NULL && "Requested function missing");
    
    LocalVariables *LV = frontend->getLocalVars();
    LoopInfoPass *LIP  = frontend->getLoopInfo();
    bool hasArgv = frontend->hasArgv();

    // Create an empty program profile for holding execution info
    ProgramProfile *PP = new ProgramProfile(targetFun);
    
    // Run the program with the given inputs
    std::string tsFilename = options->getTestSuiteFileName();
    if (options->methodConcolic() && !tsFilename.empty()) {
        MSTimer ctimer;
        if (options->printDuration()) {
            ctimer.start();
        }
        std::string goFilename = options->getGoldenOutputsFileName();
        ConcolicModule *CR =
        new IRRunner(llvmMod, targetFun, options, tsFilename, goFilename);
        CR->run(PP, LV, LIP);
        delete CR;
        if (options->printDuration()) {
            ctimer.stop("Prog. Runner Execution Time");
        }
    }
    // Concolic Execution
    else if (options->methodConcolic()) {
        MSTimer ctimer;
        if (options->printDuration()) {
            ctimer.start();
        }
        // Run the concolic algorithm of CUTE/DART (with profiler)
        ConcolicModule *CM = new ConcolicProfiler(llvmMod, targetFun, options);
        CM->run(PP, LV, LIP);
        delete CM;
        if (options->printDuration()) {
            ctimer.stop("Concolic Execution Time");
        }
    }
    if (options->methodConcolic()) {
        if (PP->getFailingProgramTraces(options).size()==0) {
            std::cout << "VERIFICATION SUCCESSFUL\n";
            exit(0);
        }
        // Fitler traces depending on option values
        unsigned maxFailTraces = options->getMaxFailTraces();
        if (maxFailTraces>0) {
            // Randomly remove exceeding traces
            unsigned n = PP->getFailingProgramTraces(options).size();
            PP->removeNFirstFailingProgramTraces(n-maxFailTraces);
            //std::cout << "Removing fail. traces: "
            //<< maxFailTraces << std::endl;
        }
        unsigned maxSuccTraces = options->getMaxSuccTraces();
        if (maxSuccTraces>0) {
            // Randomly remove exceeding traces
            unsigned n = PP->getSuccessfulProgramTraces().size();
            PP->removeNFirstSuccessfulProgramTraces(n-maxSuccTraces);
            //std::cout << "Removing succ. traces: "
            //<< maxSuccTraces << std::endl;
        }
        // Update program profile
        if (options->htfUsed()) {
            MSTimer btimer;
            if (options->printDuration()) {
                btimer.start();
            }
            PP->computeBugFreeBlocks(options);
            if (options->printDuration()) {
                btimer.stop("BF Blocks Computation Time");
            }
        }
    }
    // Encode the IR into a partial weighted formula
    Context     *C       = new Context(LV, options);
    //EncoderLightPass *EP = new EncoderLightPass(targetFun, C, LIP, PP, options);
    EncoderPass *EP = new EncoderPass(targetFun, C, LIP, PP, options);
    Formula     *TF = EP->makeTraceFormula();
    Formula     *AS = EP->getASFormula();
    delete EP;
    delete C;
    
    if (options->printTF()) {
        TF->dump();
    }
    
    // Create a partial weighted MaxSMT solver
    IWPMaxSATSolver *solver = new YicesSolver();
    
    // Generate program executions
    if (options->methodConcolic()) {
        //std::vector<Expression*> NPC = formula->getNotPostConditions();
        //std::vector<Expression*> PC = formula->getPostConditions();
    }
    else {
        BMC *bmc = new BMC(targetFun, solver, TF, AS, LIP, options, hasArgv);
        bool ok = true;
        if (options->methodPathExploration()) {
            if (LIP->hasLoops()) {
                std::cout << "exit: Cannot use Path Exploration on programs with loops.\n";
                exit(1);
            }
            bmc->runBMCWithPathExploration(PP);
            if (!PP->hasFailingProgramTraces()) {
                std::cout << "BMC with Path Exloration failed.";
                std::cout << " Running classic BMC instead...\n";
                ok = false;
            }
        }
        if (options->methodBMC() || !ok) {
            // Compute a single failing program execution
            bmc->runClassicBMC(PP);
            if (PP->hasFailingProgramTraces()) {
                if(options->verbose()) {
                    std::cout << "  Failing execution found: ";
                    PP->dump();
                }
            } else {
                std::cout << "VERIFICATION SUCCESSFUL\n";
                exit(0);
            }
        }
    }
    
    // Locate the errors in the input program
    assert(options->pushPopUsed() && "Use push & pop optimization!");
    
    IterationAlgorithm *IA =
    new IterationAlgorithm(targetFun, solver, hasArgv, options);
    
    //Combine::NONE
    //Combine::FLA
    //Combine::PWU
    //Combine::MHS
    Combine::Method CM = Combine::FLA;
    
    if (options->useDynamicDiagnosesEnum()) {
        IA->run_dynamic(TF, AS, PP, CM);
    } else {
        IA->run(TF, AS, PP, CM);
    }
}