/**
 * EncoderLightPass.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/04/02
 * @copyright : NII 2012
 */


#include "EncoderLightPass.h"


EncoderLightPass::EncoderLightPass(Function *_targetFun, Context *_ctx, LoopInfoPass *_loops, 
                         ProgramProfile *_profile, Options *_options) 
: targetFun(_targetFun), ctx(_ctx), loops(_loops), profile(_profile), options(_options) { 
    this->encoder = new EncoderLight(ctx);
    // Create an empty AS formula (for pre- and post-condition)
    this->AS = new Formula();
}

EncoderLightPass::~EncoderLightPass() { 
    delete encoder;
}

// =============================================================================
// makeFormula
// 
// Encode the target function into a partial SMT formula.
// =============================================================================
Formula* EncoderLightPass::makeTraceFormula() {
    
    if (options->verbose() || options->dbgMsg()) {
        std::cout << "EncoderLightPass [activated]\n";
    }
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
        // PFTF: Do not encode bug free blocks
        // HFTF: Encode bug free blocks as hard
        bool doEncodeBB = true;
        if (options->ptfUsed() || options->htfUsed()) {
            if(profile->isBugFreeBlock(bb)) {
                doEncodeBB = false;
            }
        }
        // Iterate through the basicblocks
        for (BasicBlock::iterator iti=bb->begin(), eti=bb->end(); iti!=eti; ++iti) {
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
                    break;
                case Instruction::Load:
                    expr = encoder->encode(cast<LoadInst>(i), AS);
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
                            error("Encoder-light Pass");
                        }
                        // New line, Add the collect constraints to the formula
                        if (line!=oldLine && oldLine!=0) {
                            if (currentConstraits.empty() || !lastInstruction) {
                                error("Encoder-light Pass");
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
                        error("Encoder-light Pass");
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
        if (options->lineGranularityLevel()) {
            if (!lastInstruction) {
                error("Encoder-light Pass");
            }
            ExprPtr e = Expression::mkAnd(currentConstraits);
            e->setInstruction(lastInstruction);
            e->setSoft();
            formula->add(e);
            currentConstraits.clear();
        } else {
            error("Encoder-light Pass");
        }
    }
    if(options->printDuration()) {
        timer1.stop("Trace Formula Encoding Time");
    }
    // Pruned Flow-sensitive Trace Formula
    if (options->ptfUsed()) {
          error("Pruned flow-sensitive trace formula is desactivated");
//        MSTimer timer2;
//        if(options->printDuration()) {
//            timer2.start();
//        }
//        ExprPtr pp = constructPartialPart();
//        if (pp) {
//            formula->assertHard(pp);
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
Formula *EncoderLightPass::getASFormula() {
    return AS;
}

// =============================================================================
// constructPartialPart
//
// Encode the partial part of TF (if the option -ptf is used).
//
// bool isUsedOutsideOfBlock(const BasicBlock *BB) const;
// =============================================================================
ExprPtr EncoderLightPass::constructPartialPart() {
    
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
    
    typedef std::vector<ExprPtr> ExprVec;
    std::vector<Path*> paths;
    // Set of the last bug free block of a safe path
    std::set<BasicBlock*> endBugFreeBlocks;
    
    /*std::cout << "Bug Free Blocks: ";
    for (BasicBlock *bb : bfBlocks) {
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
     */
    
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
            
            // Check if inst is used in an assert
            /*if (MDNode *N = inst->getMetadata("dbg")) {
                DILocation Loc(N);
                unsigned line = Loc.getLineNumber();
                if (ctx->isAssertCall(line)) {
                    continue;
                }
            }*/
            
            // New block
            if (currentBlock!=lastBlock) {
                //lastBlock = currentBlock;
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
                // Just entered a safe path
                if (!prev_inSafePath && inSafePath) {
                    // Create a new static path
                    Path *p = new Path();
                    if (p==NULL) {
                        error("Can't allocate memory for path!");
                    }
                    p->pred = lastBlock;
                    p->bb1  = currentBlock;
                    paths.push_back(p);
                }
            }
            // Just exit a safe path
            if (!inSafePath && prev_inSafePath) {
                endBugFreeBlocks.insert(lastBlock);
                // Update last path
                Path *p = paths.back();
                p->bb2 = lastBlock;
                p->next = currentBlock;
                if (andExprs.empty()) { // no vars

                } else {
                    // (and (= var1 cst1) ... (= varn cstn))
                    //ExprPtr andExpr = Expression::mkAnd(andExprs);
                    //andExprs.clear();
                    //p->clauses = andExpr;
                    
                    p->clauses = andExprs;
                    andExprs.clear();
                }
            }
            lastBlock = currentBlock;
            
            // Not in a safe path
            if (!inSafePath) {
                // Reset
                firstBugFreeBlock = NULL;
                prev_inSafePath = false;
                // Skip to the next instruction trace
                continue;
            }
            
            // Branch instructions (special case)
            if (isa<BranchInst>(inst)) {
                continue;
            }

            
            
            // GEP instructions (special case)
            /*if(GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(inst)) {
                gep->dump();
            }
            // Load instructions (special case)
            else if(LoadInst *ld = dyn_cast<LoadInst>(inst)) {
                ld->dump();
            }
            // Store instructions (special case)
            else if (StoreInst *st = dyn_cast<StoreInst>(inst)) {
                st->dump();
            }*/
            
            
            
            
            // ------------------------------------------------
            // Collect LHS variables computed in failing blocks
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
            // Collect RHS variables computed in failing blocks
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
    
    // Remove duplicate
    for(std::vector<Path*>::iterator iter = paths.begin();
        iter != paths.end(); ) {
        Path *p1 = *iter;
        bool remove = false;
        for (Path *p2 : paths) {
            // Remove p2 if equal to p1
            if (p1!=p2 && p1->equal(p2)) {
                remove = true;
                break;
            }
        }
        if (remove) {
            paths.erase(iter);
            delete p1;
        } else {
            ++iter;
        }
    }
    
    // Keep only one sub path per correct execution path
    for(std::vector<Path*>::iterator iter = paths.begin();
        iter != paths.end(); ) {
        Path *p1 = *iter;
        bool remove = false;
        for (Path *p2 : paths) {
            // Remove p2 if equal to p1
            if (p1!=p2 && p1->hasSameEntryTrans(p2)
                       && p1->hasSameExitTrans(p2)) {
                remove = true;
                break;
            }
        }
        if (remove) {
            paths.erase(iter);
            delete p1;
        } else {
            ++iter;
        }
    }
    
    
    // Collect unset blocks transitions
    std::vector<ExprPtr> blockingTransExprs;
    for (BasicBlock *bb : endBugFreeBlocks) {
        TerminatorInst *term = bb->getTerminator();
        BranchInst *br = dyn_cast<BranchInst>(term);
        if (!br) { // other terminator instructions
            continue;
        }
        bool foundNext1InPath = false;
        bool foundNext2InPath = false;
        BasicBlock *next1 = NULL;
        BasicBlock *next2 = NULL;
        if (br->isUnconditional()) {
            next1 = br->getSuccessor(0);
            foundNext2InPath = true;
        } else { // conditional
            next1 = br->getSuccessor(0);
            next2 = br->getSuccessor(1);
        }
        for (Path *p : paths) {
            if (bb==p->bb2 && next1==p->next) {
                foundNext1InPath = true;
            }
            if (bb==p->bb2 && next2==p->next) {
                foundNext2InPath = true;
            }
            if (foundNext1InPath && foundNext2InPath) {
                break;
            }
        }
        if (!foundNext1InPath) {
            ExprPtr t1 = ctx->getTransition(bb, next1);
            ExprPtr nt1 = Expression::mkNot(t1);
            blockingTransExprs.push_back(nt1);
        }
        if (!foundNext2InPath) {
            ExprPtr t2 = ctx->getTransition(bb, next2);
            ExprPtr nt2 = Expression::mkNot(t2);
            blockingTransExprs.push_back(nt2);
        }
    }
    
    // Create formulas for the paths
    unsigned nbSubPaths = 0;
    std::vector<ExprPtr> formula;
    while (!paths.empty()) {
        Path *p1 = paths.back();
        paths.pop_back();
        std::vector<Path*> tmp;
        tmp.push_back(p1);
        std::vector<Path*>::iterator itp;
        for(itp = paths.begin(); itp != paths.end();) {
            Path *p2 = *itp;
            if (p1->hasSameExitTrans(p2)) {
                itp = paths.erase(itp);
                tmp.push_back(p2);
            } else {
                ++itp;
            }
        }
        // (and VARSi pred_bb1)
        std::vector<ExprPtr> orExprs;
        for (Path *p : tmp) {
            ExprPtr e = ctx->getTransition(p->pred, p->bb1);
            ExprVec v = p->clauses;
            v.push_back(e);
            ExprPtr a = Expression::mkAnd(v);
            orExprs.push_back(a);
            nbSubPaths++;
        }
        ExprPtr e1 = Expression::mkOr(orExprs);
        // (= bb2_next (...))
        ExprPtr e2 = ctx->getTransition(p1->bb2, p1->next);
        ExprPtr e3 = Expression::mkEq(e2, e1);
        formula.push_back(e3);
    }
    
    // Free
    /*while (!paths.empty()) {
        Path *p = paths.back();
        paths.pop_back();
        //p->clauses.clear();
        delete p;
    }*/
    
    if (options->dbgMsg()) {
        std::cout << "Nb. CEPP: " << formula.size() << std::endl;
        std::cout << "Nb. sub paths: " << nbSubPaths << std::endl;
    }

    // Add the blocking constraints to block the unused transitions
    formula.insert(formula.end(), blockingTransExprs.begin(),
                   blockingTransExprs.end());
    
    // (and (...) (...)  ... )
    ExprPtr resExpr = NULL;
    if (!formula.empty()) {
        resExpr = Expression::mkAnd(formula);
    }
    return resExpr;
}


// =============================================================================
// initAssertCalls
//
// Check the line number to see if the current instruction is used in a
// call to assert function. If it is the case, we have to put the related
// constraints as hard.
// =============================================================================
void EncoderLightPass::initAssertCalls() {
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
            if (calledFunName==EncoderLight::SNIPER_ASSERT_RETINT_FUN_NAME
                || calledFunName==EncoderLight::SNIPER_ASSERT_RETVOID_FUN_NAME
                || calledFunName==EncoderLight::SNIPER_ASSUME_RETINT_FUN_NAME
                || calledFunName==EncoderLight::SNIPER_ASSUME_RETVOID_FUN_NAME
                || calledFunName==EncoderLight::ASSERT_FUN_NAME
                || calledFunName==EncoderLight::ASSUME_FUN_NAME) {
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
// init_globalValues
// =============================================================================
void EncoderLightPass::initGlobalVariables() {
    // TODO: support for gloabl variables
    Module *llvmMod = this->targetFun->getParent();
    if (!llvmMod->global_empty()) {
        //error("global variables not supported!");
    }
}
