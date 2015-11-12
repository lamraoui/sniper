/**
 * LocalVariables.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simohamed.lamraoui@gmail.com
 * @date : 2012/03/11
 * @copyright : NII 2012
 */

#include "LocalVariables.h"


// =============================================================================
// LocalVariables 
// 
// Constructor
// 
// =============================================================================
LocalVariables::LocalVariables(Function *_mainFun) {
    this->mainFun = _mainFun;
}


// =============================================================================
// process
//
// =============================================================================
void LocalVariables::processLoadStore() {
    
    // Iterate through the function
    std::vector<StoreInst*> stores;  
    inst_iterator ii0;
    for (ii0 = inst_begin(mainFun); ii0!=inst_end(mainFun); ii0++) {
        Instruction &i = *ii0;
        
        // Save the local variable positions
        switch (i.getOpcode()) {
            //case Instruction::ZExt:
            //     break;
            //case Instruction::Call:
            //     break
            case Instruction::ICmp:
            case Instruction::Add:
            case Instruction::FAdd:
            case Instruction::Sub:
            case Instruction::FSub:
            case Instruction::Mul:
            case Instruction::FMul:
            case Instruction::UDiv:
            case Instruction::SDiv:
            case Instruction::FDiv:
            case Instruction::And:
            case Instruction::Or:
            case Instruction::Xor:
            case Instruction::Shl:
            case Instruction::LShr:   
            case Instruction::AShr: {
                Value *a1 = (&i)->getOperand(0);
                Value *a2 = (&i)->getOperand(1);
                if (LoadInst *load = dyn_cast<LoadInst>(a1))
                    set(&i, load, 0);
                if (LoadInst *load = dyn_cast<LoadInst>(a2)) 
                    set(&i, load, 1);
            } break;
            case Instruction::Switch: {
                SwitchInst *s = cast<SwitchInst>(&i);
                Value *cond = s->getCondition();
                if (LoadInst *load = dyn_cast<LoadInst>(cond))
                    set(&i, load, 0);
            } break;
            case Instruction::Br: {
                BranchInst *br = cast<BranchInst>(&i);
                if(!br->isUnconditional()) {
                    Value *cond = br->getCondition();
                    if (LoadInst *load = dyn_cast<LoadInst>(cond))
                        set(&i, load, 0);
                }
            } break;
            case Instruction::Store: {
                StoreInst *s = cast<StoreInst>(&i);
                bool save = true;
                // Do not process binary instruction
                Value *vop = s->getValueOperand();
                if (Instruction *I = dyn_cast<Instruction>(vop)) {
                    if (I->isBinaryOp()) {
                        save = false;
                    }
                }
                // Do not process pointers                
                if (isa<GetElementPtrInst>(s->getPointerOperand())) {
                    save = false;
                }
                if (save) {
                    stores.push_back(s);
                }
            } break;                
            default:
                break;
        } 
    }
    // Iterate through the saved store insts
    std::vector<StoreInst*>::iterator it;
    for(it = stores.begin(); it != stores.end(); ++it) {
        StoreInst *s = *it;
        Value *vop = s->getValueOperand();
        Value *D = NULL;
        if(LoadInst *load = dyn_cast<LoadInst>(vop)) {
            D = load;
        } else {
            // Store of a function argument?
            bool isFunArg = false;
            Function::const_arg_iterator ait;
            for (ait=mainFun->arg_begin(); ait!=mainFun->arg_end(); ++ait) {
                const Argument &arg = *ait;
                if (arg.getName()==vop->getName()) {
                    isFunArg = true;
                    break;
                }
            }
            if (!isFunArg) {
                D = vop;
            }
        }
        if (D && !D->getType()->isPointerTy()) {
            // Get debugging metadata
            MDNode *storeNode = s->getMetadata("dbg");
            //if (!storeNode) {
            //    std::cout << "warning : metadata!" << std::endl;
            //}
            // Insert a dummy instruction (that will be erase)
            Value *pop = s->getPointerOperand();
            Value *zero = ConstantInt::get(D->getType(), 0);
            BinaryOperator *add =
            BinaryOperator::CreateAdd(D, zero, "_"+pop->getName(), s);
            // Replace the store
            BasicBlock::iterator ii(s);
            StoreInst *newstore = new StoreInst(add, pop);
            ReplaceInstWithInst(s->getParent()->getInstList(), ii, newstore);
            // Set debugging metadata
            if (storeNode) {
                add->setMetadata("dbg", storeNode);
                newstore->setMetadata("dbg", storeNode);
            }
        }
    }
    // Validate the generated code,
    // checking for consistency.
    //verifyFunction(*mainFun);
}


// =============================================================================
// processPhi
//
// =============================================================================
void LocalVariables::processPhi() {

    // Iterate through the function
    inst_iterator ii0;
    for (ii0 = inst_begin(mainFun); ii0!=inst_end(mainFun); ii0++) {
        Instruction &i = *ii0;
        // Save the local variable positions
        switch (i.getOpcode()) {
            case Instruction::PHI: {
                PHINode *phi = cast<PHINode>(&i);
                std::string phiName = phi->getName();
                if (!hasEnding(phiName, ".0")) {
                    break;
                }
                bool ok = true;
                for (unsigned j=0; j<phi->getNumIncomingValues(); j++) {
                    Value *val = phi->getIncomingValue(j);
                    if(!isa<ConstantInt>(val)) {
                        ok = false; break;
                    }
                }
                /*if (ok) {
                    int line = this->ptr2line[phiName.substr(0,phiName.length()-2)];
                    std::cout << "PHI-LINE: " << line << std::endl;
                }*/
                
            } break;                
            default:
                break;
        } 
    }
                
    
}


// =============================================================================
// getName
// 
// =============================================================================
std::string LocalVariables::getPtr(Instruction *i, int pos) {
    
    std::vector<varArg_t>::iterator it;
    for(it = vars.begin(); it != vars.end(); ++it) {
        varArg_t v = *it;
        if (v.inst==i && v.pos==pos) {
            return v.name;
        } 
    }
    return "";
}

// =============================================================================
// getLine
// 
// =============================================================================
unsigned LocalVariables::getLine(std::string name) {
    return this->ptr2line[name];
}


// =============================================================================
// set 
// 
// =============================================================================
void LocalVariables::set(Instruction *i, LoadInst *load, int pos) {
    
    varArg_t v;
    v.inst = i;
    v.pos = pos;
    v.name = load->getPointerOperand()->getName();
    // Type
    vars.push_back(v);    
}


// =============================================================================
// hasEnding
//
// =============================================================================
bool LocalVariables::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

LocalVariables::~LocalVariables() {

}