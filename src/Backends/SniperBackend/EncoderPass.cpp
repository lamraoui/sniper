/**
 * \file EncoderPass.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   7 March 2016
 */

#include "EncoderPass.h"
 

Formula* EncoderPass::makeTraceFormula() {

    MSTimer timer1;
    if(options->printDuration()) {
        timer1.start();
    }
    // Create an empty trace formula
    Formula *formula = new Formula();
    // Prepare the CFG variables
    encoder->prepareControlFlow(targetFun);
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
        // HFTF: Encode bug free blocks as hard
        bool doEncodeBB = true;
        if (options->htfUsed()) {
            if(profile->isBugFreeBlock(bb)) {
                doEncodeBB = false;
            }
        }
        // Iterate through the basicblocks
        for (BasicBlock::iterator iti=bb->begin(), eti=bb->end();
             iti!=eti; ++iti) {
            Instruction *i = iti;
            bool doEncodeInst = doEncodeBB;
            // Check the line number
            unsigned line = 0;
            if (MDNode *N = i->getMetadata("dbg")) {
                DILocation Loc(N); 
                line = Loc.getLineNumber();
                // Instruction related to an assert
                if (ctx->isAssertCall(line)) {
                    isWeigted = false;
                    doEncodeInst = true;
                } else {
                    // Instruction with line number
                    // (not related to an assert)
                    isWeigted = true;
                }
            } else {
                // Instruction with no line number
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
                    expr = encoder->encode(cast<CallInst>(i), preCond, postCond);
                    if (!expr) {
                        // Do not encode sniper_x functions.
                        continue;
                    }
                    isWeigted = false;
                    break;
                case Instruction::Alloca:
                    expr = encoder->encode(cast<AllocaInst>(i));
                    if (!expr) {
                        continue;
                    }
                    isWeigted = false;
                    break;
                case Instruction::Store:
                    expr = encoder->encode(cast<StoreInst>(i));
                    break;
                case Instruction::Load:
                    expr = encoder->encode(cast<LoadInst>(i), postCond);
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
                    expr = encoder->encode(cast<ReturnInst>(i));
                    isWeigted = false;
                    break;
                case Instruction::Unreachable:
                    // Do not encode.
                    continue;
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
                    assert("unsupported LLVM instruction!\n");
                    break;
                default:
                    llvm_unreachable("Illegal opcode!");
            }
            assert(expr && "Expression is null!");
            // Atoi checking
            if (isAtoiFunction(i)) {
                isWeigted = false;
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
                        assert(!currentConstraits.empty() && "No constraints!");
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
                    assert("Unknow granularity level!");
                }
            }
            // Instruction with no line number
            else {
                expr->setHard();
                formula->add(expr);
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
        timer1.stop("Trace Formula Encoding Time");
    }
    return formula;
}

Formula *EncoderPass::getPostCondition() {
    return postCond;
}

Formula *EncoderPass::getPreCondition() {
    return preCond;
}

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
                if (F) {
                    calledFunName = F->getName();
                }
            }
            if (calledFunName==Frontend::SNIPER_ASSERT_FUN_NAME ||
                calledFunName=="sniper_reportAssert") {
                hasAsserts = true;
                if (MDNode *N = I.getMetadata("dbg")) {
                    DILocation Loc(N);
                    unsigned line = Loc.getLineNumber();
                    ctx->setAssertCallLine(line);
                }
            }
            // TODO: put assume line numbers in a differente vector
            else if (calledFunName==Frontend::SNIPER_ASSUME_FUN_NAME ||
                     calledFunName=="sniper_reportAssume") {
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

void EncoderPass::initGlobalVariables() {
    // TODO: support for gloabl variables
    Module *llvmMod = this->targetFun->getParent();
    //assert(llvmMod->global_empty() && "Global variables are not supported!");
}

bool EncoderPass::isAtoiFunction(Instruction *I) {
    if(I->getOpcode()==Instruction::Add && I->getName().substr(0, 1) == "_") {
        CallInst *call = dyn_cast<CallInst>(I->getOperand(0));
        if(call && call->getNumArgOperands()==1) {
            Function *F = call->getCalledFunction();
            if (F && F->getName()=="atoi") {
                return true;
            }
        }
    }
    return false;
}
