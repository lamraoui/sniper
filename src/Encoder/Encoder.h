/**
 * \file Encoder.h
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 * \author Si-Mohamed Lamraoui
 * \date   3 March 2016
 */

#ifndef _ENCODER_H
#define _ENCODER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set> 
#include <algorithm>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/DebugInfo.h"
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/InstIterator.h>
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/PostOrderIterator.h"

#include "Options.h"
#include "Logic/Expression.h"
#include "Logic/Formula.h"
#include "Frontend/Frontend.h"
#include "Frontend/LocalVariables.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Encoder/Context.h"

using namespace llvm;

/**
 * \class Encoder
 *
 * This class provides functions to encode LLVM instructions
 * into logic formulas (expressions).
 *
 * Note: This is the light version of Encoder.
 * In this version, the program "skeleton" only consists
 * of constraints encoded from branch and phi instructions.
 */
class Encoder {

private:
    Context *ctx;
    
public:
    /**
     * Default constructor.
     *
     * \param _ctx A context.
     */
    Encoder(Context *_ctx) : ctx(_ctx)  { }
    /**
     * Destructor.
     */
    ~Encoder() { }
    
    /**
     * Traverse the IR graph to create all transitions variables 
     * so that they are available for encoding the branch and phi 
     * instructions.
     * 
     * \param targetFun An LLVM function to process.
     */
    void prepareControlFlow(Function *targetFun);

    /**
     * Encode an LLVM instruction which computes a 
     * binary operation into a logic expression. 
     * Supported binary operations are as follows. 
     * Arithmetic operations: Add, Sub, Mul, and Rem. 
     * Boolean operations: And, Or, Xor, Shl, LShr, and AShr.
     *
     * \param bo An LLVM binary operation.
     * \return A logic expression.
     */
    ExprPtr encode(BinaryOperator *bo);
    /**
     * Encode an LLVM select instruction into a 
     * logic expression.
     *
     * \param select An LLVM select instruction.
     * \return A logic expression.
     */
    ExprPtr encode(SelectInst *select);
    /**
     * Encode an LLVM phi instruction into a 
     * logic expression.
     * If a basic block \p forBlock is passed as 
     * argument, encode \p phi with only the incoming value 
     * of \p forBlock. 
     *
     * \param phi An LLVM phi instruction.
     * \param forBlock An LLVM basic block.
     * \return A logic expression.
     */
    ExprPtr encode(PHINode *phi, BasicBlock *forBlock = NULL);
    /**
     * Encode an LLVM branch instruction into a 
     * logic expression.
     *
     * \param br An LLVM branch instruction.
     * \param loops Information about loops in the IR.
     * \return A logic expression.
     */
    ExprPtr encode(BranchInst *br, LoopInfoPass *loops);
    /**
     * Encode an LLVM switch instruction into a 
     * logic expression.
     *
     * \param switch An LLVM switch instruction.
     * \return A logic expression.
     */
    ExprPtr encode(SwitchInst *s);
    /**
     * Encode an LLVM comparison instruction into a 
     * logic expression.
     *
     * \param icmp An LLVM comparison instruction.
     * \return A logic expression.
     */
    ExprPtr encode(ICmpInst *icmp);
    /**
     * Encode an LLVM memory allocation instruction into a 
     * logic expression.
     *
     * \param alloca An LLVM alloca instruction.
     * \return A logic expression.
     */
    ExprPtr encode(AllocaInst *alloca);
    /**
     * Encode an LLVM memory store instruction into a 
     * logic expression.
     *
     * \param store An LLVM store instruction.
     * \return A logic expression.
     */
    ExprPtr encode(StoreInst *store);
    /**
     * Encode an LLVM memory load instruction into a 
     * logic expression. 
     * The encoding may add post-conditions to \p AS 
     * in order to force the load operation to behave 
     * correctly. 
     *
     * \param load An LLVM load instruction.
     * \param AS A program specification formula.
     * \return A logic expression.
     */
    ExprPtr encode(LoadInst *load, Formula *AS);
    /**
     * Encode an LLVM get element pointer instruction 
     * into a logic expression.
     *
     * \param gep An LLVM GEP instruction.
     * \return A logic expression.
     */
    ExprPtr encode(GetElementPtrInst *gep);
     /**
     * Encode an LLVM cast instruction into a 
     * logic expression.
     *
     * \param sext An LLVM cast instruction.
     * \return A logic expression.
     */
    ExprPtr encode(SExtInst *sext);
    /**
     * Encode an LLVM cast instruction into a 
     * logic expression.
     *
     * \param sext An LLVM cast instruction.
     * \return A logic expression.
     */
    ExprPtr encode(ZExtInst *zext);
    /**
     * Encode an LLVM function call instruction 
     * into a logic expression.
     *
     * \param call An LLVM call instruction.
     * \param preCond A pre-condition for the function being called.
     * \param postCond A post-condition for the function being called.
     * \return A logic expression.
     */
    ExprPtr encode(CallInst *call, Formula *preCond, Formula *postCond);
    /**
     * Encode an LLVM function return instruction 
     * into a logic expression.
     *
     * \param i An LLVM return instruction.
     * \return A logic expression.
     */
    ExprPtr encode(ReturnInst *i);
    
    //@{
    /** LLVM instruction encoding function 
     *  is not yet implemented.
     */
    ExprPtr encode(FenceInst *i);
    ExprPtr encode(AtomicCmpXchgInst *i);    
    ExprPtr encode(AtomicRMWInst *i);    
    ExprPtr encode(FCmpInst *i);    
    ExprPtr encode(VAArgInst *i);
    ExprPtr encode(ExtractElementInst *i);
    ExprPtr encode(InsertElementInst *i); 
    ExprPtr encode(ShuffleVectorInst *i);    
    ExprPtr encode(ExtractValueInst *i);    
    ExprPtr encode(InsertValueInst *i);    
    ExprPtr encode(LandingPadInst *i);
    ExprPtr encode(IndirectBrInst *i);
    ExprPtr encode(InvokeInst *i);  
    ExprPtr encode(ResumeInst *i);  
    ExprPtr encode(UnreachableInst *i); 
    ExprPtr encode(TruncInst *i);  
    ExprPtr encode(FPTruncInst *i); 
    ExprPtr encode(FPExtInst *i);   
    ExprPtr encode(UIToFPInst *i); 
    ExprPtr encode(SIToFPInst *i); 
    ExprPtr encode(FPToUIInst *i);  
    ExprPtr encode(FPToSIInst *i);  
    ExprPtr encode(IntToPtrInst *i); 
    ExprPtr encode(PtrToIntInst *i); 
    ExprPtr encode(BitCastInst *i);  
    //@}

private:
    /**
     * Return true if \p V is an argument 
     * of the main function, otherwise false.
     */
    bool isArgv(const Value* V);
    
};

#endif // _ENCODER_H