/**
 * \file ProgramProfile.h
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
 * \date   18 January 2015
 */

#ifndef _PROGRAMPROFILE_H
#define _PROGRAMPROFILE_H

#include <set>
#include <vector>
#include <iostream>
#include <memory>

#include "llvm/DebugInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "Options.h"
#include "Profile/ProgramTrace.h"

using namespace llvm;

class NLOperationSummary;
class FunctionSummary;

/**
 * \class ProgramProfile
 *
 * This class holds program execution traces.
 */
class ProgramProfile {

private:
    /**
     * LLVM function from which the program execution traces were obtained.
     */
    Function *targetFun;
    std::vector<ProgramTrace*> traces;
    /**
     * Bug free blocks are LLVM basicblocks that are assumed to be free 
     * from faults because they are never executed in failing execution.
     * This set is used in the construction of the Hardened Flow-sensitive
     * Trace Formula (HFTF).
     * This set is only accessible when bugFreeBlocksComputed is true.
     */
    std::set<BasicBlock*> bugFreeBlocks;
    bool bugFreeBlocksComputed;
    
public:
    /**
     * Default constructor.
     *
     * \param _targetFun An LLVM target function.
     */
    ProgramProfile(Function *_targetFun) 
    : targetFun(_targetFun), bugFreeBlocksComputed(false) {  } 
    /**
     * Destructor.
     */ 
    ~ProgramProfile() { }
    
    /**
     * Add a program execution trace.
     */
    void addProgramTrace(ProgramTrace *e);
    /**
     * Return true if this profile has failing program execution traces, 
     * false otherwise.
     */
    bool hasFailingProgramTraces();
    /**
     * Return all program execution traces.
     */
    std::vector<ProgramTrace*> getProgramTraces();
    /**
     * Return failing program execution traces.
     */
    std::vector<ProgramTrace*> getFailingProgramTraces();
    /**
     * Return successful program execution traces.
     */
    std::vector<ProgramTrace*> getSuccessfulProgramTraces();
    /**
     * Return program execution traces that are neither failing or successful.
     */
    std::vector<ProgramTrace*> getUnkownProgramTraces();
    /**
     * Load testcases inputs and golden outputs.
     *
     * \param inputsFilename 
     * \param outputsFilename
     * \return \a true if the testcases and golden were successfully 
     * loaded, \a false otherwise.
     */
    bool loadTestcases(std::string inputsFilename,
                       std::string outputsFilename, Options *o);
    
    // ==== For constructing HFTF ====
    
    /**
     * Add a set of LLVM basicblocks that were executed in a failing execution.
     */
    void addFailingBlocks(std::vector<BasicBlock*> &bb);
    /**
     * Add a set of LLVM basicblocks that were executed in a successful execution.
     */
    void addSuccessfulBlocks(std::vector<BasicBlock*> &bb);
    /**
     * Add a set of LLVM basicblocks that were executed in neither 
     * a failing or successful execution.
     */
    void addUnknowBlocks(std::vector<BasicBlock*> &bb);
    /**
     * Build the set of bug free LLVM basicblocks.
     *
     * Bug free blocks are LLVM basicblocks that are assumed to be free
     * from faults because they are never executed in failing execution.
     * Bug free blocks are used in the construction of the Hardened 
     * Flow-sensitive Trace Formula (HFTF).
     */
    void computeBugFreeBlocks(Options *o);
    /**
     * Return true if the LLVM basicblock \a bb was never executed
     * in a failing execution.
     */
    bool isBugFreeBlock(BasicBlock *bb);
    /**
     * Return LLVM basic blocks that were executed in 
     * failing program executions.
     */
    std::set<BasicBlock*> getFailingRunBB();
    /**
     * Return LLVM basic blocks that were executed in
     * successful program executions.
     */
    std::set<BasicBlock*> getSuccessfulRunBB();
    /**
     * Return LLVM basic blocks that were neither executed in
     * failing or successful program executions.
     */
    std::set<BasicBlock*> getOtherRunBB();
    /**
     * Return LLVM basic blocks that were never executed in
     * failing program executions.
     */
    std::set<BasicBlock*> getBugFreeBlocks();
    /**
     * Dump to the standard output a textual representation of 
     * this program profile.
     */
    void dump(); 
    
private:
    /**
     * Add a successful program execution trace.
     */
    void addSuccProgramTrace(ProgramTrace *e);
    /**
     * Add a failing program execution trace.
     */
    void addFailProgramTrace(ProgramTrace *e);
    /**
     * Add a program execution trace that is neither failing or successful.
     */
    void addOtherProgramTrace(ProgramTrace *e);
    
};

#endif // _PROGRAMPROFILE_H
