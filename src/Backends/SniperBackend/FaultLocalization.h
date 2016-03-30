/**
 * \file FaultLocalization.h
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
 * This class implements various algorithms to do automatic 
 * fault localization of imperative programs with multiple faults.
 */
class FaultLocalization {
    
private:
    Function *targetFun;
    YicesSolver *solver;
    bool hasArgv;
    Options *options;
    
public:
    /**
     * Default constructor.
     */
    FaultLocalization(Function *_targetFun, YicesSolver *_solver,
                       Options *_options) :
                       targetFun(_targetFun), solver(_solver),
                       options(_options) { }
    /**
     * Destructor.
     */
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
     * for each error-inducing input. 
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
     * \param AV A set of auxiliary variables.
     * \param AVMap A map between auxiliary variables and their associated expressions.
     * \return a set of minimal MCSes.
     */
    SetOfFormulasPtr allMinMCS(YicesSolver *yices,
                               std::vector<BoolVarExprPtr> &AV,
                               std::map<BoolVarExprPtr, ExprPtr> &AVMap);

private:
    /**
     * Retreive in \p AVMap the expressions for which their 
     * associated auxiliary variables are negated in \p M. 
     *
     * \param M A set of auxiliary variables (negated or not).
     * \param AVMap A map between auxiliary variables and their associated expressions.
     * \return A set of expressions (from the trace formula).
     */
    SetOfFormulasPtr avToClauses(SetOfFormulasPtr M,
                                 std::map<BoolVarExprPtr, ExprPtr> AVMap);
    
    // === For debugging ===

    /**
     * Check the given model for control flow conflicts.
     *
     * \param solver A max-sat solver that contains a model.
     */
    void checkControlFlow(YicesSolver *solver);
    /**
     * Return the value of the transition variable <\p bb1,\p bb2> in 
     * the given model.
     *
     * \param s A max-sat solver that contains a model.
     * \param bb1 An LLVM basic block.
     * \param bb2 An LLVM basic block.
     * \return The value of the <\p bb1,\p bb2> in the model of \p s.
     */
    int  getBlockTransVal(YicesSolver *s, BasicBlock *bb1, BasicBlock *bb2);
    /**
     * Given a model, print all basic block transitions values.
     *
     * \param s A max-sat solver that contains a model.
     */
    void dumpTransValues(YicesSolver *solver);
    
};

#endif // _FAULTLOCALIZATION_H
