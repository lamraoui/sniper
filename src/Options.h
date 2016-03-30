/**
 * \file Options.h
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
 * \date   25 January 2015
 */

#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <string>
#include <iostream>

#include "llvm/Support/CommandLine.h"

#include "Logic/Combine.h"

using namespace llvm;

/**
 * \class Options
 *
 * User defined options.
 */
class Options {

public:
    /**
     * Default constructor.
     *
     * \param argc Argc variable from the main function.
     * \param argv Argv variable from the main function.
     */
    Options(int argc, char **argv);
    /**
     * Destructor.
     */
    ~Options();
    
    /**
     * Return the filename of the input intermediate 
     * representation (IR, LLVM bytecode).
     */
    std::string getInputIRFilename();
    /**
     * Return the filename of the input program source file.
     */
    std::string getInputCSourceFilename();
    /**
     * Return the function's name the user wants to analyse.
     */
    std::string getFunctionName();
    /**
     * Return the filename of the test suite file.
     */
    std::string getTestSuiteFileName();
    /**
     * Return the filename of the golden outputs file.
     */
    std::string getGoldenOutputsFileName();
    /**
     * Return \a true if debug messages are diplayed, false otherwise.
     */
    bool dbgMsg();
    /**
     * Return \a true if tool messages are diplayed, false otherwise.
     */
    bool verbose();
    /**
     * Return \a true if times are diplayed, false otherwise.
     */
    bool printDuration();
    /**
     * Return \a true if the target LLVM module is diplayed, false otherwise.
     */
    bool printModIR();
    /**
     * Return \a true if the target LLVM function diplayed, false otherwise.
     */
    bool printFunIR();
    /**
     * Return \a true if the trace formula is diplayed, false otherwise.
     */
    bool printTF();
    /**
     * Return \a true if conflicts (MUSes) are computed and diplayed, 
     * false otherwise.
     */
    bool printMUS();
    /**
     * Return \a true if diagnoses (MCSes) are diplayed, 
     * false otherwise.
     */
    bool printMCS();
    /**
     * Return \a true if the granularity level is set to "instruction", 
     * false otherwise.
     *
     * The granularity level is related to the encoding of trace formulas.
     * For the case of "instruction" level, it means that each clause in 
     * the trace formula refers to a single LLVM instruction only.
     */
    bool instructionGranularityLevel();
    /**
     * Return \a true if the granularity level is set to "line",
     * false otherwise.
     *
     * The granularity level is related to the encoding of trace formulas.
     * For the case of "line" level, it means that each clause in
     * the trace formula refers to a complete line of code 
     * (set of LLVM instructions).
     */
    bool lineGranularityLevel();
    /**
     * Return \a true if the granularity level is set to "block",
     * false otherwise.
     *
     * The granularity level is related to the encoding of trace formulas.
     * For the case of "block" level, it means that each clause in
     * the trace formula refers to a complete LLVM basicblock 
     * (set of LLVM instructions).
     */
    bool blockGranularityLevel();
    /**
     * Return \a true if the test case generation method used is 
     * bounded model checking (BMC), false otherwise.
     */
    bool methodBMC();
    /**
     * Return \a true if the test case generation method used is 
     * Concolic execution.
     */
    bool methodConcolic();
    /**
     * Return the number of times each loop has to be unrolled.
     */
    unsigned getUnrollCount();
    /**
     * Return the maximum program depth (used in Concolic execution).
     */
    unsigned getMaxDepth();
    /**
     * Return \a true if the trace formula encoding hardened flow-sensitive 
     * trace formula (HFTF) is used, false if another encoding is used.
     */
    bool htfUsed();
    /**
     * Return the total number of line of code in the target source code.
     */
    unsigned getNbLOC();
    /**
     * Return \a true if the model of the control flow graph (CFG)
     * has to be check, false if not.
     */
    bool checkCFGModel();
    /**
     * Return the maximum size diagnoses (MCSes) should not be bigger than.
     */
    unsigned mcsMaxSize();
    /**
     * Return \a true if a dot file representating the target program 
     * has to be generated and output to the user.
     */
    bool outputCFGDotFile();
    /**
     * Return the combination method (FLA, PWU, MHS) to be used.
     */
    unsigned getCombineMethod();
    
private:
    /**
     * Hide unwanted LLVM options.
     */
    void hideOptions();
    
};

#endif // _OPTIONS_H
