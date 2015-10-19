/**
 * ProgramTrace.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */

#include "ProgramTrace.h"

unsigned ProgramTrace::ID = 0;

ProgramTrace::ProgramTrace(Function *_targetFun)
: myID(ID++), targetFun(_targetFun), type(UNKNOW) {
    inputVars = std::make_shared<Variables>();
    expectedOutput = NULL;
}

ProgramTrace::ProgramTrace(Function *_targetFun, AssertResult _type) 
: myID(ID++), targetFun(_targetFun), type(_type) { 
    inputVars = std::make_shared<Variables>();
     expectedOutput = NULL;
}

ProgramTrace::ProgramTrace(Function *_targetFun, std::vector<Value*> _inputs, 
AssertResult _type) : myID(ID++), targetFun(_targetFun), type(_type) { 

    if (_targetFun->arg_size()!=_inputs.size()) {
        std::cout << "error: wrong execution trace.\n";
        exit(1);
    }
    unsigned i = 0;
    Function::arg_iterator ait;
    for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
        Value *origin = ait;
        Value *concrete = _inputs[i++];
        addProgramInput(origin, concrete);
    }
     expectedOutput = NULL;
}

void ProgramTrace::setExecutedBlocks(std::vector<BasicBlock*> &bb) {
    std::copy(bb.begin(), bb.end(),
              std::inserter(executedBlocks, executedBlocks.end()));
}

void ProgramTrace::addProgramInput(Value *origin, Value *val) {
    InputVarTracePtr vt = std::make_shared<InputVariableTrace>(origin, val);
    inputVars->add(vt);
}

void ProgramTrace::addProgramInput(Value *origin, int val) {
    InputVarTracePtr vt =  std::make_shared<InputVariableTrace>(origin, val);
    inputVars->add(vt);
}

void ProgramTrace::addProgramInput(InputVarTracePtr ivt) {
    inputVars->add(ivt);
}

/*void ProgramTrace::addProgramInput(std::string argvValue) {
 argvs.push_back(argvValue);
 }*/

std::set<BasicBlock*> ProgramTrace::getExecutedBB() {
    if (!executedBlocks.empty()) {
        return executedBlocks;
    }
    std::set<BasicBlock*> bbs;
    BasicBlock *last = NULL;
    for (InstTracePtr i : instructionTraces) {
        Instruction *inst = i->getInstruction();
        if (inst && inst->getParent()!=last) {
            bbs.insert(inst->getParent());
            last = inst->getParent();
        }
    }
    return bbs;
}

// Create the formula that encode the program inputs:
// (and (= var1 val1) (= var2 val2) ...) 
ExprPtr ProgramTrace::getProgramInputsFormula(Formula *formula) {

    std::vector<ExprPtr> and_args;   
    std::vector<InputVarTracePtr> vars = inputVars->getVector();
    std::vector<InputVarTracePtr>::iterator it;
    for(it=vars.begin(); it!=vars.end(); ++it) {
        InputVarTracePtr v = *it;
        ExprPtr val_expr = Expression::mkSInt32Num(v->getInt32());    
        ExprPtr d_expr = Expression::mkIntVar(v->getName());
        and_args.push_back(Expression::mkEq(d_expr, val_expr));
    }
    /*std::vector<std::string>::iterator it2;
    for(it2=argvs.begin(); it2!=argvs.end(); ++it2) {
        std::string s = *it2;  
        ExprPtr e = Expression::parseExpression(s);
        and_args.push_back(e);
    }*/
    // Oracle
    if (expectedOutput!=NULL) {
        // Get the return value of targetFun
        if (targetFun->doesNotReturn()) {
            error("expectedOutput set but fun does not return.");
        }
        BasicBlock *lastBlock = &targetFun->back();
        if (!lastBlock) {
            error("program trace");
        }
        Instruction *lastInst = &lastBlock->back();
        if (!lastInst) {
            error("program trace");
        }
        ReturnInst *RI = dyn_cast<ReturnInst>(lastInst);
        if (!RI) {
            error("program trace");
        }
        Value *V = RI->getReturnValue();
        if (!V) { // ret void
            error("program trace");
        }
        ExprPtr v_expr = Expression::mkIntVar(V->getName().str());
        // Get the concrete value
        ConstantInt *EO = dyn_cast<ConstantInt>(expectedOutput);
        if (!EO) {
            error("program trace");
        }
        int oval = (int) EO->getSExtValue();
        ExprPtr oval_expr = Expression::mkSInt32Num(oval);
        // (= ret_var oval)
        and_args.push_back(Expression::mkEq(v_expr, oval_expr));
    }
    ExprPtr expr;
    if (and_args.size()==1) {
        expr = and_args[0];
    }
    else if (and_args.size()>1) {
        expr = Expression::mkAnd(and_args); 
    } else {
        return NULL;
    }
    return expr;
}

void ProgramTrace::dumpProgramInputs() {
    inputVars->dump();
    /*std::vector<std::string>::iterator it2;
    for(it2=argvs.begin(); it2!=argvs.end(); ++it2) {
        std::string s = *it2; 
        std::cout << s;
        if (it2+1!=argvs.end()) {
            std::cout << " ";
        }
    }*/
}

// RHS-noarg
void ProgramTrace::add(Instruction *i) {
    if(i->getOpcode()==Instruction::Call) {
        // Check return type
        CallInst *call = cast<CallInst>(i);
        Function *calledFun = call->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
            if (!calledFun) {
                std::cout << "warning: unresolvable indirect function call.\n";
                return;
            }
        }
        const Type *retTy = calledFun->getReturnType();
        if (!retTy->isVoidTy()) {
            std::cout << "error: ret call, program trace\n"; 
            exit(1);
        }
    }
    InstTracePtr it = std::make_shared<InstructionTrace>(i);
    instructionTraces.push_back(it);
}

// RHS-1arg
void ProgramTrace::add(Instruction *i, Value *v) {
    if(i->getOpcode()==Instruction::Call) {
        // Check return type
        CallInst *call = cast<CallInst>(i);
        Function *calledFun = call->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
            if (!calledFun) {
                std::cout << "warning: unresolvable indirect function call.\n";
                return;
            }
        }
        const Type *retTy = calledFun->getReturnType();
        if (!retTy->isVoidTy()) {
            std::cout << "error: ret call, program trace\n"; 
            exit(1);
        }
    }
    InstTracePtr it = std::make_shared<InstructionTrace>(i, v);
    instructionTraces.push_back(it);
}

// RHS-2arg
void ProgramTrace::add(Instruction *i, Value *v1, Value *v2) {
    if(i->getOpcode()==Instruction::Call) {
        // Check return type
        CallInst *call = cast<CallInst>(i);
        Function *calledFun = call->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
            if (!calledFun) {
                std::cout << "warning: unresolvable indirect function call.\n";
                return;
            }
        }
        const Type *retTy = calledFun->getReturnType();
        if (!retTy->isVoidTy()) {
            std::cout << "error: ret call, program trace\n"; 
            exit(1);
        }
    }
    InstTracePtr it = std::make_shared<InstructionTrace>(i, v1, v2);
    instructionTraces.push_back(it);
}

// RHS-1ret-2arg
void ProgramTrace::add(Instruction *i, Value *v1, Value *v2, Value *v3) {
    if(i->getOpcode()==Instruction::Call) {
        // Check return type
        CallInst *call = cast<CallInst>(i);
        Function *calledFun = call->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
            if (!calledFun) {
                std::cout << "warning: unresolvable indirect function call.\n";
                return;
            }
        }
        const Type *retTy = calledFun->getReturnType();
        if (!retTy->isVoidTy()) {
            std::cout << "error: ret call, program trace\n";
            exit(1);
        }
    }
    InstTracePtr it = std::make_shared<InstructionTrace>(i, v1, v2, v3);
    instructionTraces.push_back(it);
}

std::vector<InstTracePtr> ProgramTrace::getInstructionTraces() {
    return instructionTraces;
}

// Return the concrete value taken by "v" in this trace
Value* ProgramTrace::getConcreteValue(Value *v) {
    std::vector<InstTracePtr>::iterator it;
    for (it=instructionTraces.begin(); it!=instructionTraces.end(); ++it) {
        InstTracePtr i = *it;
        Value *c = i->getConcrete(v);        
        if (c!=NULL) {
            return c;
        }
    }
    return NULL;
}


// ====== InstructionTrace ======


Value* InstructionTrace::getConcrete(Value *v) {
    std::string vname = v->getName().str();
    if (inst && vname==inst->getName()) {
        return resVal;
    }
    if (argVals.empty()) {
        return NULL;
    }
    for (unsigned i=0; i<inst->getNumOperands(); ++i) {
        std::string oname = inst->getOperand(i)->getName();
        if (vname==oname) {
            return argVals[i];
        }
    }
    return NULL;
}

int InstructionTrace::getInt32(Value *v) { 
    if(ConstantInt *ci = dyn_cast<ConstantInt>(v)) {
        return (int) ci->getSExtValue();
    } else {
        error("no concrete value for variable!");
        return 0;
    }
}

Value* InstructionTrace::emulateInst(Instruction *i, Value *val1, Value *val2) {
    int v1 = -1;
    int v2 = -1;
    if(ConstantInt *ci1 = dyn_cast<ConstantInt>(val1)) {
        v1 = (int) ci1->getSExtValue();
    } else {
        error("no concrete value for variable!");
    }
    if(ConstantInt *ci2 = dyn_cast<ConstantInt>(val2)) {
        v2 = (int) ci2->getSExtValue();
    } else {
        error("no concrete value for variable!");
    }
    int v = emulateInst(i, v1, v2);
    LLVMContext &context = getGlobalContext();
    IRBuilder<> IRB(context);
    Value *cv = IRB.getInt32(v);
    return cv;
}

Value* InstructionTrace::emulateInst(Instruction *i, Value *val1, Value *val2,
                                     Value *val3) {
    int v1 = -1;
    int v2 = -1;
    int v3 = -1;
    if(ConstantInt *ci1 = dyn_cast<ConstantInt>(val1)) {
        v1 = (int) ci1->getSExtValue();
    } else {
        error("no concrete value for variable!");
    }
    if(ConstantInt *ci2 = dyn_cast<ConstantInt>(val2)) {
        v2 = (int) ci2->getSExtValue();
    } else {
        error("no concrete value for variable!");
    }
    if(ConstantInt *ci3 = dyn_cast<ConstantInt>(val3)) {
        v3 = (int) ci3->getSExtValue();
    } else {
        error("no concrete value for variable!");
    }
    int v = emulateInst(i, v1, v2, v3);
    LLVMContext &context = getGlobalContext();
    IRBuilder<> IRB(context);
    Value *cv = IRB.getInt32(v);
    return cv;
}

int InstructionTrace::emulateInst(Instruction *i, int val1, int val2) {
    int r;
    switch (i->getOpcode()) {
        case Instruction::Add:
        case Instruction::FAdd:
            r = val1 + val2; break;
        case Instruction::Sub:
        case Instruction::FSub:
            r = val1 - val2; break;
        case Instruction::Mul:
        case Instruction::FMul:
            r = val1 * val2; break;
        case Instruction::UDiv:
        case Instruction::SDiv:
        case Instruction::FDiv:
            r = val1 / val2; break;
        case Instruction::And:
            r = val1 & val2; break;
        case Instruction::Or:
            r = val1 | val2; break;
        case Instruction::Xor:
            r = val1 ^ val2; break;
        case Instruction::ICmp: {
            ICmpInst *c = cast<llvm::ICmpInst>(i);
            CmpInst::Predicate op = c->getSignedPredicate();
            switch (op) {
                case llvm::CmpInst::FCMP_UEQ: // True if unordered or equal
                case llvm::CmpInst::FCMP_OEQ: // True if ordered and equal
                case llvm::CmpInst::ICMP_EQ:  // equal
                    r = val1 == val2; break;
                case llvm::CmpInst::FCMP_ONE: // True if ordered and operands are unequal
                case llvm::CmpInst::FCMP_UNE: // True if unordered or not equal
                case llvm::CmpInst::ICMP_NE:  // not equal
                    r = val1 != val2; break;
                case llvm::CmpInst::FCMP_UGT: // True if unordered or greater than
                case llvm::CmpInst::FCMP_OGT: // True if ordered and greater than
                case llvm::CmpInst::ICMP_UGT: // unsigned greater than
                case llvm::CmpInst::ICMP_SGT: // signed greater than
                    r = val1 > val2; break;
                case llvm::CmpInst::FCMP_UGE: // True if unordered, greater than, or equal
                case llvm::CmpInst::FCMP_OGE: // True if ordered and greater than or equal
                case llvm::CmpInst::ICMP_UGE: // unsigned greater or equal
                case llvm::CmpInst::ICMP_SGE: // signed greater or equal
                    r = val1 >= val2; break;
                case llvm::CmpInst::FCMP_ULT: //  True if unordered or less than
                case llvm::CmpInst::FCMP_OLT: // True if ordered and less than
                case llvm::CmpInst::ICMP_ULT: // unsigned less than
                case llvm::CmpInst::ICMP_SLT: // signed less than
                    r = val1 < val2; break;
                case llvm::CmpInst::FCMP_OLE: // True if ordered and less than or equal
                case llvm::CmpInst::FCMP_ULE: // True if unordered, less than, or equal
                case llvm::CmpInst::ICMP_ULE: // unsigned less or equal
                case llvm::CmpInst::ICMP_SLE: // signed less or equal
                    r = val1 <= val2; break;
                default:
                    std::cout << "error: Unsupported comaprison operator!\n"; 
                    exit(1);
            }                
        } break;           
        default:    
            std::cout << "error: program trace\n";
            exit(1);
    }
    return r;
}

int InstructionTrace::emulateInst(Instruction *i, int val1, int val2, int val3) {
    int r;
    switch (i->getOpcode()) {
        case Instruction::Select:
            r = val1 ? val2 : val3;
            break;
        default:
            std::cout << "error: program trace\n";
            exit(1);
    }
    return r;
}

void InstructionTrace::dump() {
    if (!inst->isTerminator()) { 
        std::cout << inst->getName().str() << " = ";
    } else {
        std::cout << "br/ret ";
    }
    std::cout << "[";
    std::vector<Value*>::iterator it;
    for (it=argVals.begin(); it!=argVals.end(); ++it) {
        Value *v = *it;
        std::cout << getInt32(v);
        if (it+1!=argVals.end()) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}
