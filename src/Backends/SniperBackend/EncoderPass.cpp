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
    if (!hasAsserts && options->getGoldenOutputsFileName().empty()) {
        //std::cout << "warning: no call to assert function!\n";
        error("no call to assert function!");
    }
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
        // PTF: Do not encode bug free blocks
        bool doEncodeBB = true;
        if (options->ptfUsed() || options->htfUsed()) {
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
            // Pruned Trace Formula
            else if (options->ptfUsed() && !doEncodeInst) {
                continue;
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
                    error("unsupported LLVM instruction!\n");
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
                        if (line==0) {
                            error("EncodePass");
                        }
                        // New line, Add the collect constraints to the formula
                        if (line!=oldLine && oldLine!=0) {
                            if (currentConstraits.empty() || !lastInstruction) {
                                error("EncodePass");
                            }
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
                        error("EncodePass");
                    }
                }
                // Instruction with no line number
                else {
                    e->setHard();
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
        if (options->lineGranularityLevel()) {
            if (!lastInstruction) {
                error("EncodePass");
            }
            ExprPtr e = Expression::mkAnd(currentConstraits);
            e->setInstruction(lastInstruction);
            e->setSoft();
            formula->add(e);
            currentConstraits.clear();
        } else {
            error("EncodePass");
        }
    }
    if(options->printDuration()) {
        //timer1.stop("Trace Formula Encoding Time");
    }
    // Pruned Flow-sensitive Trace Formula
    if (options->ptfUsed()) {
          error("Pruned flow-sensitive trace formula is desactivated");
//        MSTimer timer2;
//        if(options->printDuration()) {
//            timer2.start();
//        }
//        ExprPtr pp = constructPartialPart_V3();
//        if (pp) {
//            pp->setHard();
//            formula->add(pp);
//            if (options->dbgMsg()) {
//                std::cout << "Partial part of TF: "<< std::endl;
//                pp->dump();
//                std::cout << std::endl;
//            }
//        }
//        if(options->printDuration()) {
//            timer2.stop("Pruned Part Encoding Time");
//        }
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
// constructPartialPart (new 2)
//
// Encode the partial part of TF (if the option -ptf is used).
//
// bool isUsedOutsideOfBlock(const BasicBlock *BB) const;
// =============================================================================
ExprPtr EncoderPass::constructPartialPart_V3() {
    
    std::set<BasicBlock*> bfBlocks = profile->getBugFreeBlocks();
    if (bfBlocks.empty()) {
        if (options->verbose()) {
            std::cout << "No safe path segments for constructing PTF.\n";
        }
        if (options->dbgMsg()) {
            std::cout << "Nb. CEPP: 0\n";
        }
        return NULL;
    }
    std::set<BasicBlock*> failBlocks = profile->getFailingRunBB();
    std::vector<ProgramTrace*> traces = profile->getSuccessfulProgramTraces();
    
    typedef std::vector<ExprPtr>      ExprVec;
    typedef std::map<BasicBlock*, ExprVec> BBExprVecMap;
    BBExprVecMap allExprsMap;
    std::vector<ExprPtr> allExprs;
    
    // TRACE
    //std::cout << "\nNb. fail:      " << profile->getFailingProgramTraces().size() << std::endl;
    //std::cout << "Nb. succ:      " << traces.size() << std::endl;
    //std::cout << "Nb. other:     " << profile->getOtherProgramTraces().size() << std::endl;
    //std::cout << "Nb. BF blocks: " << bfBlocks.size() << std::endl;
    //std::cout << traces.size() << std::flush;
    //std::cout << "\t" << profile->getFailingProgramTraces().size()  << std::flush;
    //std::cout << "\t" << bfBlocks.size() << std::flush;
    
    // Iterate on the program traces
    std::vector<ProgramTrace*>::iterator it1;
    for (it1=traces.begin(); it1!=traces.end(); ++it1) {
        ProgramTrace *t = *it1;
        // Iterate on the instruction traces
        bool       inSafePath         = false;
        bool       prev_inSafePath    = false;
        BasicBlock *currentBlock      = NULL;
        BasicBlock *lastBlock         = NULL;
        BasicBlock *prevExecutedBlock = NULL;
        BasicBlock *firstBugFreeBlock = NULL;
        
        std::vector<ExprPtr> andExprs;
        std::vector<InstTracePtr> instTraces = t->getInstructionTraces();
        std::vector<InstTracePtr>::iterator it2;
        for (it2=instTraces.begin(); it2!=instTraces.end(); ++it2) {
            InstTracePtr i = *it2;
            Instruction *inst = i->getInstruction();
            if (currentBlock && currentBlock!=inst->getParent()) {
                prevExecutedBlock = currentBlock;
            }
            currentBlock = inst->getParent();
            // New block
            if (currentBlock!=lastBlock) {
                lastBlock = currentBlock;
                prev_inSafePath = inSafePath;
                // Check if inst is in a "bug free block"
                // currentBlock \in bfBlocks?
                bool isIn = false;
                if (bfBlocks.size()<=failBlocks.size()) {
                    isIn = bfBlocks.find(currentBlock)!=bfBlocks.end();
                } else {
                    isIn = failBlocks.find(currentBlock)==failBlocks.end();
                }
                if (!isIn) {
                    inSafePath = false;
                } else {
                    inSafePath = true;
                    if (!firstBugFreeBlock) {
                        firstBugFreeBlock = currentBlock;
                    }
                }
            }
            // Same block
            else {
                lastBlock = currentBlock;
            }
            // Check if inst is used in an assert
            if (MDNode *N = inst->getMetadata("dbg")) {
                DILocation Loc(N);
                unsigned line = Loc.getLineNumber();
                if (ctx->isAssertCall(line)) {
                    continue;
                }
            }
            
            if (!inSafePath) {
                // If previous block was successful
                if (prev_inSafePath) {
                    if (!andExprs.empty()) {
                        
                        // Check sizes
                        /*const unsigned bbSize   = bb->size()/2;
                        const unsigned exprSize = andExprs.size()-2;
                        //if (exprSize>bbSize) {
                        if (bbSize<3) {
                            std::cout << "SKIP!\n";
                            andExprs.clear();
                        } else {*/
                        
                            // (and (= x 0) ... bbn)
                            ExprPtr andExpr;
                            if (andExprs.size()==1) {
                                andExpr = andExprs.back();
                            }  else if (andExprs.size()>1) {
                                andExpr = Expression::mkAnd(andExprs);
                            }
                            andExprs.clear();
                            
                            // Is already in?
                            bool isAlreadyIn = false;
                            ExprVec vec = allExprsMap[firstBugFreeBlock];
                            for (ExprPtr e : vec) {
                                if (andExpr == e) {
                                    isAlreadyIn = true;
                                    break;
                                }
                            }
                            if (!isAlreadyIn) {
                                allExprsMap[firstBugFreeBlock].push_back(andExpr);
                            }
                        //}
                    }
                }
                // Reset
                firstBugFreeBlock = NULL;
                prev_inSafePath = false;
                // Skip to the next instruction trace
                continue;
            }
            
            // Branch instructions (special case)
            if (BranchInst *br = dyn_cast<BranchInst>(inst)) {
                BasicBlock *targetbb;
                if (br->isConditional()) {
                    Value *concrete = t->getConcreteValue(br->getCondition());
                    if (!concrete) {
                        error("no concrete value!");
                    }
                    if (br->getNumSuccessors()!=2) {
                        error("unsupported br!");
                    }
                    int val = 0;
                    if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                        val = (int) ci->getSExtValue();
                    } else {
                        error("no concrete value for variable!");
                    }
                    if (val) {
                        targetbb = br->getSuccessor(0);
                    } else {
                        targetbb = br->getSuccessor(1);
                    }
                }
                // Unconditional branch
                else {
                    targetbb = br->getSuccessor(0);
                }
                // targetbb \in bfBlocks?
                bool isIn = false;
                if (bfBlocks.size()<=failBlocks.size()) {
                    isIn = bfBlocks.find(targetbb)!=bfBlocks.end();
                } else {
                    isIn = failBlocks.find(targetbb)==failBlocks.end();
                }
                if (!isIn) {
                    // (nextbb)
                    //ExprPtr e1 = Expression::mkBoolVar(targetbb->getName().str());
                    //andExprs.push_back(e1);
                    // (bb_nextbb)
                    ExprPtr e2 = ctx->getTransition(currentBlock, targetbb);
                    andExprs.push_back(e2);
                }
                continue;
            }
            
            // ------------------------------------------------
            // Collect RHS variables computed in failing blocks
            // ------------------------------------------------
            std::set<Value*> varsUsedInFailBlocks;
            for (User::op_iterator itb=inst->op_begin(), ite=inst->op_end(); itb!=ite; ++itb) {
                Value *v = *itb;
                // Phi
                if (PHINode *phi = dyn_cast<PHINode>(inst)) {
                    if (!prevExecutedBlock) {
                        error("no prev exe block");
                    }
                    Value *vTaken = phi->getIncomingValueForBlock(prevExecutedBlock);
                    if (!vTaken) {
                        error("no phi incom. value");
                    }
                    if (vTaken!=v) {
                        continue;
                    }
                }
                // Instruction
                if (Instruction *opI = dyn_cast<Instruction>(v)) {
                    BasicBlock *parentBlock = opI->getParent();
                    const bool isIn = bfBlocks.find(parentBlock)!=bfBlocks.end();
                    if (!isIn) {
                        varsUsedInFailBlocks.insert(opI);
                    }
                }
                // Argument
                else if (Argument *arg = dyn_cast<Argument>(v)) {
                    varsUsedInFailBlocks.insert(arg);
                }
                // BasicBlock
                //else if (BasicBlock *bb = dyn_cast<BasicBlock>(v)) {
                //    const bool isIn = bfBlocks.find(bb)!=bfBlocks.end();
                //    if (!isIn) {
                //        varsUsedInFailBlocks.insert(bb);
                //    }
                //}
                // ConstantInt
                else if (isa<ConstantInt>(v)) {
                }
                else {
                    v->dump();
                    error("not supported");
                }
            }
            // ------------------------------------------------
            // Collect LHS variables computed in failing blocks
            // ------------------------------------------------
            for (Value::use_iterator itb=inst->use_begin(), ite=inst->use_end(); itb!=ite; ++itb) {
                Value *v = *itb;
                if (Instruction *useI = dyn_cast<Instruction>(v)) {
                    BasicBlock *parentBlock = useI->getParent();
                    const bool isIn = bfBlocks.find(parentBlock)!=bfBlocks.end();
                    if (!isIn) {
                        varsUsedInFailBlocks.insert(inst);
                        break;
                    }
                }
                else {
                    v->dump();
                    error("value not supported");
                }
            }
            
            // ------------------------------------------------
            // Construct the expressions
            // ------------------------------------------------
            std::set<Value*>::iterator it4;
            for (it4=varsUsedInFailBlocks.begin(); it4!=varsUsedInFailBlocks.end(); ++it4) {
                Value *v = *it4;
                // Unsupported instructions
                if (isa<SExtInst>(v) || isa<ZExtInst>(v) || isa<StoreInst>(v) ||
                    isa<LoadInst>(v) || isa<GetElementPtrInst>(v) ) {
                    inst->dump();
                    v->dump();
                    error("instruction not supported!");
                }
                std::string name = v->getName().str();
                // Get the concrete value
                Value *concrete = t->getConcreteValue(v);
                if (!concrete) {
                    continue;
                }
                int val = 0;
                if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                    val = (int) ci->getSExtValue();
                } else {
                    error("no concrete value for variable!");
                }
                ExprPtr varExpr;
                ExprPtr valExpr;
                Type *t = v->getType();
                if (t->isIntegerTy(1) || t->isLabelTy()) {
                    varExpr = Expression::mkBoolVar(name);
                    if (val) {
                        valExpr = Expression::mkTrue();
                    } else {
                        valExpr = Expression::mkFalse();
                    }
                }
                // Integer 32 bits
                else if (t->isIntegerTy(32)) {
                    varExpr = Expression::mkIntVar(name);
                    valExpr = Expression::mkSInt32Num(val);
                } else {
                    error("unsupported type");
                }
                // (= x 0)
                ExprPtr eqExpr = Expression::mkEq(varExpr, valExpr);
                // Is already in?
                bool isAlreadyIn = false;
                for (ExprPtr e : andExprs) {
                    if (eqExpr == e) {
                        isAlreadyIn = true;
                        break;
                    }
                }
                if (!isAlreadyIn) {
                    andExprs.push_back(eqExpr);
                }
            } // end for varsUsedInFailBlocks
            
        } // end for instTraces
    } // end for traces
    
    // (or (not bb) (and var1=a var2=b ... nextbb prev_nextbb) ... (and ...))
    typedef BBExprVecMap::const_iterator BBExprVecMapIt;
    for (BBExprVecMapIt it2 = allExprsMap.begin(); it2 != allExprsMap.end(); it2++) {
        BasicBlock *bb = it2->first;
        // (not bb)
        ExprPtr bbExpr    = Expression::mkBoolVar(bb->getName().str());
        ExprPtr notBbExpr = Expression::mkNot(bbExpr);
        
        /*
        std::vector<ExprPtr> prednotbbTrans;
        for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
            BasicBlock *pred = *PI;
            ExprPtr bb_predbbExpr = ctx->getTransition(pred, bb); // t(b',b)
            ExprPtr notbb_predbbExpr = Expression::mkNot(bb_predbbExpr);
            prednotbbTrans.push_back(notbb_predbbExpr);
        }
        prednotbbTrans.push_back(notBbExpr1);
        ExprPtr notBbExpr = Expression::mkAnd(prednotbbTrans);
        */
        
        
        
        // (or ...)
        ExprVec andExprs = it2->second;
        andExprs.push_back(notBbExpr);
        ExprPtr orExpr = Expression::mkOr(andExprs);
        allExprs.push_back(orExpr);
    }
    // (and (...) (...)  ... )
    ExprPtr resExpr = NULL;
    if (allExprs.size()==1) {
        resExpr = allExprs.back();
    } else if (allExprs.size()>1) {
        resExpr = Expression::mkAnd(allExprs);
    }
    if (options->dbgMsg()) {
        std::cout << "Nb. CEPP: " << allExprs.size() << std::endl;
    }
    return resExpr;
}


// =============================================================================
// constructPartialPart (new)
// 
// Encode the partial part of TF (if the option -ptf is used).
//
// bool isUsedOutsideOfBlock(const BasicBlock *BB) const;
// =============================================================================
ExprPtr EncoderPass::constructPartialPart() {
    
    std::set<BasicBlock*> bfBlocks = profile->getBugFreeBlocks();
    if (bfBlocks.empty()) {
        if (options->verbose()) {
            std::cout << "No safe path segments for constructing PTF.\n";
        }
        if (options->dbgMsg()) {
            std::cout << "Nb. CEPP: 0\n";
        }
        return NULL;
    }
    std::set<BasicBlock*> failBlocks = profile->getFailingRunBB();
    
    // Iterate on the program traces
    std::vector<ExprPtr> allExprs; 
    std::vector<ProgramTrace*> traces = profile->getSuccessfulProgramTraces();
    
    
    // TRACE
    //std::cout << "\nNb. fail:      " << profile->getFailingProgramTraces().size() << std::endl;
    //std::cout << "Nb. succ:      " << traces.size() << std::endl;
    //std::cout << "Nb. other:     " << profile->getOtherProgramTraces().size() << std::endl;
    //std::cout << "Nb. BF blocks: " << bfBlocks.size() << std::endl;
    //std::cout << traces.size() << std::flush;
    //std::cout << "\t" << profile->getFailingProgramTraces().size()  << std::flush;
    //std::cout << "\t" << bfBlocks.size() << std::flush;
    
    
    std::vector<ProgramTrace*>::iterator it1;
    for (it1=traces.begin(); it1!=traces.end(); ++it1) {
        ProgramTrace *t = *it1;
        // Iterate on the instruction traces
        bool       inSafePath         = false;
        bool       prev_inSafePath    = false;
        BasicBlock *currentBlock      = NULL;
        BasicBlock *lastBlock         = NULL;
        BasicBlock *prevExecutedBlock = NULL;
        BasicBlock *firstBugFreeBlock = NULL;
        
        std::vector<ExprPtr> andExprs;
        std::vector<InstTracePtr> instTraces = t->getInstructionTraces();
        std::vector<InstTracePtr>::iterator it2;
        for (it2=instTraces.begin(); it2!=instTraces.end(); ++it2) {
            InstTracePtr i = *it2;
            Instruction *inst = i->getInstruction();
            if (currentBlock && currentBlock!=inst->getParent()) {
                prevExecutedBlock = currentBlock;
            }
            currentBlock = inst->getParent();
            // New block
            if (currentBlock!=lastBlock) {
                lastBlock = currentBlock;
                prev_inSafePath = inSafePath;
                // Check if inst is in a "bug free block"
                // currentBlock \in bfBlocks?
                bool isIn = false;
                if (bfBlocks.size()<=failBlocks.size()) {
                    isIn = bfBlocks.find(currentBlock)!=bfBlocks.end();
                } else {
                    isIn = failBlocks.find(currentBlock)==failBlocks.end();
                }
                if (!isIn) {
                    inSafePath = false;
                } else {
                    inSafePath = true;
                    if (!firstBugFreeBlock) {
                        firstBugFreeBlock = currentBlock;
                    }
                }
            } 
            // Same block
            else {
                lastBlock = currentBlock;
            }
            // Check if inst is used in an assert
            if (MDNode *N = inst->getMetadata("dbg")) {
                DILocation Loc(N);
                unsigned line = Loc.getLineNumber();
                if (ctx->isAssertCall(line)) {
                    continue;
                }
            }
            
            if (!inSafePath) {
                // If previous block was successful
                if (prev_inSafePath) {
                    if (!andExprs.empty()) {
                        // (and (= x 0) ... bbn)
                        ExprPtr andExpr;
                        if (andExprs.size()==1) {
                            andExpr = andExprs.back();
                        }  else if (andExprs.size()>1) {
                            andExpr = Expression::mkAnd(andExprs);
                        }
                        andExprs.clear();
                        
                        // (or !bb (....))
                        ExprPtr e         = Expression::mkBoolVar(firstBugFreeBlock->getName().str());
                        ExprPtr bbExpr    = Expression::mkNot(e);
                        ExprPtr traceExpr = Expression::mkOr(bbExpr, andExpr);
                        // Is already in?
                        bool isAlreadyIn = false;
                        for (ExprPtr e : allExprs) {
                            if (traceExpr == e) {
                                isAlreadyIn = true;
                                break;
                            }
                        }
                        if (!isAlreadyIn) {
                            allExprs.push_back(traceExpr);
                        }
                    }
                }
                // Reset
                firstBugFreeBlock = NULL;
                prev_inSafePath = false;
                // Skip to the next instruction trace
                continue;
            }
            
            // Branch instructions (special case)
            if (BranchInst *br = dyn_cast<BranchInst>(inst)) {
                BasicBlock *targetbb;
                if (br->isConditional()) {
                    Value *concrete = t->getConcreteValue(br->getCondition());
                    if (!concrete) {
                        error("no concrete value!");
                    }
                    if (br->getNumSuccessors()!=2) {
                        error("unsupported br!");
                    }
                    int val = 0;
                    if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                        val = (int) ci->getSExtValue();
                    } else {
                        error("no concrete value for variable!");
                    }
                    if (val) {
                        targetbb = br->getSuccessor(0);
                    } else {
                        targetbb = br->getSuccessor(1);
                    }
                }
                // Unconditional branch
                else {
                    targetbb = br->getSuccessor(0);
                }
                // targetbb \in bfBlocks?
                bool isIn = false;
                if (bfBlocks.size()<=failBlocks.size()) {
                    isIn = bfBlocks.find(targetbb)!=bfBlocks.end();
                } else {
                    isIn = failBlocks.find(targetbb)==failBlocks.end();
                }
                if (!isIn) {
                    // (nextbb)
                    ExprPtr e1 = Expression::mkBoolVar(targetbb->getName().str());
                    andExprs.push_back(e1);
                    // (bb_nextbb)
                    ExprPtr e2 = ctx->getTransition(currentBlock, targetbb); 
                    andExprs.push_back(e2);
                }
                continue;
            }
            
            // ------------------------------------------------
            // Collect RHS variables computed in failing blocks
            // ------------------------------------------------
            std::set<Value*> varsUsedInFailBlocks;
            for (User::op_iterator itb=inst->op_begin(), ite=inst->op_end(); itb!=ite; ++itb) {
                Value *v = *itb;
                // Phi
                if (PHINode *phi = dyn_cast<PHINode>(inst)) {
                    if (!prevExecutedBlock) {
                        error("no prev exe block");
                    }
                    Value *vTaken = phi->getIncomingValueForBlock(prevExecutedBlock);
                    if (!vTaken) {
                        error("no phi incom. value");
                    }
                    if (vTaken!=v) {
                        continue;
                    }
                }
                // Instruction
                if (Instruction *opI = dyn_cast<Instruction>(v)) {
                    BasicBlock *parentBlock = opI->getParent();
                    const bool isIn = bfBlocks.find(parentBlock)!=bfBlocks.end();
                    if (!isIn) {
                        varsUsedInFailBlocks.insert(opI);
                    }
                }
                // Argument
                else if (Argument *arg = dyn_cast<Argument>(v)) {
                    varsUsedInFailBlocks.insert(arg);
                }  
                // BasicBlock
                //else if (BasicBlock *bb = dyn_cast<BasicBlock>(v)) {
                //    const bool isIn = bfBlocks.find(bb)!=bfBlocks.end();
                //    if (!isIn) {
                //        varsUsedInFailBlocks.insert(bb);
                //    }
                //} 
                // ConstantInt
                else if (isa<ConstantInt>(v)) {
                } 
                else {
                    v->dump();
                    error("not supported");
                }
            }
            // ------------------------------------------------
            // Collect LHS variables computed in failing blocks
            // ------------------------------------------------
            for (Value::use_iterator itb=inst->use_begin(), ite=inst->use_end(); itb!=ite; ++itb) {
                Value *v = *itb;
                if (Instruction *useI = dyn_cast<Instruction>(v)) {
                    BasicBlock *parentBlock = useI->getParent();
                    const bool isIn = bfBlocks.find(parentBlock)!=bfBlocks.end();
                    if (!isIn) {
                        varsUsedInFailBlocks.insert(inst);
                        break;
                    }
                }
                else {
                    v->dump();
                    error("value not supported");
                }
            }
            
            // ------------------------------------------------
            // Construct the expressions 
            // ------------------------------------------------
            std::set<Value*>::iterator it4;
            for (it4=varsUsedInFailBlocks.begin(); it4!=varsUsedInFailBlocks.end(); ++it4) {
                Value *v = *it4;
                // Unsupported instructions
                if (isa<SExtInst>(v) || isa<ZExtInst>(v) || isa<StoreInst>(v) ||
                    isa<LoadInst>(v) || isa<GetElementPtrInst>(v) ) {
                    inst->dump();
                    v->dump();
                    error("instruction not supported!");
                }
                std::string name = v->getName().str();
                // Get the concrete value
                Value *concrete = t->getConcreteValue(v);
                if (!concrete) {
                    continue;
                }
                int val = 0;
                if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                    val = (int) ci->getSExtValue();
                } else {
                    error("no concrete value for variable!");
                }
                ExprPtr varExpr;
                ExprPtr valExpr;
                Type *t = v->getType();
                if (t->isIntegerTy(1) || t->isLabelTy()) {
                    varExpr = Expression::mkBoolVar(name);
                    if (val) {
                        valExpr = Expression::mkTrue();
                    } else {
                        valExpr = Expression::mkFalse();
                    }
                } 
                // Integer 32 bits
                else if (t->isIntegerTy(32)) {
                    varExpr = Expression::mkIntVar(name);
                    valExpr = Expression::mkSInt32Num(val);
                } else {
                     error("unsupported type");
                }
                // (= x 0)
                ExprPtr eqExpr = Expression::mkEq(varExpr, valExpr);
                // Is already in?
                bool isAlreadyIn = false;
                for (ExprPtr e : andExprs) {
                    if (eqExpr == e) {
                        isAlreadyIn = true;
                        break;
                    }
                }
                if (!isAlreadyIn) {
                    andExprs.push_back(eqExpr);   
                }
            } // end for varsUsedInFailBlocks
            
        } // end for instTraces
    } // end for traces
    
    // (and (...) (...)  ... ) 
    ExprPtr resExpr = NULL;
    if (allExprs.size()==1) {
        resExpr = allExprs.back();
    } else if (allExprs.size()>1) {
        resExpr = Expression::mkOr(allExprs);
    }
    if (options->dbgMsg()) {
        std::cout << "Nb. CEPP: " << allExprs.size() << std::endl;
    }
    return resExpr;
}



// =============================================================================
// constructPartialPart (old)
// 
// Encode the partial part of TF (if the option -ptf is used).
//
// bool isUsedOutsideOfBlock(const BasicBlock *BB) const;
// =============================================================================
ExprPtr EncoderPass::constructPartialPart_old() {
    
    std::set<BasicBlock*> bfBlocks = profile->getBugFreeBlocks();
    if (bfBlocks.empty()) {
        std::cout << "No safe path segments for constructing PTF..."<< std::endl;
        return NULL;
    }
    std::set<BasicBlock*> failBlocks = profile->getFailingRunBB();

    /*ReversePostOrderTraversal<Function*> RPOT(this->targetFun);
    std::set<BasicBlock*> funBlockSet(RPOT.begin(), RPOT.end());
    std::set<BasicBlock*> notBfBlock;
    std::set_difference(funBlockSet.begin(), funBlockSet.end(),
                        bfBlocks.begin(), bfBlocks.end(),
                        std::inserter(notBfBlock, notBfBlock.end()));
    
    //if (o->dbgMsg()) {
    std::cout << "All basicblocks: ";
    std::set<BasicBlock*>::iterator its1;
    for (its1=funBlockSet.begin(); its1!=funBlockSet.end(); ++its1) {
        BasicBlock *bb = *its1;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    //}
    std::cout << "Bug free basicblocks: ";
    std::set<BasicBlock*>::iterator its2;
    for (its2=bfBlocks.begin(); its2!=bfBlocks.end(); ++its2) {
        BasicBlock *bb = *its2;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    //}
    std::cout << "Failing basicblocks: ";
    std::set<BasicBlock*>::iterator its3;
    for (its3=notBfBlock.begin(); its3!=notBfBlock.end(); ++its3) {
        BasicBlock *bb = *its3;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    
    //}
    std::cout << "Not Bug free basicblocks: ";
    std::set<BasicBlock*>::iterator its4;
    for (its4=failBlocks.begin(); its4!=failBlocks.end(); ++its4) {
        BasicBlock *bb = *its4;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;*/
    
    
    
    // Iterate on the program traces
    std::vector<ExprPtr> allExprs; 
    std::vector<ProgramTrace*> traces = profile->getSuccessfulProgramTraces();
    std::vector<ProgramTrace*>::iterator it1;
    for (it1=traces.begin(); it1!=traces.end(); ++it1) {
        ProgramTrace *t = *it1;
        //std::cout << "\n<program trace>\n";
        // Iterate on the instruction traces
        bool       inSafePath         = false;
        bool       prev_inSafePath    = false;
        BasicBlock *currentBlock      = NULL;
        BasicBlock *lastBlock         = NULL;
        BasicBlock *prevExecutedBlock = NULL;
        BasicBlock *firstBugFreeBlock = NULL;
        ExprPtr    traceExpr          = NULL;
        
        std::vector<ExprPtr> andExprs;
        std::vector<InstTracePtr> instTraces = t->getInstructionTraces();
        std::vector<InstTracePtr>::iterator it2;
        for (it2=instTraces.begin(); it2!=instTraces.end(); ++it2) {
            InstTracePtr i = *it2;
            Instruction *inst = i->getInstruction();
            if (currentBlock && currentBlock!=inst->getParent()) {
                prevExecutedBlock = currentBlock;
            }
            currentBlock = inst->getParent();
            //inst->dump();
            //std::cout << "<instru trace [" << currentBlock->getName().str() << "]>\n";
    
            // New block
            if (currentBlock!=lastBlock) {
                lastBlock = currentBlock;
                prev_inSafePath = inSafePath;
                // Check if inst is in a "bug free block"
                const bool isIn = bfBlocks.find(currentBlock)!=bfBlocks.end();
                if (!isIn) {
                    // We are in a failing block
                    inSafePath = false;
                } else {
                    // We are in a successful block
                    inSafePath = true;
                    if (!firstBugFreeBlock) {
                        firstBugFreeBlock = currentBlock;
                    }
                }
            } 
            // Same block
            else {
                lastBlock = currentBlock;
            }
            if (!inSafePath) {
                // If previous block was successful
                if (prev_inSafePath) {
                    if (!andExprs.empty()) {
                        // (and (= x 0) ... bbn)
                        ExprPtr andExpr;
                        if (andExprs.size()==1) {
                            andExpr = andExprs.back();
                        }  else if (andExprs.size()>1) {
                            andExpr = Expression::mkAnd(andExprs);
                        }
                        andExprs.clear();
                        
                        // (or !bb (....))
                        ExprPtr e      = Expression::mkBoolVar(firstBugFreeBlock->getName().str());
                        ExprPtr bbExpr = Expression::mkNot(e);
                        traceExpr      = Expression::mkOr(bbExpr, andExpr);
                        allExprs.push_back(traceExpr);
                    }
                }
                // Reset
                firstBugFreeBlock = NULL;
                prev_inSafePath = false;
                // Skip to the next instruction trace
                continue;
            }
    
            // Collect RHS variables computed in failing blocks
            std::set<Value*> varsUsedInFailBlocks;
            for (User::op_iterator itb=inst->op_begin(), ite=inst->op_end(); itb!=ite; ++itb) {
                Value *v = *itb;
                //std::cout << v->getValueID() << std::endl;
                // Instruction
                if (Instruction *opI = dyn_cast<Instruction>(v)) {
                    BasicBlock *parentBlock = opI->getParent();
                    const bool isIn = bfBlocks.find(parentBlock)!=bfBlocks.end();
                    if (!isIn) {
                        varsUsedInFailBlocks.insert(opI);
                    }
                }
                // BasicBlock
                else if (isa<BasicBlock>(v)) {
                } 
                // Argument
                else if (Argument *arg = dyn_cast<Argument>(v)) {
                    varsUsedInFailBlocks.insert(arg);
                }  
                // ConstantInt
                else if (isa<ConstantInt>(v)) {
                } 
                else {
                    v->dump();
                    error("not supported");
                }
            }
            // Collect RHS variables computed in failing blocks
            for (Value::use_iterator itb=inst->use_begin(), ite=inst->use_end(); itb!=ite; ++itb) {
                Value *v = *itb;
                if (Instruction *useI = dyn_cast<Instruction>(v)) {
                    BasicBlock *parentBlock = useI->getParent();
                    const bool isIn = bfBlocks.find(parentBlock)!=bfBlocks.end();
                    if (!isIn) {
                        varsUsedInFailBlocks.insert(inst);
                        break;
                    }
                }
                // ConstantInt
                else if (isa<ConstantInt>(v)) {
                } 
                else {
                    v->dump();
                    error("not supported");
                }
            }
            
            // =========================
            // Construct the expressions 
            // =========================
            
            
            // Branch instruction
            if (BranchInst *br = dyn_cast<BranchInst>(inst)) {
                BasicBlock *targetbb;
                if (br->isConditional()) {
                    Value *concrete = t->getConcreteValue(br->getCondition());
                    if (!concrete) {
                        error("no concrete value!");
                    }
                    if (br->getNumSuccessors()!=2) {
                        error("unsupported br!");
                    }
                    int val = 0;
                    if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                        val = (int) ci->getSExtValue();
                    } else {
                        error("no concrete value for variable!");
                    }
                    if (val) {
                        targetbb = br->getSuccessor(0);
                    } else {
                        targetbb = br->getSuccessor(1);
                    }
                }
                // Unconditional branch
                else {
                    targetbb = br->getSuccessor(0);
                }
                const bool isIn = bfBlocks.find(targetbb)!=bfBlocks.end();
                if (!isIn) {
                    // (nextbb)
                    ExprPtr e1 = Expression::mkBoolVar(targetbb->getName().str());
                    andExprs.push_back(e1);
                    // (bb_nextbb)
                    ExprPtr e2 = ctx->getTransition(currentBlock, targetbb); 
                    andExprs.push_back(e2);
                }
            } 
            // Phi
            else if (PHINode *phi = dyn_cast<PHINode>(inst)) {
                if (!lastBlock) {
                     error("no last block");
                }
                Value *vTaken = phi->getIncomingValueForBlock(prevExecutedBlock);
                if (!vTaken) {
                    error("no phi incom. value");
                }                
                // LHS: Is the ouput value used in a failing block
                const bool isIn1 = varsUsedInFailBlocks.find(phi)!=varsUsedInFailBlocks.end();
                // RHS: Is the selected value computed in a failing block
                const bool isIn2 = varsUsedInFailBlocks.find(vTaken)!=varsUsedInFailBlocks.end();
                // LHS
                if (isIn1) {
                    // The LHS var of a phi is equal to the selected var
                    Value *concrete = t->getConcreteValue(vTaken);
                    if (concrete) {
                        int val = 0;
                        if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                            val = (int) ci->getSExtValue();
                        } else {
                            error("no concrete value for variable!");
                        }
                        ExprPtr varExpr;
                        ExprPtr valExpr;
                        std::string name = phi->getName().str();
                        if (phi->getType()->isIntegerTy(1)) {
                            varExpr = Expression::mkBoolVar(name);
                            if (val) {
                                valExpr = Expression::mkTrue();
                            } else {
                                valExpr = Expression::mkFalse();
                            }
                            // TODO: check for other types
                        } else {
                            varExpr = Expression::mkIntVar(name);
                            valExpr = Expression::mkSInt32Num(val);
                        }
                        // (= x 0)
                        ExprPtr eqExpr = Expression::mkEq(varExpr, valExpr);
                        andExprs.push_back(eqExpr);
                    } else {
                        vTaken->dump();
                        error("no concrete value for phi");
                    }
                }
                // RHS
                varsUsedInFailBlocks.clear();
                if (isIn2) {
                    varsUsedInFailBlocks.insert(vTaken);
                }
            }
            // SExt, ZExt, Store, Load, GetElementPtr
            else if (isa<SExtInst>(inst) || isa<ZExtInst>(inst) || isa<StoreInst>(inst) || 
                     isa<LoadInst>(inst) || isa<GetElementPtrInst>(inst) ) {
                inst->dump();
                error("instruction not supported!");
            }
            
            std::set<Value*>::iterator it4;
            for (it4=varsUsedInFailBlocks.begin(); it4!=varsUsedInFailBlocks.end(); ++it4) {
                Value *v = *it4;
                std::string name = v->getName().str();
                // Get the concrete value
                Value *concrete = t->getConcreteValue(v);
                if (!concrete) {
                    continue;
                }
                int val = 0;
                if(ConstantInt *ci = dyn_cast<ConstantInt>(concrete)) {
                    val = (int) ci->getSExtValue();
                } else {
                    error("no concrete value for variable!");
                }
                ExprPtr varExpr;
                ExprPtr valExpr;
                if (v->getType()->isIntegerTy(1) || isa<BasicBlock>(v)) {
                    varExpr = Expression::mkBoolVar(name);
                    if (val) {
                        valExpr = Expression::mkTrue();
                    } else {
                        valExpr = Expression::mkFalse();
                    }
                    // TODO: check for other types
                } else {
                    varExpr = Expression::mkIntVar(name);
                    valExpr = Expression::mkSInt32Num(val);
                }
                // (= x 0)
                ExprPtr eqExpr = Expression::mkEq(varExpr, valExpr);
                
                // Is already in?
                // TODO: isEqual for Expression
                bool isAlreadyIn = false;
                std::vector<ExprPtr>::const_iterator cit;
                for (cit=andExprs.begin(); cit!=andExprs.end(); ++cit) {
                    ExprPtr e = *cit;
                    if (EqExprPtr expr = std::static_pointer_cast<EqExpression>(e)) {
                        ExprPtr e1 = expr->getExpr1();
                        ExprPtr e2 = expr->getExpr2();
                        IntVarExprPtr expr1 = std::static_pointer_cast<IntVarExpression>(e1);
                        SInt32NumExprPtr expr2 = std::static_pointer_cast<SInt32NumExpression>(e2);
                        if (expr1 && expr2 && expr1->getName()==name && expr2->getValue()==val) {
                            isAlreadyIn = true;
                        }
                    }
                }
                if (!isAlreadyIn) {
                    andExprs.push_back(eqExpr);   
                }
            }
        }
    }
    // (and (...) (...)  ... ) 
    ExprPtr resExpr = NULL;
    if (allExprs.size()==1) {
        resExpr = allExprs.back();
    } else if (allExprs.size()>1) {
        resExpr = Expression::mkOr(allExprs);
    } 
    return resExpr;
}


// =============================================================================
// init_globalValues 
// =============================================================================
void EncoderPass::initGlobalVariables() {
    Module *llvmMod = this->targetFun->getParent();
    if (!llvmMod->global_empty()) {
        //error("global variables not supported!");
    }
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