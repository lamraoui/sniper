/**
 * ConcolicProfiler.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */


#ifndef _CONCOLICPROFILER_H
#define _CONCOLICPROFILER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ConcolicModule.h"
#include "Executor.h"

using namespace llvm;

//============================================================================
class ConcolicProfiler : public ConcolicModule {
       
private:
    Function *targetFun;
    ExecutionEngine *EE;
    
public:
    ConcolicProfiler(Module *_llvmMod, Function *_targetFun, Options *_options);
    ~ConcolicProfiler();
    void run(ProgramProfile *profile, LocalVariables *locVars, 
             LoopInfoPass *loopInfo);

private:
    VariablesPtr generateInputValues();
    bool solve(std::vector<ExprCellPtr> path, std::vector<ExprCellPtr> asserts, 
               bool genFailing, VariablesPtr inputs);
    
};
//============================================================================

#endif
