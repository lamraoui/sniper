/**
 * BMC.h
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/29
 * @copyright : NII 2013
 */


#ifndef _BMC_H
#define _BMC_H

#include <vector>

#include "Options.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Logic/GenPath.h"
#include "Logic/ISolver.h"
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"

using namespace llvm;


//============================================================================
class BMC {
 
private:
    Function *targetFun;
    ISolver *solver;
    Formula *TF;
    Formula *AS;
    LoopInfoPass *loopInfo;
    Options *options;
    bool hasArgv;

public:
    BMC(Function *_targetFun, ISolver *_solver, Formula *_TF, Formula *_AS,
        LoopInfoPass *_loopInfo, Options *_options, bool hasArgv);
    ~BMC() { }
    void runBMCWithPathExploration(ProgramProfile *profile);
    void runClassicBMC(ProgramProfile *profile);
    
};
//============================================================================

#endif
