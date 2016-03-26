/**
 * \file EncoderPass.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   7 March 2016
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

/**
 * \class EncoderPass
 *
 * This class is used to encode an LLVM function
 * into a logic formula, a trace formula.
 *
 * Note: This class uses a light version of Encoder.
 * In the light version, the control flow only consists
 * of constraints of branch and phi instructions.
 */
class EncoderPass {
    
private:
    Function *targetFun;
    Context *ctx;
    Encoder *encoder;
    LoopInfoPass *loops;
    ProgramProfile * profile;
    Options *options;
    Formula *preCond;
    Formula *postCond;

public:
    /**
     * Default constructor.
     */
    EncoderPass(Function *_targetFun, Context *_ctx, LoopInfoPass *_loops,
                ProgramProfile *_profile, Options *_options)
                : targetFun(_targetFun), ctx(_ctx), loops(_loops),
                profile(_profile), options(_options) {
        this->encoder = new Encoder(ctx);
        // Create empty pre- and post-condition formulas
        this->preCond  = new Formula();
        this->postCond = new Formula();
    }
    /**
     * Destructor.
     */
    EncoderPass() {
        delete encoder;
    }

    /**
     * Return a trace formula that encodes the function 
     * EncoderPass::targetFun.
     */
    Formula* makeTraceFormula();
    /**
     * Return a formula that represents the pre-conditions 
     * for the function EncoderPass::targetFun.
     */
    Formula* getPreCondition();
    /**
     * Return a formula that represents the post-conditions 
     * for the function EncoderPass::targetFun.
     */
    Formula* getPostCondition();
    
private:
    /**
     * Process global variables of the parent module of 
     * the function EncoderPass::targetFun.
     * [NOT YET IMPLEMENTED!]
     */
    void initGlobalVariables();
    /**
     * Check the line numbers of all instructions 
     * that are related to assert or assume functions. 
     */
    void initAssertCalls();
    /**
     * Return true if \p I is a call to an "atoi" function, 
     * otherwise false.
     */
    bool isAtoiFunction(Instruction *I);
    
};

#endif // _ENCODERPASS_H