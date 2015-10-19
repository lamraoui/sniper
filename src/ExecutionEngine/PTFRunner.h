/**
 * PTFRunner.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/09/18
 * @copyright : NII 2013
 */


#ifndef _PTFRUNNER_H
#define _PTFRUNNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>

#include "ConcolicModule.h"
#include "Executor.h"

using namespace llvm;

//============================================================================
class PTFRunner : public ConcolicModule {
       
private:
    Function *targetFun;
    ExecutionEngine *EE;
    std::vector<std::vector<Value*> > inputValuesVec;
    std::vector<Value*> outputValuesVec;
    
public:
    PTFRunner(Module *_llvmMod, Function *_targetFun, Options *_options,
              const std::string &tsFilename,
              /* optional */ const std::string &goFilename = std::string());
    ~PTFRunner();
    void run(ProgramProfile *profile, LocalVariables *locVars,
             LoopInfoPass *loopInfo);
    std::vector<std::vector<Value*> > parseTestsuiteFile(const std::string &filename);
    std::vector<Value*> parseGoldenOutputsFile(const std::string &filename);
    
};
//============================================================================

#endif
