/**
 * Executor.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */

#include "Executor.h"

bool            Executor::Terminated;
unsigned        Executor::MaxDepth;
unsigned        Executor::AllLocDefinite;
bool            Executor::DisabledSymbolicExe;
bool            Executor::DisabledSymbolicExeCurRun;
bool            Executor::CollectBlockTraces;
bool            Executor::NoAssert;
unsigned        Executor::RunID;
unsigned        Executor::GotoCount;
unsigned        Executor::InvokeCount;
AssertResult    Executor::LastRunAssertResult;
SymbolMap*      Executor::SMAP;
SymbolMap*      Executor::PMAP;
SymbolicPath*   Executor::Path;
ProgramTrace*   Executor::Trace;
ProgramProfile* Executor::Profile;
Function*       Executor::TargetFun;
BasicBlock*     Executor::LastExecutedBB;
BasicBlock*     Executor::NextBB;
LoopInfoPass*   Executor::PathLoopInfo;
Formula*        Executor::PathFormula;
Encoder*        Executor::PathEncoder;
Context*        Executor::PathContext;
std::vector<SymbolPtr>   Executor::argStack;
std::vector<BasicBlock*> Executor::ExecutedBlocks; // for HFTF


Status Executor::status = IDLE;


void Executor::init(LoopInfoPass *lip, ProgramProfile *p,
                    bool collectBlockTraces,
                    bool disableSymbExe, bool noAssert) {
    
    assert(lip && "Expecting a loopinfo pass!");
    assert(p && "Expecting a program profile!");
    assert(status==IDLE && "Wrong status for Executor (!=IDLE)!");
    PathLoopInfo        = lip;
    Profile             = p;
    LastRunAssertResult = UNKNOW;
    NoAssert            = noAssert;
    RunID               = 0;
    GotoCount           = 0;
    InvokeCount         = 0;
    AllLocDefinite      = true;
    CollectBlockTraces  = collectBlockTraces;
    DisabledSymbolicExe = disableSymbExe; // default: false
    DisabledSymbolicExeCurRun = DisabledSymbolicExe;
    if (!DisabledSymbolicExe) {
        SMAP = new SymbolMap();
        PMAP = new SymbolMap();
        Path = new SymbolicPath();
    } else {
        SMAP = NULL;
        PMAP = NULL;
        Path = NULL;
    }
    LastExecutedBB = NULL;
    NextBB         = NULL;
    PathFormula    = NULL;
    PathEncoder    = NULL;
    PathContext    = NULL;
    Trace          = NULL;
    status         = INIT;
}

void Executor::start(Function *f, VariablesPtr vals, LocalVariables *lv) {

    assert(f && "Expecting a function!");
    assert((vals && lv) && "Expecting variables!");
    assert((status==INIT || status==IDLE) &&
           "Wrong status for Executor (!=INIT/IDLE)!");
    TargetFun = f;
    if (!DisabledSymbolicExe) {
        // Create a new context and a new
        // formula to hold the next path
        PathContext = new Context(lv);
        PathEncoder = new Encoder(PathContext);
        PathFormula = new Formula();
        // Reset symbolic path
        Path->clear();
    }
    // Create a program trace 
    Trace = new ProgramTrace(f);
    // Initialize the input symbols for the main function
    // TODO : Function of type main(argc,argv)
    // Function of type foo(arg1,...,argn)
    // Retrieve all main function arguments
    std::vector<InputVarTracePtr> valsVec = vals->getVector();
    unsigned i = 0;
    const FunctionType *FTy = f->getFunctionType();
    Function::arg_iterator ait;
    for (ait = f->arg_begin(); ait != f->arg_end(); ++ait) {
        // Check the type of the argument
        const unsigned argNo = ait->getArgNo();
        const Type *argTy = FTy->getParamType(argNo);
        assert(argTy->isIntegerTy(32) && "Unsuported function arg type!");
        if (!DisabledSymbolicExe) {
            SMAP->addInput(ait);
        }
        assert(i<valsVec.size() && "Function arg!");
        Trace->addProgramInput(valsVec[i++]);
    }
    status = START;
}

// Cast, Phi, Memory operations
void Executor::ExecuteInst(Instruction *i) {
    
    assert(i && "Expecting an instruction!");
    assert(status==START && "wrong status for Executor (!=START)!");
    ExprPtr expr = NULL;
    switch (i->getOpcode()) {
        case Instruction::SExt:
            executeCast(i);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<SExtInst>(i));
            }
            break;
        case Instruction::ZExt:
            executeCast(i);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<ZExtInst>(i));
            }
            break;
        case Instruction::GetElementPtr:
            //executeGep(i);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<GetElementPtrInst>(i));
            }
            break;
        case Instruction::BitCast:
            std::cout << "bitcast\n";
            break;
        case Instruction::Alloca:
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<AllocaInst>(i));
            }
            break;
        default:   
            assert("test-executor");
    }
    if (expr && !DisabledSymbolicExeCurRun) {
        expr->setHard();
        PathFormula->add(expr);
    }
}

// Call, Assert, Branch
void Executor::ExecuteInst1(Instruction *i, Value *val) {

    assert(i && "Expecting an instruction!");
    assert(val && "Expecting a value!");
    assert(status==START && "Wrong status for Executor (!=START)!");
    ExprPtr expr = NULL;
    switch (i->getOpcode()) {
        case Instruction::GetElementPtr:
            //executeGep(i);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<GetElementPtrInst>(i));
            }
            break;
        case Instruction::Store:
            executeStore(i, val);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<StoreInst>(i));
            }
            break;
        case Instruction::Load:
            executeLoad(i, val);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<LoadInst>(i), PathFormula);
            }
            break;
        case Instruction::PHI:
            executePhi(i);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<PHINode>(i), LastExecutedBB);
            }
            break;
        case Instruction::Br: {
            ConstantInt *CI = dyn_cast<ConstantInt>(val);
            assert(CI && "Unsupported value!");
            const bool cond = CI->getSExtValue();
            executeBranch(i, cond);
        } break;
        // TODO
        case Instruction::Call:
            executeCall(i, 0, 0);
            break;
        default:    
            assert("test-executor");
    }
    if (expr && !DisabledSymbolicExeCurRun) {
        expr->setHard();
        PathFormula->add(expr);
    }
}

// BinaryOp (Add, ICmp,...), Call
void Executor::ExecuteInst2(Instruction *i, Value *val1, Value *val2) {

    assert(i && "Expecting an instruction!");
    assert((val1 && val2) && "Expecting values!");
    assert(status==START && "wrong status for Executor (!=START)!");
    ExprPtr expr = NULL;
    switch (i->getOpcode()) {
        case Instruction::Add:
        case Instruction::FAdd:
        case Instruction::Sub:
        case Instruction::FSub:
        case Instruction::Mul:
        case Instruction::FMul:
        case Instruction::UDiv:
        case Instruction::SDiv:
        case Instruction::FDiv:
        case Instruction::URem:
        case Instruction::SRem:
        case Instruction::FRem:
        case Instruction::And:
        case Instruction::Or:
        case Instruction::Xor: {
            executeBinaryOp(i, val1, val2);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<BinaryOperator>(i));
            }
        } break;
        case Instruction::ICmp: {
            executeBinaryOp(i, val1, val2);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<ICmpInst>(i));
            }
        } break;
        case Instruction::Call: // Call type foo(arg) / void foo(arg1,arg2)
            executeCall(i, val1, val2);
            break;
        default:    
            assert("test-executor");
    }
    if (expr && !DisabledSymbolicExeCurRun) {
        expr->setHard();
        PathFormula->add(expr);
    }
}

// Call
void Executor::ExecuteInst3(Instruction *i, Value *val1,
                            Value *val2, Value *val3) {

    assert(i && "Expecting an instruction!");
    assert((val1 && val2 && val3) && "Expecting values!");
    assert(status==START && "Wrong status for Executor (!=START)!");
    ExprPtr expr = NULL;
    switch (i->getOpcode()) {
        case Instruction::Select:
            // Select
            executeSelect(i, val1, val2, val3);
            if (!DisabledSymbolicExe) {
                expr = PathEncoder->encode(cast<SelectInst>(i));
            }
            break;
        case Instruction::Call:
            // Call type foo(arg) / void foo(arg1,arg2)
        default:    
            assert("test-executor");
    }
    if (expr && !DisabledSymbolicExeCurRun) {
        expr->setHard();
        PathFormula->add(expr);
    }
}

void Executor::executeGep(Instruction *i) {
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    assert(i && "Expecting an instruction!");
    GetElementPtrInst *gep = (GetElementPtrInst*) i;
    assert(gep->getNumIndices()<=2 && "Unsupported gep instruction");
    if(AllocaInst *a = dyn_cast<AllocaInst>(gep->getPointerOperand())) {
        Type *ty = a->getAllocatedType();
        if (ty->isIntegerTy()) {
            // Incompleteness: pointer dereferencement on symbolic value
            AllLocDefinite = false;
            return;
        }
    }
    unsigned idx = 0;
    Type *subTy = NULL;
    Type *ptrOpTy = gep->getPointerOperandType();    
    if (ptrOpTy && ptrOpTy->getNumContainedTypes()==1) {
        subTy = ptrOpTy->getContainedType(0);
    }
    if (subTy && subTy->getNumContainedTypes()>0) {
        idx = 1;
    } else {
        assert("Unsupported gep instruction!");
    }
    Value *ptr = (Value*) i;
    if(gep->hasIndices()) {
        Value *index = gep->getOperand(idx+1);
        if (SMAP->contains(index)) {
            // P[v->sv] // sv is a new symbolic value
            // Value *a = gep->getPointerOperand();
            // ptr = (a index)  
            //SymbolPtr Sindex = SMAP->get(index);
            //PMAP->createGepOp(ptr, a, Sindex);
            
            // Incompleteness: pointer dereferencement on symbolic value
            AllLocDefinite = false;
        } else {
            // P = P - ptr
            PMAP->remove(ptr);
        }
    }
    // Global allocation
    /*else if(isa<GlobalVariable>(ptr)) {
        // ptr in domain(P)
        if (PMAP->contains(ptr)) {
            SymbolPtr Pptr = PMAP->get(ptr);
            // P' = P[ptr->S(val)]
            PMAP->update(ptr, Pptr);
        } else {
            // P = P - ptr
            PMAP->remove(ptr);
        }
    }*/
}


void Executor::executeStore(Instruction *i, Value *addr) {    
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    assert(i && "Expecting an instruction!");
    StoreInst *store = (StoreInst*) i;
    Value *val = store->getValueOperand();
    // val is a constant value 
    if (isa<ConstantInt>(val)) {
        // P = P - ptr
        PMAP->remove(addr);
    } 
    // val in domain(S)
    else if (SMAP->contains(val)) {
        SymbolPtr Sval = SMAP->get(val);
        // P' = P[ptr->S(val)]
        PMAP->update(addr, Sval);
    } else {
        // S = S - val
        SMAP->remove(val);
        // P = P - ptr
        PMAP->remove(addr);
    }
}

void Executor::executeLoad(Instruction *i, Value *addr) {
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    assert(i && "Expecting an instruction!");
    assert(addr && "Expecting an address!");
    Value *val = (Value*) i;
    // ptr in domain(P)
    if (PMAP->contains(addr)) {
        SymbolPtr Pptr = PMAP->get(addr);
        // S = S[val->P(ptr)]
        SMAP->update(val, Pptr);
    } else {
        // S = S - val
        SMAP->remove(val);
        // P = P - ptr
        PMAP->remove(addr);
    }
}

// Trunc, Zext, Sext
void Executor::executeCast(Instruction *i) {
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    assert(i && "Expecting an instruction!");
    Value *v   = i;
    Value *v1  = i->getOperand(0);
    std::string name = i->getName().str();
    // v1 is a constant value 
    if (isa<ConstantInt>(v1)) {
        // S = S - v
        SMAP->remove(v);
    } 
    // v1 is a variable
    else {
        if (SMAP->contains(v1)) {            
            SymbolPtr Sv1 = SMAP->get(v1);
            // Sext cast (i32->i64) or (i64->i32)
            if ((v->getType()->isIntegerTy(32) && v1->getType()->isIntegerTy(64))
                || (v->getType()->isIntegerTy(64) && v1->getType()->isIntegerTy(32))) {
                // S = S[v->S(v1)]
                SMAP->update(v, Sv1);
            } else {
                // S[v->sv] // sv is a new symbolic value
                // (Sv = cast Sv1) 
                SMAP->createCastOp(v, Sv1);
            }
        } else {
            // S = S - v
            SMAP->remove(v);
        }
    }
}

void Executor::executeCall(Instruction *i, Value *input, Value *output) {
    assert(i && "Expecting an instruction!");
    assert((input && output) && "Expecting values!");
    if (!DisabledSymbolicExeCurRun) {
        // (v = val) 
        Value *v = i;
        SMAP->remove(v);
    }
    // Retrieve the function
    /*CallInst *call = (CallInst*) i;
    Function *F = call->getCalledFunction();
    if (!F) {
        // Indirect call
        F = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
        if (!F) {
            std::cout << "warning: unresolvable indirect function call.\n";
            return;
        }
    }
    // Save the function summary (inputs/output values)
    Profile->addFunSummary(F, input, output);*/
}

static const std::string prefix("_");
void Executor::executeBinaryOp(Instruction *i, Value *value1, Value *value2) {
    
    assert(i && "Expecting an instruction!");
    assert((value1 && value2) && "Expecting values!");
    
    // TMP
    ConstantInt *CI1 = dyn_cast<ConstantInt>(value1);
    int val1 = CI1->getSExtValue();
    ConstantInt *CI2 = dyn_cast<ConstantInt>(value2);
    int val2 = CI2->getSExtValue();
    
    Value *v   = i;
    Value *v1  = i->getOperand(0);
    Value *v2  = i->getOperand(1);
    int opCode = i->getOpcode();
    std::string name = i->getName().str();
    
    // Dummy/BinaryOp (v = v1) 
    if(opCode==Instruction::Add && !name.compare(0, prefix.size(), prefix)) 
    {
        if (DisabledSymbolicExeCurRun) {
            return;
        }
        // v1 is a constant value 
        if (isa<ConstantInt>(v1)) {
            // S = S - v
            SMAP->remove(v);
        } 
        // v1 is a variable
        else {
            if (SMAP->contains(v1)) {
                // S = S[v ->S(v1)]
                SMAP->update(v, v1);
            } else {
                // S = S - v
                SMAP->remove(v);
            }
        }
    }
    // v = v1 op v2
    else 
    {
        // Save the nl-summary (the input values of i)
        // if the operation is non-linear
        if (isNonLinear(i)) {
            if(MDNode *N = i->getMetadata("dbg")) {
                DILocation Loc(N);
                //Profile->addNLOpSummary(Loc, value1, value2);
            } else {
                std::cout << "error: concolic execution module\n";
            }
        } 
        if (DisabledSymbolicExeCurRun) {
            return;
        }
        // v1 in domain(S) & v2 in domain(S)
        if (SMAP->contains(v1) && SMAP->contains(v2)) 
        {
            SymbolPtr Sv1 = SMAP->get(v1);
            SymbolPtr Sv2 = SMAP->get(v2);
            // S[v->si] // si is a new symbolic value
            // Only linear constraints supported
            if (isNonLinear(i)) {  
                if (NON_LINEAR_SUPPORTED) {
                    // Report(S(v)= S(v1) op v2)
                    Value *cst_v2 = ConstantInt::get(v2->getType(), val2);                
                    SMAP->createBinaryOp(v, Sv1, cst_v2);
                } else {
                    // S = S - v
                    SMAP->remove(v);
                }
            } else {
                // Report(S(v)= S(v1) op S(v2))
                SMAP->createBinaryOp(v, Sv1, Sv2);
            }            
        } 
        // v1 in domain(S)
        else if(SMAP->contains(v1)) {
            SymbolPtr Sv1 = SMAP->get(v1);
            // S[v->si]
            // Report(S(v)= S(v1) op v2)
            Value *cst_v2 = ConstantInt::get(v2->getType(), val2);
            SMAP->createBinaryOp(v, Sv1, cst_v2);
        }
        // v2 in domain(S)
        else if(SMAP->contains(v2)) {
            SymbolPtr Sv2 = SMAP->get(v2);
            // S[v->si]
            // Report(S(v)= v1 op S(v2))
            Value *cst_v1 = ConstantInt::get(v1->getType(), val1);
            SMAP->createBinaryOp(v, cst_v1, Sv2);
        } 
        else {
            // S = S - v
            SMAP->remove(v);
        }
    }
}

// Select
// (v = v1 ? v2 : v3)
void Executor::executeSelect(Instruction *i, Value *cond,
                             Value *trueval, Value *falseval) {
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    
    assert(i && "Expecting an instruction!");
    assert((cond && trueval && falseval) && "Expecting values!");
    Value *v   = i;
    Value *v1  = i->getOperand(0); // condition
    Value *v2  = i->getOperand(1); // true-value
    Value *v3  = i->getOperand(2); // false-value
    std::string name = i->getName().str();
        // condition has a symbolic value
    if (SMAP->contains(v1)) 
    {
        SymbolPtr Sv1 = SMAP->get(v1);
        // v2 in domain(S) & v3 in domain(S)
        if (SMAP->contains(v2) && SMAP->contains(v3)) {
            SymbolPtr Sv2 = SMAP->get(v2);
            SymbolPtr Sv3 = SMAP->get(v3);
            SMAP->createSelectOp(v, Sv1, Sv2, Sv3);
        } 
        // v2 in domain(S)
        else if (SMAP->contains(v2)) {
            SymbolPtr Sv2 = SMAP->get(v2);
            Value *cst_v3 = dyn_cast<ConstantInt>(falseval);
            SMAP->createSelectOp(v, Sv1, Sv2, cst_v3);
        }
        // v3 in domain(S)
        else if (SMAP->contains(v3)) {
            SymbolPtr Sv3 = SMAP->get(v3);
            Value *cst_v2 = dyn_cast<ConstantInt>(trueval);
            SMAP->createSelectOp(v, Sv1, cst_v2, Sv3);
        }
        else {
            Value *cst_v2 = dyn_cast<ConstantInt>(trueval);
            Value *cst_v3 = dyn_cast<ConstantInt>(falseval);
            SMAP->createSelectOp(v, Sv1, cst_v2, cst_v3); 
        }
    } 
    // condition is a non symbolic value
    else 
    {
        ConstantInt *CI1 = dyn_cast<ConstantInt>(cond);
        int condValue = CI1->getSExtValue();
        // true-value was selected
        if (condValue!=0) { 
            if (SMAP->contains(v2)) {
                // S = S[v ->S(trueval)]
                SMAP->update(v, v2);
            } else {
                // S = S - v
                SMAP->remove(v);
            }
        } 
        // false-value was selected
        else {
            if (SMAP->contains(v3)) {
                // S = S[v ->S(falseval)]
                SMAP->update(v, v3);
            } else {
                // S = S - v
                SMAP->remove(v);
            }
        }
    }
}


void Executor::executePhi(Instruction *i) {
    
    assert(LastExecutedBB && "Something wrong with sniper_execute for Phi!");
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    assert(i && "Expecting an instruction!");
    PHINode *phi = (PHINode*) i;
    Value *vTaken;
    vTaken = phi->getIncomingValueForBlock(LastExecutedBB);
    assert(vTaken && "Something wrong with sniper_execute for Phi!");
    Value *v   = i;
    // vTaken is a constant value 
    if (isa<ConstantInt>(vTaken)) {
        // Trying to retrieve the variable 
        // associated with the constant value:
        //
        // bb1:
        // icmp sgt %tmp, 0, 1 
        // br %tmp, bb2, bb3
        // bb3:
        // %x = phi [true, %bb1]...
        /*
        bool toRemove = true;
        ConstantInt *c = dyn_cast<ConstantInt>(vTaken);
        if (c && c->getType()->isIntegerTy(1)) {
            BasicBlock *bb = LastExecutedBB;
            BranchInst *bi = dyn_cast<BranchInst>(bb->getTerminator());
            if (bi && bi->isConditional()) {
                BasicBlock *targetbb;
                if (c->isZero()) { // false value
                    targetbb = (BasicBlock*) bi->getSuccessor(1);
                } else { // true value
                    targetbb = (BasicBlock*) bi->getSuccessor(0);
                }
                if (currentbb==targetbb) {
                    // Replace true/false by %cond
                    Value *cond = bi->getCondition();
                    phi->setIncomingValue(i, cond);
                }
            }
          
        }
        if (toRemove) {
            // S = S - v
            SMAP->remove(v);
        } else {
            if (SMAP->contains(vTaken)) {
                // S = S[v ->S(vTaken)]
                SMAP->update(v, vTaken);
            } else {
                // S = S - v
                SMAP->remove(v);
            }
        }
        */

        // S = S - v
        SMAP->remove(v);
    } 
    // vTaken is a variable
    else {
        if (SMAP->contains(vTaken)) {
            // S = S[v ->S(vTaken)]
            SMAP->update(v, vTaken);
        } else {
            // S = S - v
            SMAP->remove(v);
        }
    }
}

void Executor::executeBranch(Instruction *i, bool cond) {
    
    assert(i && "Expecting an instruction!");
    LastExecutedBB = i->getParent(); 
    if (CollectBlockTraces) {
        ExecutedBlocks.push_back(LastExecutedBB);
    }
    BranchInst *br = dyn_cast<BranchInst>(i);
    if (br->isConditional()) {        
        // Loops: check if the control-flow
        //        jump back to a loop header.
        std::string bbname = PathLoopInfo->getLoopLatchName(br);  
        BasicBlock *nextBB1 = br->getSuccessor(0);
        BasicBlock *nextBB2 = br->getSuccessor(1);
        if ((cond && bbname==nextBB1->getName().str()) 
            || (!cond && bbname==nextBB2->getName().str())) {
            unsigned line = 0;
            if (MDNode *N = br->getMetadata("dbg")) { 
                DILocation Loc(N); 
                line = Loc.getLineNumber();
            }
            //std::cout << "Current: " << LastExecutedBB->getName().str() << std::endl;
            //std::cout << "LoopLatch: " << bbname << std::endl;
            std::cout << "Violated property: unwinding assertion";
            std::cout << " (line " << line << ")\n";
            //exit(1);
            DisabledSymbolicExeCurRun = true;
            return;
        }
        // Next block to be executed
        if (cond) {
            NextBB = nextBB1;
        } else {
            NextBB = nextBB2;
        }
    } else {
        // Next block to be executed
        NextBB = br->getSuccessor(0);
    }
    if (!DisabledSymbolicExeCurRun) {
        PathContext->propagatePointers(LastExecutedBB, NextBB);
    }
    // For program coverage analysis
    //Profile->covered(br, cond);
    //Profile->covered(br->getParent());
    if(br->isConditional()) {
        // Update the number of time we jump in the IR
        GotoCount++;
        if (GotoCount>MaxDepth) {
            std::cout << "warning: maximum goto-count (";
            std::cout << GotoCount  << ") reached.\n";
            DisabledSymbolicExeCurRun = true;
            return;
        }
        if (DisabledSymbolicExeCurRun) {
            return;
        }
        Value *vcond = br->getCondition();
        // vcond in domain(S) 
        if (SMAP->contains(vcond)) {
            SymbolPtr Svcond = SMAP->get(vcond);
            // Report(S(vcond), branchTaken)
            Path->addBranchCell(Svcond, cond, i);
        } 
    }
}

void Executor::PushArgs(Value *arg) {
    assert(arg && "Expecting a value!");
    // Push before a call
    if (CallInst *C = dyn_cast<CallInst>(arg)) {
        // Check invoke count
        InvokeCount++;
        //std::cout << "push (call): ";
        // Add the arg's symbols to the stack
        for (unsigned i=0 ; i<C->getNumArgOperands(); i++) {
            Value *Arg = C->getArgOperand(i);
            if(SMAP->contains(Arg)) {
                SymbolPtr S = SMAP->get(Arg);
                argStack.push_back(S);
                //S->dump(); std::cout << " ";
            } else {
                argStack.push_back(NULL);
                //std::cout << "NULL ";
            }
        }
    }
    // Push inside a function
    else if (Function *F = dyn_cast<Function>(arg)) {
        // Push the return symbol
        inst_iterator it = inst_end(F);
        Instruction &last = *--it;
        bool err = true;
        if (ReturnInst *R = dyn_cast<ReturnInst>(&last)) {
            Value *V = R->getReturnValue();
            // No void return
            if (V) {
                err = false;
                //std::cout << "push (fun:" << F->getName().str() << " ): ";
                if(SMAP->contains(V)) {
                    SymbolPtr S = SMAP->get(V);
                    argStack.push_back(S);
                    //S->dump(); std::cout << " ";
                } else {
                    argStack.push_back(NULL);
                    //std::cout << "NULL ";
                }
            } 
        }
        assert(!err && "executor:push");
    } else {
        assert("executor:push, wrong arg");
    }
    //std::cout << std::endl;
}

void Executor::PopArgs(Value *arg) {
    assert(arg && "Expecting a value!");
    // Pop after a call
    if (CallInst *C = dyn_cast<CallInst>(arg)) {
        Function *calledFun = C->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun =
            dyn_cast<Function>(C->getCalledValue()->stripPointerCasts());
            assert(calledFun && "Unresolvable indirect function call!");
        }
        assert(!argStack.empty() && "executor: pop, empty stack");
        //std::cout << "pop (call:" << calledFun->getName().str() << " ): ";
        SymbolPtr S = argStack.back();
        argStack.pop_back();
        if (S) {
            SMAP->update(arg, S);
            //S->dump(); std::cout << " ";
        } else {
            //std::cout << "NULL ";
        }

    }
    // Pop inside a function
    else if (Function *F = dyn_cast<Function>(arg)) {
        //std::cout << "pop (fun:" << F->getName().str() << " ): ";
        Function::arg_iterator ait;
        for (ait = F->arg_begin(); ait != F->arg_end(); ++ait) {
            Value *V = ait;
            assert(!argStack.empty() && "executor: pop, empty stack");
            SymbolPtr S = argStack.back();
            argStack.pop_back();
            if (S) {
                SMAP->update(V, S);
                //S->dump(); std::cout << " ";
            } else {
                //std::cout << "NULL ";
            }
        }
    } else {
        assert("executor: pop, wrong arg");
    }
    //std::cout << std::endl;
}

void Executor::ReportAssert(Value *v, int assertResult) {
    assert(v && "Expecting a value!");
    LastRunAssertResult = (assertResult ? SUCCESS : FAIL);
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    // v in domain(S) 
    SymbolPtr Sv;
    if (SMAP->contains(v)) {
        Sv = SMAP->get(v);
        // TODO
        Instruction *i = dyn_cast<Instruction>(v);
        Path->addAssertCell(Sv, (bool)assertResult, i);
    }
}

void Executor::ReportAssume(Value *v, int assumeResult) {
    assert(v && "Expecting a value!");
    if (DisabledSymbolicExeCurRun) {
        return;
    }
    // v in domain(S) 
    SymbolPtr Sv;
    if (SMAP->contains(v)) {
        Sv = SMAP->get(v);
        Instruction *i = dyn_cast<Instruction>(v);
        Path->addAssumeCell(Sv, assumeResult, i);
        if (assumeResult==0) {
            std::cout << "ASSUME !!!!!!!!!!!\n";
            exit(1);
        }
    }
}

void Executor::ReportEnd() {

    assert(status==START && "wrong status for Executor (!=START)!");
    // Add the trace
    if (LastRunAssertResult==FAIL) {
        Trace->setFailing();
    } else if (LastRunAssertResult==SUCCESS) {
        Trace->setSuccessful();            
    } else {
        Trace->setUnknow();
    }
    if (CollectBlockTraces) {
        // Get the last blocks
        if (NextBB) {
            ExecutedBlocks.push_back(NextBB);
        } else {
            assert(TargetFun->size()==1 && "Cannot get the last executed block!");
            ExecutedBlocks.push_back(&TargetFun->getEntryBlock());
        }
        // Update the current trace
        Trace->setExecutedBlocks(ExecutedBlocks);
        ExecutedBlocks.clear();
        Profile->addProgramTrace(Trace);
    }
    else if (NoAssert) {
        Profile->addProgramTrace(Trace);
    } else {
        // Only save failing traces when not using PTF
        if (LastRunAssertResult==FAIL) {
            Profile->addProgramTrace(Trace);
        }
    }
    // Reset 
    RunID++;
    GotoCount = 0;
    DisabledSymbolicExeCurRun = DisabledSymbolicExe;
    LastExecutedBB = NULL;
    argStack.clear();
    if (!DisabledSymbolicExe) {
        //Path->dump();
        //PathFormula->dump();
        delete PathFormula;
        delete PathEncoder;
        delete PathContext;
    }
}

// Check if the operation performed by i
// is non-linear.
bool Executor::isNonLinear(Instruction *i) {
    assert(i && "Expecting an instruction!");
    Value *v1 = i->getOperand(0);
    Value *v2 = i->getOperand(1);
    int opCode = i->getOpcode();
    return ((opCode==Instruction::Mul  || 
             opCode==Instruction::FMul ||
             opCode==Instruction::UDiv ||
             opCode==Instruction::SDiv ||
             opCode==Instruction::FDiv ||
             opCode==Instruction::URem ||
             opCode==Instruction::SRem ||
             opCode==Instruction::FRem) &&
    (!isa<ConstantInt>(v1) && !isa<ConstantInt>(v2)));
}

SymbolicPath* Executor::getSymbPath() {
    return Path;
}

void Executor::setMaxDepth(unsigned d) {
    MaxDepth = d;
}

bool Executor::areAllLocDefinite() {
    return AllLocDefinite;
}

void Executor::endOfRun() {
    assert(status==START && "Wrong status for Executor!");
    AllLocDefinite = true;
    if (!DisabledSymbolicExe) {
        SMAP->clear();
        PMAP->clear();
    }
    status = IDLE;
}

void Executor::clean() {
    if (DisabledSymbolicExe) {
        delete Path;
        delete SMAP;
        delete PMAP;
    }
    status = END;
}
