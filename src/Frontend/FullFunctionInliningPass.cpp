/**
 * \file FullFunctionInliningPass.cpp
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
 * \date   28 January 2015
 */

#include "FullFunctionInliningPass.h"


char FullFunctionInliningPass::ID = 0;
Pass *FullFunctionInliningPass::createFullFunctionInliningPass(Options *o) { 
    return new FullFunctionInliningPass(o); 
}


// =============================================================================
// runOnFunction 
// 
// 
// =============================================================================
bool FullFunctionInliningPass::runOnFunction(Function &F) {
    
    //const AttributeSet attr = F.getAttributes(); 
    //const AttributeSet attr_new = attr.addAttribute(~0U, Attribute::AlwaysInline); 
    //F.setAttributes(attr_new); 
    F.addFnAttr(Attribute::AlwaysInline);
    
    // Process function calls
    // x = call f  ->  y = call f; x = y + 0
    std::vector<StoreInst*> worklist;
    for (inst_iterator I = inst_begin(&F), E = inst_end(&F); I != E; ++I) {
        CallInst *C = dyn_cast<CallInst>(&*I);
        if (!C || C->doesNotReturn()) {
                continue;
        }
        for (Value::use_iterator U = C->use_begin(), UE = C->use_end();
             U != UE; ++U) {
            if (StoreInst *S = dyn_cast<StoreInst>(*U)) {
                worklist.push_back(S); //(&*I);
            }
        }
    }
    for (StoreInst *S : worklist) {
        // addVal = retVal + 0
        Value *C = S->getValueOperand();
        BinaryOperator *add =
        BinaryOperator::CreateAdd(C, C, "_ret", S); // name?
        Instruction *I = dyn_cast<Instruction>(C);
        assert(I && "Instruction is null!");
        MDNode *N = I->getMetadata("dbg");
        add->setMetadata("dbg", N);
        // store retVal, *p  ->  store addVal, *p
        StoreInst *newS = new StoreInst(add, S->getPointerOperand());
        BasicBlock::iterator ii(S);
        ReplaceInstWithInst(S->getParent()->getInstList(), ii, newS);
        /*std::cout << "--------------\n";
        C->dump();
        add->dump();
        newS->dump();
        std::cout << "--------------\n";*/
    }

    // Put an extra add just before the 'ret' instruction
    // of this function. This will prevent the propagation
    // of variables into other nstruction after the inlining
    // pass.
    BasicBlock *lastBB = &F.back();
    if (!lastBB) {
        return false;
    }
    Instruction *lastInst = &lastBB->back();
    if (!lastInst) {
        return false;
    }
    if (ReturnInst *RI = dyn_cast<ReturnInst>(lastInst)) {
        // Get the return value
        Value *V = RI->getReturnValue();
        if (!V) { // ret void
            return false;
        }
        if (!dyn_cast<LoadInst>(V)) { 
            return false;
        }
        // Insert a dummy instruction
        Value *zero = ConstantInt::get(V->getType(), 0);
        BinaryOperator *add = 
        BinaryOperator::CreateAdd(V, V, "_ret"+V->getName(), RI);
        // Replace the ret
        BasicBlock::iterator ii(RI);
        LLVMContext &Context = getGlobalContext();
        ReturnInst *newret = ReturnInst::Create(Context, add);
        ReplaceInstWithInst(RI->getParent()->getInstList(), ii, newret);
        // Set debugging metadata
        MDNode *N = RI->getMetadata("dbg");
        if (N) {
            add->setMetadata("dbg", N);
            newret->setMetadata("dbg", N);
        } else {
            RI->dump();
            std::cout << "warning : metadata!" << std::endl;
        }
    } 
    return true;
}