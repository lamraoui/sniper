/**
 * \file BMC.h
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
 * \date   12 January 2015
 */

#ifndef _BMC_H
#define _BMC_H

#include <vector>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"

#include "Options.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Profile/ProgramTrace.h"
#include "Logic/Expression.h"
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"

using namespace llvm;

/**
 * \class BMC
 *
 * \brief Bounded Model checking.
 *
 * This warper class provides a function to 
 * perform bounded model checking (%BMC).
 */
class BMC {

public:
    
    /**
     * \brief Run bounded model checking (%BMC) on \a targetFun.
     *
     * This method can generate a single error-inducing input (failing trace).
     * BMC with Boolean satisfiability is performed by the satisfiability 
     * checking of the formula φbmc = not(PRE ^ TF -> AS) = PRE ^ TF ^ not(AS).
     * If φbmc is satisfiable, the corresponding assignment represents a 
     * counterexample that illustrates a faulty program execution. 
     * We extract from the counterexample the values for the input arguments 
     * of the procedure that result in the failing execution, namely the 
     * error-inducing input values. Let EI be a formula expressing that those 
     * arguments take such values, which constitutes the pre-state of the 
     * program that will violate the post-condition.
     *
     * \param profile A program profile to save the error-inducing input 
     * if bounded model checking was successful.
     * \param targetFun An LLVM function on which we want to perform bounded
     * model checking.
     * \param solver An SMT solver to check the unsatisfiability of the 
     * formula (\a TF ^ not(\a AS)).
     * \param TF A trace formula that encodes \a targetFun.
     * \param preCond A formula that encodes the pre-condtion of \a targetFun.
     * \param postCond A formula that encodes the post-condtion of \a targetFun.
     * \param loopInfo Information about loops in \a targetFun.
     * \param options User defined SNIPER options.
     */
    static void run(ProgramProfile *profile, Function *targetFun,
                    YicesSolver *solver, Formula *TF, Formula *preCond,
                    Formula *postCond, LoopInfoPass *loopInfo, Options *options);
    
};

#endif // _BMC_H
