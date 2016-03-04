/**
 * \file GlobalVariables.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   29 January 2015
 */


#include "GlobalVariables.h"


// =============================================================================
// process 
// 
// TODO: inbound GEPI?
// 
//
//
// =============================================================================
void GlobalVariables::process(Function *targetFun) {

    // Insert an Alloca instruction in the main function
    // entry block for each GV declaration
    Instruction *firstInst = targetFun->getEntryBlock().begin();
    Module *m = targetFun->getParent();
    Module::global_iterator iter = m->global_begin();
    Module::global_iterator end = m->global_end();
    for (; iter != end; ++iter) {
        Value *gv = iter;
        const PointerType *pty = dyn_cast<PointerType>(gv->getType());
        Type *ty = pty->getElementType();
        AllocaInst *a = new AllocaInst(ty, 0, "a"+gv->getName(), firstInst);
        this->gv2a[gv] = a; 
    }    
    
    // Iterate over the instructions
    inst_iterator ii0;
    for (ii0 = inst_begin(targetFun); ii0!=inst_end(targetFun); ii0++) {
        Instruction &i = *ii0;
        
        switch (i.getOpcode()) {
            case Instruction::Load: {
                
                replacePointer(&i);

            } break;
            case Instruction::Store: {
                
                replacePointer(&i);
            
            } break;
            
            // TODO
            case Instruction::GetElementPtr: {
                GetElementPtrInst *gepi = cast<GetElementPtrInst>(&i);
                Value *ptr = gepi->getPointerOperand();
                if(GlobalVariable *gv = dyn_cast<GlobalVariable>(ptr)) {
                    i.setOperand(0, this->gv2a[gv]);
                }
            } break;
            default:
                break;
        } 
    }
    //verifyFunction(*fun);
    //fun->dump();
    //MSG("---------------------------------------------------\n");
}


// =============================================================================
// replacePointer
// 
//
// =============================================================================

void GlobalVariables::replacePointer(Instruction *inst) {
    Value *ptr;
    unsigned opId;
    if (LoadInst *l = dyn_cast<LoadInst>(inst)) {
        ptr = l->getPointerOperand();
        opId = 0;
    }
    else if (StoreInst *s = dyn_cast<StoreInst>(inst)) {
        ptr = s->getPointerOperand();
        opId = 1;
    } else {
        std::cout << "error: replace pointer!\n";
        exit(1);
    }

    // Basic type pointer
    if(GlobalVariable *gv = dyn_cast<GlobalVariable>(ptr)) {
        // First load for this GV
        if (opId==0 && !gv2val[gv]) {
            if(gv->hasInitializer()) {
                Value *initVal = gv->getInitializer();
                new StoreInst(initVal, gv2a[gv], inst);  
            } 
            else if (gv->hasExternalLinkage()) {
                // Nothing to do
            } else {
                std::cout << "error: load from a non-initilized global varibale!\n";
                exit(1);
            }
        }         
        // Replace the pointer argument
        inst->setOperand(opId, this->gv2a[gv]);
        // Update the map
        this->gv2val[gv] = inst;
    }
    // Array type pointer
    else if(GetElementPtrInst *gepi = dyn_cast<GetElementPtrInst>(ptr)) {
        if(GlobalVariable *gv = dyn_cast<GlobalVariable>(gepi->getPointerOperand())) {
            // Replace the pointer argument
            gepi->setOperand(opId, gv2a[gv]);
            // Update the map
            this->gv2val[gv] = inst;
        }
    }else if(GEPOperator *gep = dyn_cast<GEPOperator>(ptr)) {
        if(GlobalVariable *gv = dyn_cast<GlobalVariable>(gep->getPointerOperand())) {
            // Replace the GEP instruction by a GEPI
            std::vector <Value*> idxs;
            for (unsigned i = 1; i < gep->getNumIndices()+1; ++i) {
                idxs.push_back(gep->getOperand(i));
            }
            GetElementPtrInst *gepi = 
            GetElementPtrInst::CreateInBounds(this->gv2a[gv], 
                                              idxs, "", inst);
            // Replace the pointer argument by the 
            // newly created GEPI instruction
            inst->setOperand(opId, gepi);
            // Update the map
            this->gv2val[gv] = inst;
        }
    }
}