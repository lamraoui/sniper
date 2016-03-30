/**
 * \file IRRunner.h
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
 * \date   28 February 2016
 */

#ifndef _IRRUNNER_H
#define _IRRUNNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>

#include "ConcolicModule.h"
#include "Executor.h"

using namespace llvm;

/**
 * \class IRRunner
 *
 * This class is used in case the user has already 
 * a set of test cases available. 
 * This class implements a module to parse the test cases 
 * in the format used by SNIPER. This module uses the same 
 * implementation of the ConcolicProfiler but with the 
 * symbolic execution disabled. The module executes the 
 * test cases on the input program and checks whether or 
 * not the obtained output satisfies the post-condition 
 * in order to know if the test case is a failing or 
 * successful one. As in the ConcolicProfiler, this module 
 * also benefits from the JIT capabilities of LLVM.
 */
class IRRunner : public ConcolicModule {
       
private:
    Function *targetFun;
    ExecutionEngine *EE;
    std::vector<std::vector<Value*> > inputValuesVec;
    std::vector<Value*> outputValuesVec;
    
public:
    /**
     * Default constructor.
     *
     * \param _llvmMod An LLVM module in which the target function is.
     * \param _targetFun An LLVM target function. 
     * \param _options SNIPER options.
     * \param tsFilename Test suite (test inputs) filename.
     * \param goFilename Golden output (oracle) filename (not used by default).
     */
    IRRunner(Module *_llvmMod, Function *_targetFun, Options *_options,
             const std::string &tsFilename, 
             /* optional */ const std::string &goFilename = std::string())
            : ConcolicModule(_llvmMod, _targetFun, _options) {
        this->targetFun = _targetFun;
        this->inputValuesVec = parseTestsuiteFile(tsFilename);
        if (!goFilename.empty()) {
            this->outputValuesVec = parseGoldenOutputsFile(goFilename);
        }
    }
    /**
     * Destructor.
     */
    ~IRRunner() {
      //delete EE;
    }

    /**
     * Execute the target function on all test cases and 
     * collect execution information, such as executed basic block 
     * or program ouputs.
     *
     * \param profile A program profile to store program information.
     * \param locVars Information on local variables in targetFun.
     * \param loopInfo Information on loops in targetFun.
     */
    void run(ProgramProfile *profile, LocalVariables *locVars, LoopInfoPass *loopInfo);
    /**
     * Parse a file of test cases into the SNIPER 
     * internal representation (LLVM values). 
     * A file of test cases consists of one test case per line, 
     * with each test case of the form: value_1 value_2 ... value_n.
     *
     * \param filename A file of test cases to be parsed.
     * \return A set of sets of LLVM concrete values. 
     */
    std::vector<std::vector<Value*> > parseTestsuiteFile(const std::string &filename);
    /**
     * Parse a file of golden outputs into the SNIPER 
     * internal representation (LLVM values). 
     * A golden output is the expected value being output by program. 
     * A file of golden output consists of one value per line. 
     *
     * \param filename A file of golden outputs to be parsed.
     * \return A set of LLVM concrete values. 
     */
    std::vector<Value*> parseGoldenOutputsFile(const std::string &filename);

};

#endif // _IRRUNNER_H
