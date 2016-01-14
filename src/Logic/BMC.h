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
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"

using namespace llvm;


//============================================================================
class BMC {

public:
    static void run(ProgramProfile *profile, Function *targetFun,
                    YicesSolver *solver, Formula *TF, Formula *AS,
                    LoopInfoPass *loopInfo, Options *options, bool hasArgv);
    
};
//============================================================================

#endif
