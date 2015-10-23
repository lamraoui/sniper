/**
 * IterationAlgorithm.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/01/23
 * @copyright : NII 2013
 */

#include "IterationAlgorithm.h"

unsigned nbCallsToSolver = 0;

void IterationAlgorithm::run(Formula *TF, ProgramProfile *prof,
                             Combine::Method combineMethod) {
    // Nothing to do
    if (!prof->hasFailingProgramTraces()) {
        return;
    }
    // Init timer
    MSTimer timer;
    if (options->printDuration()) {
        timer.start();
    }
    std::vector<ProgramTrace*>
    failingTraces = prof->getFailingProgramTraces(options);
    
    // Add all the post-conditions to the context
    std::vector<ExprPtr> postConds = TF->getPostConditions();
    for(ExprPtr post_expr : postConds) {
        TF->assertHard(post_expr);
    }
    if (options->verbose()) {
        std::cout << "=================================================\n";
        std::cout << "Running AllDiagnosis algorithm ";
        if (options->pushPopUsed())  std::cout << "[Push&Pop]";
        if (combineMethod==Combine::MHS)      std::cout << "[MHS]";
        else if (combineMethod==Combine::PWU) std::cout << "[PWU]";
        else if (combineMethod==Combine::FLA) std::cout << "[FLA]";
        else std::cout << "[NOCOMB]";
        if (options->ptfUsed())      std::cout << "[PFTF]";
        else if (options->htfUsed()) std::cout << "[HFTF]";
        else                         std::cout << "[FFTF]";
        if (options->lineGranularityLevel()) std::cout << "[Line-lvl]";
        else if (options->instructionGranularityLevel()) std::cout << "[Inst-lvl]";
        else if (options->blockGranularityLevel()) std::cout << "[Block-lvl]";
        std::cout << "\n\n";
        std::cout << "   number of hard constraints  ";
        std::cout << TF->getNbHardExpr() << std::endl;
        std::cout << "   number of soft constraints  ";
        std::cout << TF->getNbSoftExpr() << std::endl;
        std::cout << "   number of error-in. inputs  ";
        std::cout << failingTraces.size() << std::endl;
        std::cout << "\n\n";
    }
    // Compute the root causes (MCSes)
    YicesSolver *yices = (YicesSolver*) solver;
    std::vector<SetOfFormulasPtr> MCSes = allDiagnosis(TF, failingTraces, yices);
    
    std::cout << "Nb calls solver  : " << nbCallsToSolver << std::endl;
    
    // Combination methods: PWU, MHS, FLA
    SetOfFormulasPtr combMCSes;
    switch (combineMethod) {
        case Combine::MHS:
            // Minimal-hitting set
            combMCSes = Combine::combineByMHS(MCSes);
            break;
        case Combine::PWU:
            // Pair-wise union
            combMCSes = Combine::combineByPWU(MCSes);
            break;
        case Combine::FLA:
            // Just flatten the MCSes
            combMCSes = Combine::combineByFlatten(MCSes);
            break;
        case Combine::NONE:
            // Nothing do do
            break;
        default:
            break;
    }
    if (!combMCSes->empty()) {
        if (options->verbose()) {
            std::cout << "\nCombined MCSes size: ";
            std::cout << combMCSes->size() << std::endl;
            std::cout << "Combined MCSes:\n";
            std::cout << combMCSes << std::endl;
        }
    } else {
        if (combineMethod==Combine::NONE) {
            if (options->verbose()) {
                std::cout << "\nNo combination method...\n";
            }
        } else {
            std::cout << "SNIPER was unable to localize any root causes.\n";
        }
    }
    if (options->printDuration()) {
        timer.stop("\nRun AllDiagnosis Time");
    }
    if (options->verbose()) {
        std::cout << "==============================================\n";
    }
}


// Dynamic Diagnoses Enumeration
void IterationAlgorithm::run_dynamic(Formula *TF, ProgramProfile *prof,
                                     Combine::Method combineMethod) {
    
    if (!options->instructionGranularityLevel()) {
        error("DynamicDiagnosesEnum : set inst granularity level");
    }
    if (!prof->hasFailingProgramTraces()) {
        return; // Nothing to do
    }
    std::vector<ProgramTrace*>
    failingTraces = prof->getFailingProgramTraces(options);
    
    //
    // Create a new trace formula with a block-level granularity
    //
    Formula *formula2 = new Formula();
    std::vector<ExprPtr> tmpClauses;
    BasicBlock *lastBlock = NULL;
    Instruction *lastInst = NULL;
    std::vector<ExprPtr> clauses = TF->getExprs();
    for (ExprPtr e : clauses) {
        // Hard clause
        if (e->isHard()) {
            formula2->assertHard(e);
        }
        // Soft clause
        else {
            Instruction *I = e->getInstruction();
            if (!I) error("DynamicDiagnosesEnum");
            BasicBlock *currentBlock = I->getParent();
            if (!currentBlock) error("DynamicDiagnosesEnum");
            // New block
            if (currentBlock!=lastBlock && lastBlock!=NULL) {
                if (tmpClauses.empty()) error("DynamicDiagnosesEnum");
                ExprPtr blockExpr = Expression::mkAnd(tmpClauses);
                blockExpr->setInstruction(lastInst);
                blockExpr->setSoft();
                formula2->assertSoft(blockExpr);
                tmpClauses.clear();
            }
            tmpClauses.push_back(e);
            lastInst = I;
            lastBlock = currentBlock;
        }
    }
    // Remaining clauses?
    if (!tmpClauses.empty()) {
        ExprPtr blockExpr = Expression::mkAnd(tmpClauses);
        blockExpr->setInstruction(lastInst);
        blockExpr->setSoft();
        formula2->assertSoft(blockExpr);
        tmpClauses.clear();
    }
    
    std::cout << "\nFormula 2:\n";
    std::cout << "   number of hard constraints  ";
    std::cout << formula2->getNbHardExpr() << std::endl;
    std::cout << "   number of soft constraints  ";
    std::cout << formula2->getNbSoftExpr() << std::endl;
    
    //
    // Compute MCSes with formula2 and allDiagnosis (AllMinMCS)
    //
    YicesSolver *yices = (YicesSolver*) solver;
    std::vector<SetOfFormulasPtr>
    MCSes = allDiagnosis(formula2, failingTraces, yices);
    std::cout << "Nb calls solver  : " << nbCallsToSolver << std::endl;
    nbCallsToSolver = 0;
    
    // Flatten
    // TODO: use std::insert()
    std::set<ExprPtr> allExprs;
    for (SetOfFormulasPtr v : MCSes ) {
        std::vector<FormulaPtr> F = v->getFormulas();
        for (FormulaPtr f : F) {
            std::vector<ExprPtr> E = f->getExprs();
            for (ExprPtr e : E) {
                allExprs.insert(e);
            }
        }
    }
    // Collect suspicious blocks
    std::set<BasicBlock*> suspicousBlocks;
    for (ExprPtr e : allExprs) {
        Instruction *I = e->getInstruction();
        if (!I) error("DynamicDiagnosesEnum");
        BasicBlock *bb = I->getParent();
        if (!bb) error("DynamicDiagnosesEnum");
        suspicousBlocks.insert(bb);
    }
    
    
    // Debug
    std::cout << "Suspicous blocks: " << suspicousBlocks.size() << "/"
    << targetFun->size() << std::endl;
    
    
    //
    // Create a new trace formula with all block set
    // as hard except for the suspicous blocks.
    //
    Formula *formula3 = new Formula();
    std::vector<ExprPtr> clauses2 = TF->getExprs();
    for (ExprPtr e : clauses2) {
        // Hard clause
        if (e->isHard()) {
            formula3->assertHard(e);
        }
        // Soft clause
        else {
            Instruction *I = e->getInstruction();
            if (!I) error("DynamicDiagnosesEnum");
            BasicBlock *bb = I->getParent();
            if (!bb) error("DynamicDiagnosesEnum");
            
            const bool isSuspicous
            = (suspicousBlocks.find(bb)!=suspicousBlocks.end());
            if (!isSuspicous) {
                formula3->assertHard(e);
            } else {
                e->setSoft();
                formula3->assertSoft(e);
            }
        }
    }
    
    std::cout << "\nFormula 3:\n";
    std::cout << "   number of hard constraints  ";
    std::cout << formula3->getNbHardExpr() << std::endl;
    std::cout << "   number of soft constraints  ";
    std::cout << formula3->getNbSoftExpr() << std::endl;
    
    //
    // Compute MCSes with formula3 and allDiagnosis (AllMinMCS)
    //
    std::vector<SetOfFormulasPtr>
    MCSes2 = allDiagnosis(formula3, failingTraces, yices);
    std::cout << "Nb calls solver  : " << nbCallsToSolver << std::endl;
    nbCallsToSolver = 0;
    
    
}



//
// Compute the root causes (MCSes)
//
std::vector<SetOfFormulasPtr>
IterationAlgorithm::allDiagnosis(Formula *TF,
                                 std::vector<ProgramTrace*> traces,
                                 YicesSolver *yices) {
    std::vector<SetOfFormulasPtr> MCSes;
    int progress = 0;
    int total = traces.size();
    // Working formula
    Formula *WF = new Formula(TF);
    // For each E in WF, E tagged as soft do
    unsigned z = 0;
    std::vector<BoolVarExprPtr> AV;
    std::map<BoolVarExprPtr, ExprPtr> AVMap;
    std::vector<ExprPtr> clauses = WF->getExprs();
    for(ExprPtr e : clauses) {
        if (e->isSoft()) {
            // AI is a new auxiliary var. created
            std::ostringstream oss;
            oss << z++;
            BoolVarExprPtr ai = Expression::mkBoolVar("a_"+oss.str());
            ai->setInstruction(e->getInstruction());
            ai->setLine(e->getLine());
            AV.push_back(ai);
            AVMap[ai] = e;
            ExprPtr notai = Expression::mkNot(ai);
            //notai->setWeight(e->getWeight());
            notai->setLine(e->getLine());
            WF->assertSoft(notai);
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            WF->assertHard(ea);
        }
    }
    if (AV.empty()) {
        delete WF;
        std::cout << "No MaxSMT solution!\n";
        if (options->verbose()) {
            std::cout << "==============================================\n";
        }
        return MCSes;
    }
    yices->init();
    yices->addToContext(WF);
    for(ProgramTrace *E : traces) {
        if (options->verbose()) {
            displayProgressBar(progress, total);
        }
        // At this point there is only WF in the context
        yices->push();
        // Assert as hard the error-inducing input formula
        ExprPtr eiExpr = E->getProgramInputsFormula(TF);
        eiExpr->setHard();
        yices->addToContext(eiExpr);
        if (options->dbgMsg()) {
            std::cout << "-- Error-inducing Input: ";
            eiExpr->dump();
            std::cout << std::endl;
        }
        // Compute a MCS
        SetOfFormulasPtr M = allMinMCS(yices, AV, AVMap);
        if (!M->empty()) {
            SetOfFormulasPtr M2 = avToClauses(M, AVMap);
            MCSes.push_back(M2);
            if (options->printMCS()) {
                std::cout << "\n" << M2 << "\n" << std::endl;
            }
        } else {
            //if (options->verbose() || options->dbgMsg()) {
                std::cout << "Empty MCS!\n";
            //}
        }
        // Backtrack to the point where there
        // was no Pre/Post-conditions in the formula
        yices->pop();
        // Progress bar
        progress++;
    }
    yices->clean();
    delete WF;
    if (options->verbose()) {
        displayProgressBar(progress, total);
        std::cout << std::endl;
    }
    return MCSes;
}

// Input:  a weighted formula in CNF
// Output: a set of minimal MCS
SetOfFormulasPtr 
IterationAlgorithm::allMinMCS(YicesSolver *yices,
                              std::vector<BoolVarExprPtr> &AV,
                              std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    SetOfFormulasPtr MCSes = SetOfFormulas::make();
    bool done = false;
    while (!done) {
        nbCallsToSolver++;
        switch(yices->maxSat()) {
            case l_true: {
                // Debug
                if (options->checkCFGModel()) {
                    checkControlFlow(yices);
                }
                // U is a Minimal Correction Subset
                // (MCS) if st is true
                std::vector<ExprPtr> U = solver->getUnsatExpressions();
                if (U.empty()) { // SAT
                    done = true;
                    break;
                }
                // Add the blocking constraint to block the last solution
                ExprPtr blockFormula = Expression::mkOr(U);
                blockFormula->setHard();
                yices->addToContext(blockFormula);
                
                // Save the MCSes
                FormulaPtr m = std::make_shared<Formula>(U);
                MCSes->add(m);
                
                // Save the line numbers
                /*MCSPtr M;
                for (ExprPtr notAi : U) {
                    //M.insert(AVMap[ai]->getLine());
                    //M.insert(ai->getLine());
                    if (options->getTestSuiteFileName().empty()) {
                        M.insert(notAi->getLine());
                    } else {
                        // FOR TCAS
                        if (notAi->getLine()<160) {
                            M.insert(notAi->getLine());
                        }
                    }
                }
                if (!M.empty()) {
                    // Bound MCS size
                    if (M.size()<=options->mcsMaxSize()) {
                         MCSes->add(M);
                    }
                }*/
            } break;
            case l_false: // unsatisfiable
                done = true;
                break;
            case l_undef: // unknown
                done = true;
                break;
        }
    }
    return MCSes;
}


// =============================================================================
// avToClauses
//
// From the negated auxiliary variables (not a_i) in M, retreive
// the corresponding expressions in AVMap and save them in M2.
// =============================================================================
SetOfFormulasPtr
IterationAlgorithm::avToClauses(SetOfFormulasPtr M,
                                std::map<BoolVarExprPtr, ExprPtr> AVMap) {
    SetOfFormulasPtr M2 = SetOfFormulas::make();
    for (FormulaPtr f : M->getFormulas()) {
        FormulaPtr F2 = Formula::make();
        for (ExprPtr notai : f->getExprs()) {
            if (notai->getOpCode()!=Expression::Not) {
                continue;
            }
            NotExprPtr ne =
            std::static_pointer_cast<NotExpression>(notai);
            BoolVarExprPtr ai =
            std::static_pointer_cast<BoolVarExpression>(ne->get());
            auto it = AVMap.find(ai);
            if(it != AVMap.end()) { // Element found
                F2->add(it->second);
            }
        }
        if (!F2->empty()) {
            M2->add(F2);
        }
    }
    return M2;
}


// =============================================================================
// checkControlFlowIte
//
// Check the given model for control flow conflicts.
// =============================================================================
void IterationAlgorithm::checkControlFlow(YicesSolver *solver) {
    std::cout << "Checking control flow...";
    if (!solver) {
        error("checkControlFlowIte: wrong arg");
    }
    // Iterate over basic blocks of the main function
    for (Function::iterator i=targetFun->begin(), e=targetFun->end(); i!=e; ++i) {
        BasicBlock *bb = i;
        // Get the values of the predecessor transitions
        unsigned nbPredTrans      = 0;
        unsigned nbTruePredTrans  = 0;
        unsigned nbUndefPredTrans = 0;
        for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
            BasicBlock *predbb = *PI;
            int val = getBlockTransVal(solver, predbb, bb);
            if (val==l_true) {
                nbTruePredTrans++;
            }
            else if (val==l_undef) {
                nbUndefPredTrans++;
            }
            nbPredTrans++;
        }
        if (nbTruePredTrans>1) {
            error("invalid control flow value (0)");
        }
        if (nbPredTrans==0) { // Entry block doesn't have any predecessors
            nbTruePredTrans = 1;
        }
        const TerminatorInst *t = bb->getTerminator();
        if (isa<ReturnInst>(t)) {
            return; // End of recursion
        }
        const BranchInst *br = dyn_cast<BranchInst>(t);
        if (!br) {
            error("unsupported instruction");
        }
        if (br->isUnconditional()) {
            BasicBlock *nextbb = br->getSuccessor(0);
            int val = getBlockTransVal(solver, bb, nextbb);
            if (nbTruePredTrans==1 && val==l_false) {
                dumpTransValues(solver);
                error("invalid control flow value (1)");
            }
            if (nbTruePredTrans==0 && nbUndefPredTrans==0 && val==l_true) {
                std::cout << bb->getName().str() << std::endl;
                dumpTransValues(solver);
                error("invalid control flow value (2)");
            }
        } else {
            BasicBlock *nextbb1 = br->getSuccessor(0);
            BasicBlock *nextbb2 = br->getSuccessor(1);
            int val1 = getBlockTransVal(solver, bb, nextbb1);
            int val2 = getBlockTransVal(solver, bb, nextbb2);
            if (nbTruePredTrans==1 && val1==l_false && val2==l_false) {
                dumpTransValues(solver);
                error("invalid control flow value (3)");
            }
            if (nbTruePredTrans==0 && nbUndefPredTrans ==0
                && (val1==l_true || val2==l_true)) {
                dumpTransValues(solver);
                error("invalid control flow value (4)");
            }
            if (val1==l_true && val2==l_true) {
                dumpTransValues(solver);
                error("invalid control flow value (5)");
            }
        }
    }
    std::cout << "OK\n";
}

// =============================================================================
// getBlockTransVal
//
// Return the value of the transition <bb1,bb2> in the given model.
// =============================================================================
int IterationAlgorithm::getBlockTransVal(YicesSolver *s,
                                         BasicBlock *bb1, BasicBlock *bb2) {
    if (!s || !bb1 || !bb2) {
        error("getBlockTransVal: wrong arg");
    }
    std::string bbname1 = bb1->getName().str();
    std::string bbname2 = bb2->getName().str();
    std::string t = bbname1 + "_" + bbname2;
    int val = solver->getBoolValue(t);
    return val;
}

// =============================================================================
// dumpTransValues
//
//  Given a model, print all basic block transitions values.
// =============================================================================
void IterationAlgorithm::dumpTransValues(YicesSolver *solver) {
    std::cout <<  "\n---------------------" << std::endl;
    for (Function::iterator i=targetFun->begin(), e=targetFun->end(); i!=e; ++i) {
        BasicBlock *bb = i;
        const TerminatorInst *t = bb->getTerminator();
        if (isa<ReturnInst>(t)) {
            continue; // End
        }
        const BranchInst *br = dyn_cast<BranchInst>(t);
        if (!br) {
            error("unsupported instruction");
        }
        if (br->isUnconditional()) {
            BasicBlock *nextbb = br->getSuccessor(0);
            int val = getBlockTransVal(solver, bb, nextbb);
            std::cout << "[1]" << bb->getName().str() << "_"
            << nextbb->getName().str() <<  " : " << val << std::endl;
        } else {
            BasicBlock *nextbb1 = br->getSuccessor(0);
            BasicBlock *nextbb2 = br->getSuccessor(1);
            int val1 = getBlockTransVal(solver, bb, nextbb1);
            int val2 = getBlockTransVal(solver, bb, nextbb2);
            std::cout << "[2]" << bb->getName().str() << "_"
            << nextbb1->getName().str() <<  " : " << val1 << std::endl;
            std::cout << "[2]" << bb->getName().str() << "_"
            << nextbb2->getName().str() <<  " : " << val2 << std::endl;
        }
    }
    std::cout <<  "---------------------\n" << std::endl;
}
