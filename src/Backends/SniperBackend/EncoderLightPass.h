/**
 * EncoderLightPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/04
 * @copyright : NII 2013
 */

#ifndef _ENCODERLIGHTPASS_H
#define _ENCODERLIGHTPASS_H

#include <map>
#include <ctime>
#include <sys/time.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

#include "Options.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Encoder/Context.h"
#include "Encoder/EncoderLight.h"

using namespace llvm;


//============================================================================
class EncoderLightPass {
    
private:
    Function *targetFun;
    Context *ctx;
    EncoderLight *encoder;
    LoopInfoPass *loops;
    ProgramProfile * profile;
    Options *options;
    Formula *AS;

public:
    EncoderLightPass(Function *_targetFun, Context *_ctx, LoopInfoPass *_loops,
                ProgramProfile *profile, Options *options);
    ~EncoderLightPass();
    Formula* makeTraceFormula();
    Formula* getASFormula();
    
private:
    void initGlobalVariables();
    void initAssertCalls();
    bool isAtoiFunction(Instruction *I);
    
};
//============================================================================

#endif