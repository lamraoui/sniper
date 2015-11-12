/**
 * Options.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/01
 * @copyright : NII 2013
 */


#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <string>
#include <iostream>
#include "llvm/Support/CommandLine.h"

using namespace llvm;

//============================================================================
class Options {

public:
    Options(int argc, char **argv);
    ~Options();
    
    std::string getInputIRFilename();
    std::string getInputCSourceFilename();
    std::string getFunctionName();
    std::string getTestSuiteFileName();
    std::string getGoldenOutputsFileName();
    bool        dbgMsg();
    bool        verbose();
    bool        printDuration();
    bool        printModIR();
    bool        printFunIR();
    bool        printTF();
    bool        printMUS();
    bool        printMCS();
    bool        instructionGranularityLevel();
    bool        lineGranularityLevel();
    bool        blockGranularityLevel();
    bool        methodBMC();
    bool        methodPathExploration();
    bool        methodConcolic();
    unsigned    getUnrollCount();
    unsigned    getMaxDepth();
    bool        htfUsed();
    unsigned    getMaxFailTraces();
    unsigned    getMaxSuccTraces();
    bool        withoutWeights();
    unsigned    getNbLOC();
    bool        checkCFGModel();
    bool        useDynamicDiagnosesEnum();
    unsigned    mcsMaxSize();
    bool        outputCFGDotFile();
    
private:
    void hideOptions();
    
};
//============================================================================

#endif