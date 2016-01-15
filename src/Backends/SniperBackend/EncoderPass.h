/**
 * EncoderPass.h
 *
 * This class can be used to encode an LLVM function
 * into a logic formula.
 *
 * Note: This class uses a light version of Encoder.
 * In the light version, the control flow only consists
 * of contraints of branch and phi instructions.
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simohamed.lamraoui@gmail.com
 * @date : 2015/12/18
 * @copyright : NII 2014 & Hosei 2015
 */

#ifndef _ENCODERPASS_H
#define _ENCODERPASS_H

#include <map>
#include <ctime>
#include <sys/time.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

#include "Options.h"
#include "Frontend/Frontend.h"
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
                ProgramProfile *_profile, Options *_options)
                : targetFun(_targetFun), ctx(_ctx), loops(_loops),
                profile(_profile), options(_options) {
        this->encoder = new Encoder(ctx);
        // Create an empty AS formula (for pre- and post-condition)
        this->AS = new Formula();
    }
    
    EncoderPass() {
        delete encoder;
    }
    Formula* makeTraceFormula();
    Formula* getASFormula();
    
private:
    void initGlobalVariables();
    void initAssertCalls();
    bool isAtoiFunction(Instruction *I);
    
};
//============================================================================

#endif