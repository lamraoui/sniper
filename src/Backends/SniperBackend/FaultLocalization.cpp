/**
 * \file FaultLocalization.cpp
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 * \author Si-Mohamed Lamraoui
 * \date   7 March 2016
 */

#include "FaultLocalization.h"

unsigned nbCallsToSolver = 0;

void FaultLocalization::run(Formula *TF, Formula *preCond, Formula *postCond,
                             ProgramProfile *prof,
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
    failingTraces = prof->getFailingProgramTraces();
    
    // Add the pre-conditions to the context (as hard)
    for (ExprPtr e : preCond->getExprs()) {
        e->setHard();
        TF->add(e);
    }
    // Add the post-conditions to the context (as hard)
    for (ExprPtr e : postCond->getExprs()) {
        e->setHard();
        TF->add(e);
    }
    
    if (options->verbose()) {
        std::cout << "=================================================\n";
        std::cout << "Running AllDiagnosis algorithm ";
        if (combineMethod==Combine::MHS)      std::cout << "[MHS]";
        else if (combineMethod==Combine::PWU) std::cout << "[PWU]";
        else if (combineMethod==Combine::FLA) std::cout << "[FLA]";
        else std::cout << "[NOCOMB]";
        if (options->htfUsed()) std::cout << "[HFTF]";
        else                    std::cout << "[FFTF]";
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
    
    if (options->dbgMsg()) {
        std::cout << "Nb calls solver  : " << nbCallsToSolver << std::endl;
    }
    
    // Combination methods: PWU, MHS, FLA
    SetOfFormulasPtr combMCSes = NULL;
    switch (combineMethod) {
        case Combine::MHS:
            // Minimal-hitting set
            combMCSes = Combine::combineByMHS(MCSes, options->getNbLOC());
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
    if (combMCSes && !combMCSes->empty()) {
        if (options->verbose()) {
            std::cout << "\nCombined MCSes size: ";
            std::cout << combMCSes->size() << std::endl;
            std::cout << "Combined MCSes:\n";
            std::cout << combMCSes << std::endl;
        }
    } else {
        if (combineMethod==Combine::NONE && !MCSes.empty()) {
            if (options->verbose()) {
                std::cout << "\nMCSes size: ";
                std::cout << MCSes.size() << std::endl;
                std::cout << "MCSes (not combined):\n";
                std::cout << "{";
                std::vector<SetOfFormulasPtr>::iterator it;
                for (it=MCSes.begin(); it!=MCSes.end(); ++it) {
                    SetOfFormulasPtr sof = *it;
                    std::cout << sof << std::endl;
                    if (std::distance(it, MCSes.end())>1) {
                        std::cout << ", ";
                    }
                }
                std::cout << "}\n";
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

std::vector<SetOfFormulasPtr>
FaultLocalization::allDiagnosis(Formula *TF,
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
            notai->setSoft();
            WF->add(notai);
            // Remove E and add EA as hard
            ExprPtr ea = Expression::mkOr(e, ai);
            WF->remove(e);
            ea->setHard();
            WF->add(ea);
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
        // Assert as hard the golden output (if any)
        // (= return_var golden_output)
        Value *goldenOutput = E->getExpectedOutput();
        if (goldenOutput) {
            BasicBlock *lastBB = &targetFun->back();
            Instruction *lastInst = &lastBB->back();
            if (ReturnInst *ret= dyn_cast<ReturnInst>(lastInst)) {
                Value *retVal = ret->getReturnValue();
                if (retVal) {
                    ExprPtr retExpr = Expression::getExprFromValue(retVal);
                    ExprPtr goExpr = Expression::getExprFromValue(goldenOutput);
                    ExprPtr eqExpr = Expression::mkEq(retExpr, goExpr);
                    eqExpr->setHard();
                    yices->addToContext(eqExpr);
                    if (options->dbgMsg()) {
                        std::cout << "-- Golden ouput: ";
                        eqExpr->dump();
                        std::cout << std::endl;
                    }
                }
            }
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

SetOfFormulasPtr 
FaultLocalization::allMinMCS(YicesSolver *yices,
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

SetOfFormulasPtr
FaultLocalization::avToClauses(SetOfFormulasPtr M,
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

void FaultLocalization::checkControlFlow(YicesSolver *solver) {
    std::cout << "Checking control flow...";
    assert(solver && "Solver is null!");
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
        assert((nbTruePredTrans==0 || nbTruePredTrans==1) 
                && "Invalid control flow value!");
        if (nbPredTrans==0) { // Entry block doesn't have any predecessors
            nbTruePredTrans = 1;
        }
        const TerminatorInst *t = bb->getTerminator();
        if (isa<ReturnInst>(t)) {
            return; // End of recursion
        }
        const BranchInst *br = dyn_cast<BranchInst>(t);
        assert(br && "Unsupported terminal instruction!");
        if (br->isUnconditional()) {
            BasicBlock *nextbb = br->getSuccessor(0);
            int val = getBlockTransVal(solver, bb, nextbb);
            if (nbTruePredTrans==1 && val==l_false) {
                dumpTransValues(solver);
                assert("invalid control flow value (1)");
            }
            if (nbTruePredTrans==0 && nbUndefPredTrans==0 && val==l_true) {
                std::cout << bb->getName().str() << std::endl;
                dumpTransValues(solver);
                assert("invalid control flow value (2)");
            }
        } else {
            BasicBlock *nextbb1 = br->getSuccessor(0);
            BasicBlock *nextbb2 = br->getSuccessor(1);
            int val1 = getBlockTransVal(solver, bb, nextbb1);
            int val2 = getBlockTransVal(solver, bb, nextbb2);
            if (nbTruePredTrans==1 && val1==l_false && val2==l_false) {
                dumpTransValues(solver);
                assert("invalid control flow value (3)");
            }
            if (nbTruePredTrans==0 && nbUndefPredTrans ==0
                && (val1==l_true || val2==l_true)) {
                dumpTransValues(solver);
                assert("invalid control flow value (4)");
            }
            if (val1==l_true && val2==l_true) {
                dumpTransValues(solver);
                assert("invalid control flow value (5)");
            }
        }
    }
    std::cout << "OK\n";
}

int FaultLocalization::getBlockTransVal(YicesSolver *s,
                                         BasicBlock *bb1, BasicBlock *bb2) {
    assert(s && "Solver is null!");
    assert((bb1 && bb2) && "Basic block is null!");
    std::string bbname1 = bb1->getName().str();
    std::string bbname2 = bb2->getName().str();
    std::string t = bbname1 + "_" + bbname2;
    int val = solver->getBoolValue(t);
    return val;
}

void FaultLocalization::dumpTransValues(YicesSolver *solver) {
    std::cout <<  "\n---------------------" << std::endl;
    for (Function::iterator i=targetFun->begin(), e=targetFun->end(); i!=e; ++i) {
        BasicBlock *bb = i;
        const TerminatorInst *t = bb->getTerminator();
        if (isa<ReturnInst>(t)) {
            continue; // End
        }
        const BranchInst *br = dyn_cast<BranchInst>(t);
        assert(br && "Unsupported terminal instruction!");
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
