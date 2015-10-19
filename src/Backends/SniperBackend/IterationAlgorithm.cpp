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

IterationAlgorithm::IterationAlgorithm(Function *_targetFun,
                                       IWPMaxSATSolver *_solver,
                                       Formula *_formula,
                                       bool _hasArgv, Options *_options) :
targetFun(_targetFun), solver(_solver), formula(_formula),
hasArgv(_hasArgv), options(_options) {

}

IterationAlgorithm::~IterationAlgorithm() {

}

void IterationAlgorithm::run(ProgramProfile *prof, CombMethod combineMethod) {
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
        std::cout << "Running AllDiagnosis algorithm ";
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
    // Compute the root causes (MCSes)
    YicesSolver *yices = (YicesSolver*) solver;
    USVVec MCSes = allDiagnosis(failingTraces, yices);
    
    std::cout << "Nb calls solver  : " << nbCallsToSolver << std::endl;
    
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
USVVec IterationAlgorithm::allDiagnosis(std::vector<ProgramTrace*> traces,
                                       YicesSolver *yices) {
    USVVec MCSes;
    int progress = 0;
    int total = traces.size();
    // Working formula
    Formula *WF = new Formula(formula);
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
        ExprPtr eiExpr = E->getProgramInputsFormula(formula);
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

// Pair-wise union based combination
USVec IterationAlgorithm::combineByPWU(USVVec &MCSes) {
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
USVec IterationAlgorithm::combineByMHS(USVVec &MCSes) {
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
USVec IterationAlgorithm::combineByFlatten(USVVec &MCSes) {
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
// Output: a set of minimal MCS
USVec IterationAlgorithm::allMinMCS2(YicesSolver *yices,
                                    std::vector<BoolVarExprPtr> &AV,
                                    std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    USVec MCSes;
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
                    // Bound MCS size
                    if (M.size()<=options->mcsMaxSize()) {
                         MCSes.push_back(M);
                    }
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












// #####################################
// ## TCAS
// #####################################




// =============================================================================
// runTCAS_AR (with assert+/retract optimization)
// =============================================================================
void IterationAlgorithm::runTCAS_AR(std::vector<ExprPtr> TCExprs,
                                    std::vector<ExprPtr> GOExprs) {
    //
    // Some preparation before running 
    // the AllMInMCS algorithm
    //
    Formula *WF = new Formula(formula); // Working formula
    // Auxiliary variables
    std::vector<BoolVarExprPtr> AV;
    std::map<BoolVarExprPtr, ExprPtr> AVMap;
    // for each E in WF, E tagged as soft do
    std::vector<ExprPtr> clauses = WF->getExprs();
    unsigned z = 0;
    std::vector<ExprPtr>::iterator it;
    for(it=clauses.begin(); it!=clauses.end(); it++) {
        ExprPtr e = *it;
        if (e->isSoft()) {
            // AI is a new auxiliary var. created
            std::ostringstream oss;
            oss << z++;
            BoolVarExprPtr ai = Expression::mkBoolVar("a_"+oss.str());
            AV.push_back(ai);
            AVMap[ai] = e;
            ExprPtr notai = Expression::mkNot(ai);
            notai->setWeight(e->getWeight());
            WF->assertSoft(notai); 
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            WF->assertHard(ea);
        }
    }
    if (AV.empty()) {
        std::cout << "No solution!\n";
        delete WF;
        return; 
    }
    YicesSolver *yices = (YicesSolver*) solver;
    yices->init();
    yices->addToContext(WF);
    
    //
    // Compute the root causes
    //    
    unsigned nbDetect = 0;
    std::vector<std::set<unsigned> > MUSes;
    std::vector<ExprPtr>::const_iterator ite;
    for (unsigned i=0; i<TCExprs.size(); i++) 
    {
        // Assert the test case formula (pre-cond) as hard
        TCExprs[i]->setHard();
        assertion_id idTCExpr = yices->addToContextRetractable(TCExprs[i]);
        // Assert the golden ouput formula (not post-cond) as hard
        GOExprs[i]->setHard();
        assertion_id idGOExpr = yices->addToContextRetractable(GOExprs[i]);
        
        //
        // Run the AllMinMCS algorithm
        //
        UVVec M;
        M = allMinMCS_AR(yices, AV, AVMap);
        // Found some MCS!
        if (!M.empty()) 
        {
            nbDetect++;
            USVec Mset;
            UVVec::const_iterator itv;
            copy(M, Mset);
            removeDoublons(Mset);
            // Merge the MUSes together
            USVec MUS;
            HittingSet::getMinimalHittingSets_LP(Mset, MUS);
            MUSes.insert(MUSes.end(), MUS.begin(), MUS.end()); 
        }
        // Backtrack to the point where there 
        // was no Pre/Post-conditions in the formula
        yices->retractFromContext(idTCExpr);
        yices->retractFromContext(idGOExpr);
    }
    // Clean the MUSes
    removeDoublons(MUSes);
    removeSubsets(MUSes);
    // Minimal hitting set the MUSes (conflicts) to obtain the diagnoses
    USVec Diag;
    HittingSet::getMinimalHittingSets_LP(MUSes, Diag);
    
    // Print the result
    std::cout << "#Detect/#TC: " 
    << nbDetect << "/" << TCExprs.size() << std::endl;
    std::cout << "ACSR:        " 
    << getCodeSizeReduction(MUSes, 173) << "%" << std::endl;
    std::cout << "MCSes size:  " << Diag.size() << "\n";
    std::cout << "MCSes:\n";
    std::cout << Diag << std::endl;
    
    delete WF;
    yices->clean();
}


// =============================================================================
// runTCAS_PP (with push-pop optimization)
// =============================================================================
void IterationAlgorithm::runTCAS_PP(std::vector<ExprPtr> TCExprs,
                                    std::vector<ExprPtr> GOExprs) {
    //
    // Some preparation before running 
    // the AllMInMCS algorithm
    //
    Formula *WF = new Formula(formula); // Working formula
    // Auxiliary variables
    std::vector<BoolVarExprPtr> AV;
    std::map<BoolVarExprPtr, ExprPtr> AVMap;
    // for each E in WF, E tagged as soft do
    std::vector<ExprPtr> clauses = WF->getExprs();
    unsigned z = 0;
    std::vector<ExprPtr>::iterator it;
    for(it=clauses.begin(); it!=clauses.end(); it++) {
        ExprPtr e = *it;
        if (e->isSoft()) {
            // AI is a new auxiliary var. created
            std::ostringstream oss;
            oss << z++;
            BoolVarExprPtr ai = Expression::mkBoolVar("a_"+oss.str());
            AV.push_back(ai);
            AVMap[ai] = e;
            ExprPtr notai = Expression::mkNot(ai);
            notai->setWeight(e->getWeight());
            WF->assertSoft(notai); 
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            WF->assertHard(ea);
        }
    }
    if (AV.empty()) {
        std::cout << "No solution!\n";
        delete WF;
        return; 
    }
    YicesSolver *yices = (YicesSolver*) solver;
    yices->init();
    yices->addToContext(WF);

    //
    // Compute the root causes
    //    
    unsigned nbDetect = 0;
    std::vector<std::set<unsigned> > MUSes;
    std::vector<ExprPtr>::const_iterator ite;
    for (unsigned i=0; i<TCExprs.size(); i++) 
    {
        // Only WF in the context
        yices->push();
        
        // Assert the test case formula (pre-cond) as hard
        TCExprs[i]->setHard();
        yices->addToContext(TCExprs[i]);
        // Assert the golden ouput formula (not post-cond) as hard
        GOExprs[i]->setHard();
        yices->addToContext(GOExprs[i]);
        
        //
        // Run the AllMinMCS algorithm
        //
        UVVec M;
        M = allMinMCS_PP(yices, AV, AVMap);
        // Found some MCS!
        if (!M.empty()) 
        {
            nbDetect++;
            USVec Mset;
            UVVec::const_iterator itv;
            copy(M, Mset);
            removeDoublons(Mset);
            // Merge the MUSes together
            USVec MUS;
            HittingSet::getMinimalHittingSets_LP(Mset, MUS);
            MUSes.insert(MUSes.end(), MUS.begin(), MUS.end()); 
        }
        // Backtrack to the point where there 
        // was no Pre/Post-conditions in the formula
        yices->pop();
    }
    // Clean the MUSes
    removeDoublons(MUSes);
    removeSubsets(MUSes);
    // Minimal hitting set the MUSes (conflicts) to obtain the diagnoses
    USVec Diag;
    HittingSet::getMinimalHittingSets_LP(MUSes, Diag);
    
    // Print the result
    std::cout << "#Detect/#TC: " 
    << nbDetect << "/" << TCExprs.size() << std::endl;
    std::cout << "ACSR:        " 
    << getCodeSizeReduction(MUSes, 173) << "%" << std::endl;
    std::cout << "MCSes size:  " << Diag.size() << "\n";
    std::cout << "MCSes:\n";
    std::cout << Diag << std::endl;
    
    delete WF;
    yices->clean();
}



// =============================================================================
// runTCAS (without push-pop optimization, pair-wise union combination)
// =============================================================================
void IterationAlgorithm::runTCAS_PWU(std::vector<ExprPtr> TCExprs,
                                     std::vector<ExprPtr> GOExprs) {

    MSTimer timer1;
    MSTimer timer2;
    if (options->printDuration()) {
        timer1.start();
        timer2.start();
    }
    //
    // Some preparation before running 
    // the AllMInMCS algorithm
    //
    Formula *WF = new Formula(formula); // Working formula
    // Auxiliary variables
    std::vector<BoolVarExprPtr> AV;
    std::map<BoolVarExprPtr, ExprPtr> AVMap;
    // for each E in WF, E tagged as soft do
    std::vector<ExprPtr> clauses = WF->getExprs();
    unsigned i = 0;
    std::vector<ExprPtr>::iterator it;
    for(it=clauses.begin(); it!=clauses.end(); it++) {
        ExprPtr e = *it;
        if (e->isSoft()) {
            // AI is a new auxiliary var. created
            std::ostringstream oss;
            oss << i++;
            BoolVarExprPtr ai = Expression::mkBoolVar("a_"+oss.str());
            AV.push_back(ai);
            AVMap[ai] = e;
            ExprPtr notai = Expression::mkNot(ai);
            notai->setWeight(e->getWeight());
            WF->assertSoft(notai); 
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            WF->assertHard(ea);
        }
    }
    if (AV.empty()) {
        std::cout << "No solution!\n";
        delete WF;
        return;
    }
    
    //
    // Compute the root causes
    //    
    unsigned nbDetect = 0;
    USVVec MCSes;
    std::vector<ExprPtr>::const_iterator ite;
    for (unsigned i=0; i<TCExprs.size(); i++) 
    {
        // Assert the test case formula (pre-cond) as hard
        WF->assertHard(TCExprs[i]);
        // Assert the golden ouput formula (not post-cond) as hard
        WF->assertHard(GOExprs[i]);
        //
        // Run the AllMinMCS algorithm
        //
        UVVec M;
        M = allMinMCS(solver, WF, AV, AVMap);
        // Found some MCS!
        if (!M.empty()) 
        {
            nbDetect++;
            USVec Mset;
            UVVec::const_iterator itv;
            copy(M, Mset);
            removeDoublons(Mset);
            TCExprs[i]->dump();
            std::cout << "\n" << Mset << "\n" << std::endl;
            MCSes.push_back(Mset);
        }
        // Backtrack to the point where there 
        // was no Pre/Post-conditions in the formula
        WF->remove(TCExprs[i]);
        WF->remove(GOExprs[i]);
    }
    if (options->printDuration()) {
        timer1.stop("\nRunTCAS_PWU Time (no PWU)");
    }
    
    // Clean the MUSes
    for (size_t i=0; i<MCSes.size(); ++i) {
        USVec M = MCSes[i];
        removeDoublons(M);
        removeSubsets(M);
    }
    // Remove doublons
    sort( MCSes.begin(), MCSes.end() );
    MCSes.erase( unique( MCSes.begin(), MCSes.end() ), MCSes.end() );
    
    // Pair-wise union of MCSes to obtain the complete diagnosis
    USVec CombinedMCSes;
    pairwiseUnion(MCSes, CombinedMCSes);
    removeDoublons(CombinedMCSes);
    removeSubsets(CombinedMCSes);
    
    // Print the result
    std::cout << "#Detect/#TC: " 
    << nbDetect << "/" << TCExprs.size() << std::endl;
    if (CombinedMCSes.size()>0) {
        std::cout << "\nCombined MCSes size: " << CombinedMCSes.size() << "\n";
        std::cout << "Combined MCSes:\n";
        std::cout << CombinedMCSes << std::endl;  
    } else {
        std::cout << "SNIPER was unable to localize any root causes.\n";
    }
    //std::cout << "ACSR:        " 
    //<< getCodeSizeReduction(MUSes, 173) << "%" << std::endl;
    delete WF;
    if (options->printDuration()) {
        timer2.stop("\nRunTCAS_PWU Time");
    }
}


// =============================================================================
// runTCAS (without push-pop optimization)
// =============================================================================
void IterationAlgorithm::runTCAS(std::vector<ExprPtr> TCExprs,
                                 std::vector<ExprPtr> GOExprs) {
    //
    // Some preparation before running 
    // the AllMInMCS algorithm
    //
    Formula *WF = new Formula(formula); // Working formula
    // Auxiliary variables
    std::vector<BoolVarExprPtr> AV;
    std::map<BoolVarExprPtr, ExprPtr> AVMap;
    // for each E in WF, E tagged as soft do
    std::vector<ExprPtr> clauses = WF->getExprs();
    unsigned i = 0;
    std::vector<ExprPtr>::iterator it;
    for(it=clauses.begin(); it!=clauses.end(); it++) {
        ExprPtr e = *it;
        if (e->isSoft()) {
            // AI is a new auxiliary var. created
            std::ostringstream oss;
            oss << i++;
            BoolVarExprPtr ai = Expression::mkBoolVar("a_"+oss.str());
            AV.push_back(ai);
            AVMap[ai] = e;
            ExprPtr notai = Expression::mkNot(ai);
            notai->setWeight(e->getWeight());
            WF->assertSoft(notai); 
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            WF->assertHard(ea);
        }
    }
    if (AV.empty()) {
        std::cout << "No solution!\n";
        delete WF;
        return;
    }
    
    //
    // Compute the root causes
    //    
    unsigned nbDetect = 0;
    std::vector<std::set<unsigned> > MUSes;
    std::vector<ExprPtr>::const_iterator ite;
    for (unsigned i=0; i<TCExprs.size(); i++) 
    {
        // Assert the test case formula (pre-cond) as hard
        WF->assertHard(TCExprs[i]);
        // Assert the golden ouput formula (not post-cond) as hard
        WF->assertHard(GOExprs[i]);
        //
        // Run the AllMinMCS algorithm
        //
        UVVec M;
        M = allMinMCS(solver, WF, AV, AVMap);
        // Found some MCS!
        if (!M.empty()) 
        {
            nbDetect++;
            USVec Mset;
            copy(M, Mset);
            removeDoublons(Mset);
            // Merge the MUSes together
            USVec MUS;
            HittingSet::getMinimalHittingSets_LP(Mset, MUS);
            MUSes.insert(MUSes.end(), MUS.begin(), MUS.end()); 
        }
        // Backtrack to the point where there 
        // was no Pre/Post-conditions in the formula
        WF->remove(TCExprs[i]);
        WF->remove(GOExprs[i]);
    }
    // Clean the MUSes
    removeDoublons(MUSes);
    removeSubsets(MUSes);
    // Minimal hitting the MUSes (conflicts) to obtain the diagnoses
    USVec Diag;
    HittingSet::getMinimalHittingSets_LP(MUSes, Diag);
    
    // Print the result
    std::cout << "#Detect/#TC: " 
    << nbDetect << "/" << TCExprs.size() << std::endl;
    std::cout << "ACSR:        "
    << getCodeSizeReduction(MUSes, 173) << "%" << std::endl;
    std::cout << "MCSes size: " << Diag.size() << "\n";
    std::cout << "MCSes:\n";
    std::cout << Diag << std::endl;
 }


// Input:  a weighted formula in CNF
// Output: a set of minimal MCS
UVVec IterationAlgorithm::allMinMCS_AR(YicesSolver *yices,
                                       std::vector<BoolVarExprPtr> &AV,
                                       std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    UVVec MCSes;
    std::vector<ExprPtr> blockFormulas;
    std::vector<assertion_id> idsBF;
    bool done = false;
    while (!done) {
        switch(yices->maxSat()) {
            case l_true: { 
                // U is a Minimal Correction Subset 
                // (MCS) if st is true
                std::vector<ExprPtr> U = solver->getUnsatExpressions();
                if (U.empty()) { // SAT
                    done = true;
                    break; 
                }
                UVec M;    
                // Add the blocking constraint
                std::vector<ExprPtr> toadd;
                std::vector<BoolVarExprPtr>::iterator it2;
                for (it2=AV.begin(); it2!=AV.end(); it2++) {
                    BoolVarExprPtr ai = *it2;
                    int val = solver->getBoolValue(ai->getName());
                    if (val==l_true) {
                        toadd.push_back(Expression::mkNot(ai));
                        M.push_back(AVMap[ai]->getLine());
                    }
                }
                if (!toadd.empty()) {
                    MCSes.push_back(M);
                    ExprPtr blockFormula = Expression::mkOr(toadd);
                    blockFormula->setHard();
                    assertion_id i = yices->addToContextRetractable(blockFormula);
                    // Save it to remove it later
                    blockFormulas.push_back(blockFormula);
                    idsBF.push_back(i); 
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
    // Retract the block formulas
    std::vector<assertion_id>::iterator it;
    for (it=idsBF.begin(); it!=idsBF.end(); ++it) {
        assertion_id i = *it;
        yices->retractFromContext(i);
    }
    while (!blockFormulas.empty()) {
        ExprPtr e = blockFormulas.back();
        blockFormulas.pop_back();
        //delete e;
    }  
    return MCSes;
}

// Input:  a weighted formula in CNF
// Output: a set of minimal MCS
UVVec IterationAlgorithm::allMinMCS_PP(YicesSolver *yices,
                                       std::vector<BoolVarExprPtr> &AV,
                                       std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    UVVec MCSes;
    std::vector<ExprPtr> blockFormulas;
    bool done = false;
    while (!done) {
        switch(yices->maxSat()) {
            case l_true: { 
                // U is a Minimal Correction Subset 
                // (MCS) if st is true
                std::vector<ExprPtr> U = solver->getUnsatExpressions();
                if (U.empty()) { // SAT
                    done = true;
                    break; 
                }
                UVec M;    
                // Add the blocking constraint
                std::vector<ExprPtr> toadd;
                std::vector<BoolVarExprPtr>::iterator it2;
                for (it2=AV.begin(); it2!=AV.end(); it2++) {
                    BoolVarExprPtr ai = *it2;
                    int val = solver->getBoolValue(ai->getName());
                    if (val==l_true) {
                        toadd.push_back(Expression::mkNot(ai));
                        //M.push_back(AVMap[ai]->getLine());
                        M.push_back(ai->getLine());
                    }
                }
                if (!toadd.empty()) {
                    MCSes.push_back(M);
                    ExprPtr blockFormula = Expression::mkOr(toadd);
                    blockFormula->setHard();
                    yices->addToContext(blockFormula);
                    // Save it to remove it later
                    blockFormulas.push_back(blockFormula); 
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
    while (!blockFormulas.empty()) {
        ExprPtr e = blockFormulas.back();
        blockFormulas.pop_back();
        //delete e;
    }    
    return MCSes;
}


// Input:  a weighted formula in CNF
// Output: a set of minimal MCS
UVVec IterationAlgorithm::allMinMCS(IWPMaxSATSolver *solver, Formula *WF,
                                    std::vector<BoolVarExprPtr> &AV,
                                    std::map<BoolVarExprPtr, ExprPtr> &AVMap) {
    
    UVVec MCSes;
    std::vector<ExprPtr> blockFormulas;
    bool done = false;
    while (!done) {
        switch(solver->maxSat(WF)) {
            case l_true: { 
                // U is a Minimal Correction Subset 
                // (MCS) if st is true
                std::vector<ExprPtr> U = solver->getUnsatExpressions();
                if (U.empty()) { // SAT
                    done = true;
                    break; 
                }
                UVec M;    
                // Add the blocking constraint
                std::vector<ExprPtr> toadd;
                std::vector<BoolVarExprPtr>::iterator it2;
                for (it2=AV.begin(); it2!=AV.end(); it2++) {
                    BoolVarExprPtr ai = *it2;
                    int val = solver->getBoolValue(ai->getName());
                    if (val==l_true) {
                        toadd.push_back(Expression::mkNot(ai));
                        M.push_back(AVMap[ai]->getLine());                        
                    }
                }
                if (!toadd.empty()) {
                    MCSes.push_back(M);
                    ExprPtr blockFormula = Expression::mkOr(toadd);
                    WF->assertHard(blockFormula);
                    // Save it to remove it later
                    blockFormulas.push_back(blockFormula); 
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
    while (!blockFormulas.empty()) {
        ExprPtr e = blockFormulas.back();
        blockFormulas.pop_back();
        WF->remove(e);
        //delete e;
    } 
    return MCSes;
}


// [[[int]]] -> [[int]]
// [[mcs]]   -> [[mcs]]
void IterationAlgorithm::pairwiseUnion(USVVec MCSes, USVec &Diag) {
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

double IterationAlgorithm::getCodeSizeReduction(USVec MCSes, unsigned totalNbLine) {

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
void IterationAlgorithm::copy(UVVec &vv, USVec &sv) {
    UVVec::const_iterator itv;
    for (itv=vv.begin(); itv!=vv.end(); ++itv) {
        // Copy the MCS (vector) into a MCS (set)
        USet subset((*itv).begin(), (*itv).end());
        sv.push_back(subset);
    }
}

// Remove all subset doublons
// {{x,y}, {a,b}, {x,y}} -> {{a,b}}
void IterationAlgorithm::removeDoublons(USVec &v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());   
}

// Remove all subsets of subsets
// {{x}, {a,b}, {x,y}} -> {{a,b},{x,y}}
void IterationAlgorithm::removeSubsets(USVec &v) {
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
}