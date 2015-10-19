/**
 * DynamicDiagnosesEnum.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/04/27
 * @copyright : NII 2013
 */

#include "DynamicDiagnosesEnum.h"

unsigned nbCallsToSolver2 = 0;

DynamicDiagnosesEnum::DynamicDiagnosesEnum(Function *_targetFun,
                                       IWPMaxSATSolver *_solver,
                                       Formula *_formula,
                                       bool _hasArgv, Options *_options) :
targetFun(_targetFun), solver(_solver), formula(_formula),
hasArgv(_hasArgv), options(_options) {
    if (!_options->instructionGranularityLevel()) {
        error("DynamicDiagnosesEnum : set inst granularity level");
    }

}

DynamicDiagnosesEnum::~DynamicDiagnosesEnum() {

}

void DynamicDiagnosesEnum::run(ProgramProfile *prof, CombMethod combineMethod) {
    // Nothing to do
    if (!prof->hasFailingProgramTraces()) {
        return;
    }
    // Init timer
    MSTimer timer;
    if (options->printDuration()) {
        timer.start();
    }
    std::vector<ProgramTrace*> failingTraces = prof->getFailingProgramTraces(options);
    // Add all the post-conditions to the context
    std::vector<ExprPtr> postConds = formula->getPostConditions();
    for(ExprPtr post_expr : postConds) {
        formula->assertHard(post_expr);
    }
    if (options->verbose()) {
        std::cout << "=================================================\n";
        std::cout << "Running Dynamic Diagnoses Enumeration algorithm ";
        if (options->pushPopUsed())  std::cout << "[Push&Pop]";
        if (combineMethod==MHS)      std::cout << "[MHS]";
        else if (combineMethod==PWU) std::cout << "[PWU]";
        else if (combineMethod==FLA) std::cout << "[FLA]";
        else std::cout << "[NOCOMB]";
        if (options->ptfUsed())      std::cout << "[PFTF]";
        else if (options->htfUsed()) std::cout << "[HFTF]";
        else                         std::cout << "[FFTF]";
        if (options->lineGranularityLevel()) std::cout << "[Line-lvl]";
        else if (options->instructionGranularityLevel()) std::cout << "[Inst-lvl]";
        else if (options->blockGranularityLevel()) std::cout << "[Block-lvl]";
        std::cout << "\n\n";
        std::cout << "   number of hard constraints  ";
        std::cout << formula->getNbHardExpr() << std::endl;
        std::cout << "   number of soft constraints  ";
        std::cout << formula->getNbSoftExpr() << std::endl;
        std::cout << "   number of error-in. inputs  ";
        std::cout << failingTraces.size() << std::endl;
        std::cout << "\n\n";
    }
    
    
    //
    // Create a new trace formula with a block-level granularity
    //
    Formula *formula2 = new Formula();
    std::vector<ExprPtr> tmpClauses;
    BasicBlock *lastBlock = NULL;
    Instruction *lastInst = NULL;
    std::vector<ExprPtr> clauses = formula->getExprs();
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
    // Compute MCSes with formula2 and allDiagnosis2 (AllMinMCS3)
    //
    YicesSolver *yices = (YicesSolver*) solver;
    ESVVec eMCSes = allDiagnosis2(formula2, failingTraces, yices);
    std::cout << "Nb calls solver  : " << nbCallsToSolver2 << std::endl;
    nbCallsToSolver2 = 0;
    
    // Flatten
    ESet allExprs;
    for (ESVec v : eMCSes ) {
        for (ESet s : v) {
            for (ExprPtr e : s) {
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
    std::vector<ExprPtr> clauses2 = formula->getExprs();
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
    // Compute MCSes with formula3 and allDiagnosis (AllMinMCS2)
    //
    USVVec MCSes = allDiagnosis(formula3, failingTraces, yices);
    std::cout << "Nb calls solver  : " << nbCallsToSolver2 << std::endl;
    nbCallsToSolver2 = 0;
    

    
    
    
    
    // Combination methods: PWU, MHS, FLA
    USVec combMCSes;
    switch (combineMethod) {
        case MHS:
            // Minimal-hitting set
            combMCSes = combineByMHS(MCSes);
            break;
        case PWU:
            // Pair-wise union
            combMCSes = combineByPWU(MCSes);
            break;
        case FLA:
            // Just flatten the MCSes
            combMCSes = combineByFlatten(MCSes);
            break;
        case NONE:
            // Nothing do do
            break;
        default:
            break;
    }
    if (!combMCSes.empty()) {
        if (options->verbose()) {
            std::cout << "\nCombined MCSes size: ";
            std::cout << combMCSes.size() << std::endl;
            std::cout << "Combined MCSes:\n";
            std::cout << combMCSes << std::endl;
        }
    } else {
        if (combineMethod==NONE) {
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

//
// Compute the root causes (MCSes)
//
USVVec DynamicDiagnosesEnum::allDiagnosis(Formula *TF, std::vector<ProgramTrace*> traces,
                                       YicesSolver *yices) {
    USVVec MCSes;
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
            notai->setInstruction(e->getInstruction());
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
        USVec MCS;
        MCS = allMinMCS2(yices, AV, AVMap);
        if (!MCS.empty()) {
            if (options->printMCS()) {
                std::cout << "\n" << MCS << "\n" << std::endl;
            }
            MCSes.push_back(MCS);
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


//
// Compute the root causes (MCSes)
//
ESVVec DynamicDiagnosesEnum::allDiagnosis2(Formula *TF,
                        std::vector<ProgramTrace*> traces,
                        YicesSolver *yices) {
    
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
            notai->setInstruction(e->getInstruction());
            notai->setLine(e->getLine());
            WF->assertSoft(notai);
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            WF->assertHard(ea);
        }
    }
    ESVVec MCSes;
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
        ESVec MCS;
        MCS = allMinMCS3(yices, AV, AVMap);
        if (!MCS.empty()) {
            MCSes.push_back(MCS);
        } else {
            std::cout << "Empty MCS!\n";
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


// Pair-wise union based combination
USVec DynamicDiagnosesEnum::combineByPWU(USVVec &MCSes) {
    // Compute the MCSes
    USVVec MCSesNoDoublons;
    for (USVec MCS : MCSes) {
        removeDoublons(MCS);
        MCSesNoDoublons.push_back(MCS);
        if (options->printMUS()) {
            USVec MUS;
            HittingSet::getMinimalHittingSets_LP(MCS, MUS);
            std::cout << "MUS: " << MUS << "\n\n";
        }
    }
    USVec combMCSes;
    if (!MCSesNoDoublons.empty()) {
        //removeDoublons(MCSesNoDoublons);
        // Pair-wise union of MCSes to obtain the complete diagnosis
        pairwiseUnion(MCSesNoDoublons, combMCSes);
        removeDoublons(combMCSes);
        removeSubsets(combMCSes);
    }
    return combMCSes;
}

// Minimal-hitting set based combination
USVec DynamicDiagnosesEnum::combineByMHS(USVVec &MCSes) {
    // Compute the MUSes
    USVec MUSes;
    for (USVec MCS : MCSes) {
        removeDoublons(MCS);
        // Compute a MUS with the MHS of a MCS
        USVec MUS;
        HittingSet::getMinimalHittingSets_LP(MCS, MUS);
        // Print MUS
        if (options->printMUS()) {
            std::cout << "MUS: " << MUS << "\n\n";
        }
        MUSes.insert(MUSes.end(), MUS.begin(), MUS.end());
    }
    // Remove doublons
    std::sort(MUSes.begin(), MUSes.end());
    MUSes.erase(std::unique(MUSes.begin(), MUSes.end()), MUSes.end());
    // ACSR
    const unsigned loc = options->getNbLOC();
    if (loc>0) {
        std::cout << "ACSR: " << getCodeSizeReduction(MUSes, loc) << "%\n";;
    }
    // Minimal hitting set of the union of the MUSes
    USVec combMCSes;
    HittingSet::getMinimalHittingSets_LP(MUSes, combMCSes);
    return combMCSes;
}

// Put all elements in MCSes into a single set
// {{{x},{x,y}},{{z}}} -> {x,y,z}
USVec DynamicDiagnosesEnum::combineByFlatten(USVVec &MCSes) {
    USet allElts;
    for (USVec v : MCSes ) {
        for (USet s : v) {
            for (unsigned e : s ) {
                allElts.insert(e);
            }
        }
    }
    USVec tmp;
    tmp.push_back(allElts);
    return tmp;
}

// Input:  a weighted formula in CNF
// Output: a set of minimal MCS (line numbers)
USVec DynamicDiagnosesEnum::allMinMCS2(YicesSolver *yices,
                                    std::vector<BoolVarExprPtr> &AV,
                                    std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    USVec MCSes;
    bool done = false;
    while (!done) {
        nbCallsToSolver2++;
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
                // Save the line numbers
                USet M;
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
                    MCSes.push_back(M);
                }
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

// Input:  a weighted formula in CNF
// Output: a set of minimal MCS (expressions)
ESVec DynamicDiagnosesEnum::allMinMCS3(YicesSolver *yices,
                                       std::vector<BoolVarExprPtr> &AV,
                                       std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    ESVec MCSes;
    bool done = false;
    while (!done) {
        nbCallsToSolver2++;
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
                // Save the expressions
                ESet M(U.begin(), U.end());;
                MCSes.push_back(M);
                // Add the blocking constraint to block the last solution
                ExprPtr blockFormula = Expression::mkOr(U);
                blockFormula->setHard();
                yices->addToContext(blockFormula);
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
// checkControlFlowIte
//
// Check the given model for control flow conflicts.
// =============================================================================
void DynamicDiagnosesEnum::checkControlFlow(YicesSolver *solver) {
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
int DynamicDiagnosesEnum::getBlockTransVal(YicesSolver *s,
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
void DynamicDiagnosesEnum::dumpTransValues(YicesSolver *solver) {
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


// [[[int]]] -> [[int]]
// [[mcs]]   -> [[mcs]]
void DynamicDiagnosesEnum::pairwiseUnion(USVVec MCSes, USVec &Diag) {
    // Init
    int size = MCSes.size();
    unsigned a[size];
    for (int i=0; i<size; ++i) {
        a[i] = 0;
    }
    bool done = false;
    while (!done) {
        // Unions
        USet S;
        for (int i=0; i<size; ++i) {
            USVec A = MCSes[i];
            int j = a[i];
            USet B = A[j];
            S.insert(B.begin(), B.end());
        }
        Diag.push_back(S);
        // Update indexes
        a[0]++;
        for (int i=0; i<size-1; ++i) {
            USVec A = MCSes[i];
            if(a[i]>=A.size()) {
                a[i] = 0;
                a[i+1]++;
            }
        }
        // Finished?
        USVec A = MCSes[size-1];
        if (a[size-1]>=A.size()) {
            done = true;
            break;
        }
    }
}

double DynamicDiagnosesEnum::getCodeSizeReduction(USVec MCSes, unsigned totalNbLine) {

    std::vector<double> CSR(MCSes.size());
    unsigned i = 0;
    USVec::const_iterator it;
    for (it=MCSes.begin(); it!=MCSes.end(); ++it) {
        USet mcs = (*it);
        CSR[i] = ((100.0*(double)mcs.size())/(double)totalNbLine);
        i++;
    }
    double sum = 0;
    std::vector<double>::const_iterator it2;
    for (it2=CSR.begin(); it2!=CSR.end(); ++it2) {
        double crs_i = *it2;
        sum = sum + crs_i;
    }
    return sum/(double)CSR.size();
}

// Copy vv to sv
void DynamicDiagnosesEnum::copy(UVVec &vv, USVec &sv) {
    UVVec::const_iterator itv;
    for (itv=vv.begin(); itv!=vv.end(); ++itv) {
        // Copy the MCS (vector) into a MCS (set)
        USet subset((*itv).begin(), (*itv).end());
        sv.push_back(subset);
    }
}

// Remove all subset doublons
// {{x,y}, {a,b}, {x,y}} -> {{a,b}}
void DynamicDiagnosesEnum::removeDoublons(USVec &v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());   
}

// Remove all subsets of subsets
// {{x}, {a,b}, {x,y}} -> {{a,b},{x,y}}
void DynamicDiagnosesEnum::removeSubsets(USVec &v) {
    std::vector<std::set<unsigned> >::iterator itm;
    for (itm=v.begin(); itm!=v.end();) {
        std::set<unsigned> s(*itm);
        unsigned i = 0;
        bool isIn = false;
        while (i<v.size()) {
            if (s!=v[i]) {
                isIn = std::includes(v[i].begin(), v[i].end(), s.begin(),s.end());
                if (isIn) {
                    break;
                }
            }
            i++;
        }
        if (isIn) {
            itm = v.erase(itm); 
        } else {
            ++itm;
        }
    }    
}
/*
std::ostream& operator<<(std::ostream& os, const UVec& v) {
    os << "{";
    for(UVec::const_iterator it = v.begin(); it != v.end(); it++) {
        os << *it;
        if (std::distance(it, v.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const UVVec& vv) {
    os << "{";
    for(UVVec::const_iterator it = vv.begin(); it != vv.end(); it++) {
        os << *it;
        if (std::distance(it, vv.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const UVVVec& vvv) {
    os << "{";
    for(UVVVec::const_iterator it = vvv.begin(); it != vvv.end(); it++) {
        os << *it;
        if (std::distance(it, vvv.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const USet& s) {
    os << "{";
    for(USet::const_iterator it = s.begin(); it != s.end(); ++it) {
        os << *it;
        if (std::distance(it, s.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const USVec& s) {
    os << "{";
    for(USVec::const_iterator it = s.begin(); it != s.end(); ++it) {
        os << *it;
        if (std::distance(it, s.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}*/