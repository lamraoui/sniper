/**
 * EncoderPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/04
 * @copyright : NII 2013
 */

#ifndef _ENCODERPASS_H
#define _ENCODERPASS_H

#include <map>
#include <ctime>
#include <sys/time.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

#include "Options.h"
#include "Utils/Utils.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Encoder/Context.h"
#include "Encoder/Encoder.h"

using namespace llvm;


//============================================================================
class EncoderPass {
    
private:
    Function *targetFun;
    Context *ctx;
    Encoder *encoder;
    LoopInfoPass *loops;
    ProgramProfile * profile;
    Options *options;
    Formula *AS;

public:
    EncoderPass(Function *_targetFun, Context *_ctx, LoopInfoPass *_loops, 
                ProgramProfile *profile, Options *options);
    ~EncoderPass();
    
    Formula* makeTraceFormula();
    Formula* getASFormula();
    
private:
    void modelControlFlow();
    void initGlobalVariables();
    void initAssertCalls();
    
    ExprPtr constructPartialPart_V3();
    ExprPtr constructPartialPart();
    ExprPtr constructPartialPart_old();
    
};
//============================================================================


#endif