/**
 * \file FaultLocalization.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   8 March 2016
 */

#ifndef _FAULTLOCALIZATION_H
#define _FAULTLOCALIZATION_H

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
//#include <dispatch/dispatch.h>

#include "Options.h"
#include "Utils/Utils.h"
#include "Utils/HittingSet.h"
#include "Profile/ProgramTrace.h"
#include "Profile/ProgramProfile.h"
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"
#include "Logic/Combine.h"

using namespace llvm;

/**
 * \class FaultLocalization
 *
 * This class implements various algorithm to do automatic 
 * fault localization of imperative programs with multiple faults.
 */
class FaultLocalization {
    
private:
    Function *targetFun;
    YicesSolver *solver;
    bool hasArgv;
    Options *options;
    
public:
    FaultLocalization(Function *_targetFun, YicesSolver *_solver,
                       Options *_options) :
                       targetFun(_targetFun), solver(_solver),
                       options(_options) { }
    ~FaultLocalization() { }
    
    /**
     * Run the diagnosis enumeration algorithm, 
     * combine the generated diagnoses, and print them.
     *
     * \param TF A trace formula (partial formula in CNF).
     * \param preCond A formula that encodes the pre-conditions (with hard clauses and in CNF).
     * \param postCond A formula that encodes the post-conditions (with hard clauses and in CNF).
     * \param prof A profile that contains program traces (program inputs, etc).
     * \param combineMethod Desired combination method.
     */
    void run(Formula *TF, Formula *preCond, Formula *postCond,
             ProgramProfile *prof, Combine::Method combineMethod);
    
    /**
     * Enumerate diagnoses for the given trace formula and 
     * for each failing inputs. 
     *
     * \param TF A trace formula (partial formula in CNF).
     * \param traces A set of program traces that contain error-inducing inputs.
     * \param yices A partial max-sat solver.
     */
    std::vector<SetOfFormulasPtr> allDiagnosis(Formula *TF,
                                               std::vector<ProgramTrace*> traces,
                                               YicesSolver *yices);
    /**
     * Enumerate ALL diagnoses (MCSes) for the given formula.
     * The diagnoses enumeration consists of enumerating all the 
     * MCSes (minimal correction subsets) of the input program 
     * formula for each failing test input (error-inducing input). 
     *
     * Definition: Given a constraint system C, the AllMinMCS problem 
     * consists of finding all the minimum size MCSes of C. The AllMCS 
     * problem consists of finding all MCSes of C (independent of their size).
     *
     * \param yices A partial max-sat solver.
     * \param AV 
     * \param AVMap 
     * \return a set of minimal MCSes.
     */
    SetOfFormulasPtr allMinMCS(YicesSolver *yices,
                               std::vector<BoolVarExprPtr> &AV,
                               std::map<BoolVarExprPtr, ExprPtr> &AVMap);

private:
    /**
     * From the negated auxiliary variables (not a_i) in M, 
     * retreive the corresponding expressions in AVMap and 
     * save them in M2.
     */
    SetOfFormulasPtr avToClauses(SetOfFormulasPtr M,
                                 std::map<BoolVarExprPtr, ExprPtr> AVMap);
    
    // === For debugging ===

    /**
     * Check the given model for control flow conflicts.
     */
    void checkControlFlow(YicesSolver *solver);
    /**
     * Return the value of the transition <bb1,bb2> in 
     * the given model.
     */
    int  getBlockTransVal(YicesSolver *s, BasicBlock *bb1, BasicBlock *bb2);
    /**
     * Given a model, print all basic block transitions values.
     */
    void dumpTransValues(YicesSolver *solver);
    
};

#endif // _FAULTLOCALIZATION_H
