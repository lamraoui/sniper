/**
 * Pass.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/03/11
 * @copyright : NII 2012
 */


#include "EncoderPass.h"


EncoderPass::EncoderPass(Function *_targetFun, Context *_ctx, LoopInfoPass *_loops, 
                         ProgramProfile *_profile, Options *_options) 
: targetFun(_targetFun), ctx(_ctx), loops(_loops), profile(_profile), options(_options) { 
    this->encoder = new Encoder(ctx);
    // Create an empty AS formula (for pre- and post-condition)
    this->AS = new Formula();
}

EncoderPass::~EncoderPass() { 
    delete encoder;
}

// =============================================================================
// initAssertCalls
//
// Check the line number to see if the current instruction is used in a 
// call to assert function. If it is the case, we have to put the related 
// constraints as hard.
// =============================================================================
void EncoderPass::initAssertCalls() {
    bool hasAsserts = false;
    inst_iterator ii0;
    for (ii0 = inst_begin(targetFun); ii0!=inst_end(targetFun); ii0++) {
        Instruction &I = *ii0;
        if (CallInst *C = dyn_cast<CallInst>(&I)) {
            StringRef calledFunName;
            Function *F = C->getCalledFunction();
            // Direct call
            if (F) {
                calledFunName = F->getName();
            } 
            // Indirect function call
            else { 
                F = dyn_cast<Function>(C->getCalledValue()->stripPointerCasts());
                if (F) 
                    calledFunName = F->getName();
            }
            if (calledFunName==Encoder::SNIPER_ASSERT_RETINT_FUN_NAME
                || calledFunName==Encoder::SNIPER_ASSERT_RETVOID_FUN_NAME
                || calledFunName==Encoder::SNIPER_ASSUME_RETINT_FUN_NAME
                || calledFunName==Encoder::SNIPER_ASSUME_RETVOID_FUN_NAME
                || calledFunName==Encoder::ASSERT_FUN_NAME 
                || calledFunName==Encoder::ASSUME_FUN_NAME) {
                hasAsserts = true;
                if (MDNode *N = I.getMetadata("dbg")) { 
                    DILocation Loc(N); 
                    unsigned line = Loc.getLineNumber();
                    ctx->setAssertCallLine(line);
                }
            }
        }
    }
    // No post-condition and no oracle?
    assert((hasAsserts || !options->getGoldenOutputsFileName().empty()) &&
           "No call to assert function nor oracle!");
}

// =============================================================================
// makeTraceFormula
// 
// Encode the target function into a partial SMT formula.
// =============================================================================
Formula* EncoderPass::makeTraceFormula() {
    
    MSTimer timer1;
    if(options->printDuration()) {
        //timer1.start();
    }
    // Create an empty formula
    Formula *formula = new Formula();
    // Model the CFG formula
    ExprPtr cfExpr = encoder->encodeControlFlow(targetFun);
    cfExpr->setHard();
    formula->add(cfExpr);
    // Pre-process the global variables
    initGlobalVariables();
    // Save the line numbers of the call to assert 
    initAssertCalls();
    
    bool isWeigted = false;
    unsigned oldLine = 0;
    Instruction *lastInstruction = NULL;
    std::vector<ExprPtr> currentConstraits;
    
    // Iterate through the function in Topological Order
    // encode each instructions in SMT constraints
    // @See: eli.thegreenplace.net/2013/09/16/analyzing-function-cfgs-with-llvm/
    ReversePostOrderTraversal<Function*> RPOT(this->targetFun);
    ReversePostOrderTraversal<Function*>::rpo_iterator itb;
    for (itb=RPOT.begin(); itb!=RPOT.end(); ++itb) {
        BasicBlock *bb = *itb;
        // Propagate pointers when we enter in a new basicblock
        ExprPtr e = ctx->propagatePointers(bb);
        if (e) {
            e->setHard();
            formula->add(e);
        }
        // HFTF: Do not encode bug free blocks
        bool doEncodeBB = true;
        if (options->htfUsed()) {
            if(profile->isBugFreeBlock(bb)) {
                doEncodeBB = false;
            }
        }
        // Iterate through the basicblocks
        for (BasicBlock::iterator iti=bb->begin(), eti=bb->end(); iti!=eti; ++iti) {
            Instruction *i = iti;
            // Check the line number
            unsigned line = 0;
            bool doEncodeInst = doEncodeBB;
            if (MDNode *N = i->getMetadata("dbg")) {
                DILocation Loc(N); 
                line = Loc.getLineNumber();
                if (ctx->isAssertCall(line)) {
                    isWeigted = false;
                    doEncodeInst = true;
                } else {
                    isWeigted = true;
                }
            } else {
                isWeigted = false;
            }
            // If i is a store/load/gep instruction
            // encoded (as hard) in any cases
            if(isa<GetElementPtrInst>(i)
               || isa<LoadInst>(i)
               || isa<StoreInst>(i)) {
                doEncodeInst = true;
                isWeigted = false;
            }
            // Hardened Trace Formula
            if (options->htfUsed() && !doEncodeInst) {
                isWeigted = false;
            }
            // Encode the instruction in a SMT formula
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
                case Instruction::Xor:
                case Instruction::Shl:
                case Instruction::LShr:
                case Instruction::AShr:
                    expr = encoder->encode(cast<BinaryOperator>(i));
                    break;
                case Instruction::Select:
                    expr = encoder->encode(cast<SelectInst>(i));
                    break;
                case Instruction::PHI:
                    expr = encoder->encode(cast<PHINode>(i));
                    isWeigted = false;
                    break;
                case Instruction::Br:
                    expr = encoder->encode(cast<BranchInst>(i), loops);
                    isWeigted = false;
                    break;
                case Instruction::Switch:
                    expr = encoder->encode(cast<SwitchInst>(i));
                    break;
                case Instruction::ICmp:
                    expr = encoder->encode(cast<ICmpInst>(i));
                    break;
                case Instruction::Call:
                    expr = encoder->encode(cast<CallInst>(i), AS);
                    isWeigted = false;
                    break;
                case Instruction::Alloca:
                    expr = encoder->encode(cast<AllocaInst>(i));
                    isWeigted = false;
                    break;
                case Instruction::Store:
                    expr = encoder->encode(cast<StoreInst>(i));
                    //isWeigted = true;
                    break;
                case Instruction::Load:
                    expr = encoder->encode(cast<LoadInst>(i), AS);
                    //isWeigted = true;
                    break;
                case Instruction::GetElementPtr:
                    expr = encoder->encode(cast<GetElementPtrInst>(i));
                    isWeigted = false;
                    break;
                case Instruction::SExt:
                    expr = encoder->encode(cast<SExtInst>(i));
                    isWeigted = false;
                    break;
                case Instruction::ZExt:
                    expr = encoder->encode(cast<ZExtInst>(i));
                    isWeigted = false;
                    break;
                case Instruction::Ret:
                case Instruction::Unreachable:
                    break;
                case Instruction::PtrToInt: {
                    // Instruction added by SNIPER
                    std::string instName = i->getName().str();
                    std::string prefix("sniper_ptrVal");
                    if (!instName.compare(0, prefix.size(), prefix)) {
                        continue;
                    }
                    // NO BREAK!
                }
                case Instruction::VAArg:
                case Instruction::Invoke:
                case Instruction::Trunc:
                case Instruction::FPTrunc:
                case Instruction::FPExt:
                case Instruction::UIToFP:
                case Instruction::SIToFP:
                case Instruction::FPToUI:
                case Instruction::FPToSI:
                case Instruction::IntToPtr:
                case Instruction::BitCast:
                case Instruction::FCmp:
                case Instruction::ExtractElement:
                case Instruction::InsertElement:
                case Instruction::ShuffleVector:
                case Instruction::ExtractValue:
                case Instruction::InsertValue:
                    i->dump();
                    assert("Unsupported LLVM instruction!");
                    break;
                default:
                    llvm_unreachable("Illegal opcode!");
            }
            if (expr) {
                // Atoi checking
                if(i->getOpcode()==Instruction::Add 
                   && i->getName().substr(0, 1) == "_") {
                    CallInst *call = dyn_cast<CallInst>(i->getOperand(0));
                    if(call && call->getNumArgOperands()==1) {
                        Function *F = call->getCalledFunction();
                        if (F && F->getName()=="atoi") {
                            isWeigted = false;
                        }
                    }
                }
                // Instruction with line number
                if (isWeigted) {
                    // Add each instruction separately
                    if (options->instructionGranularityLevel()) {
                        expr->setInstruction(i);
                        expr->setSoft();
                        formula->add(expr);
                    }
                    // Pack and add all instructions from
                    // the same line number
                    else if (options->lineGranularityLevel()) {
                        assert(line>0 && "Illegal line number!");
                        // New line, Add the collect constraints to the formula
                        if (line!=oldLine && oldLine!=0) {
                            assert(!currentConstraits.empty() &&
                                   "No constraints!");
                            assert(lastInstruction && "Instruction is null!");
                            ExprPtr e = Expression::mkAnd(currentConstraits);
                            e->setInstruction(lastInstruction);
                            e->setSoft();
                            formula->add(e);
                            currentConstraits.clear();
                        }
                        currentConstraits.push_back(expr);
                        oldLine = line;
                        lastInstruction = i;
                    }
                    // Block level
                    else if (options->blockGranularityLevel()) {
                        currentConstraits.push_back(expr);
                    } else {
                        assert("EncodePass!");
                    }
                }
                // Instruction with no line number
                else {
                    expr->setHard();
                    formula->add(expr);
                }
            }
        }
        // End of basic block iteration
        if (options->blockGranularityLevel()) {
            if (!currentConstraits.empty()) {
                ExprPtr e = Expression::mkAnd(currentConstraits);
                e->setInstruction(lastInstruction);
                e->setSoft();
                formula->add(e);
                currentConstraits.clear();
            }
        }
    }
    // Add the remaining soft constraints to the formula
    if (!currentConstraits.empty()) {
        assert(options->lineGranularityLevel()
               && "Some instructions could not be encoded!");
        assert(lastInstruction && "Instruction is null!");
        ExprPtr e = Expression::mkAnd(currentConstraits);
        e->setInstruction(lastInstruction);
        e->setSoft();
        formula->add(e);
        currentConstraits.clear();
    }
    if(options->printDuration()) {
        //timer1.stop("Trace Formula Encoding Time");
    }
    return formula;
}

// =============================================================================
// getASFormula
//
// Return a formula representating the pre- and post-conditions
// =============================================================================
Formula* EncoderPass::getASFormula() {
    return AS;
}

// =============================================================================
// init_globalValues 
// =============================================================================
void EncoderPass::initGlobalVariables() {
    Module *llvmMod = this->targetFun->getParent();
    //assert(llvmMod->global_empty() && "Global variables are not supported!");
    
    // TODO : initializer
    /* Module *m = this->targetFun->getParent();
    Module::global_iterator iter = m->global_begin();
    Module::global_iterator end = m->global_end();
    for (; iter != end; ++iter) {
        Value *gv = iter;
        // Set the version of the variable
        //this->gv2v[gv] = 0; 
    }*/  
}