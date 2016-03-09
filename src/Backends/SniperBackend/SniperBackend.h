/**
 * \file SniperBackend.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   8 March 2016
 */

#ifndef _SNIPERBACKEND_H
#define _SNIPERBACKEND_H

#include <cerrno>
#include <string>
#include <ctime>
#include <sys/time.h>
#include <vector>

#include "Options.h"
#include "Frontend/Frontend.h"
#include "Backends/SniperBackend/FaultLocalization.h"
#include "Backends/SniperBackend/EncoderPass.h"
#include "Encoder/Context.h"
#include "Logic/Combine.h"
#include "Logic/BMC.h"
#include "Logic/YicesSolver.h"
#include "ExecutionEngine/ConcolicProfiler.h"
#include "ExecutionEngine/IRRunner.h"
#include "Utils/Utils.h"

using namespace llvm;

/**
 * \class SniperBackend
 *
 * This class implements a backend to do automatic 
 * fault localization of multi-fault imperative programs.
 * The backend follows the formula-based approach for 
 * imperative programs. 
 */
class SniperBackend {
    
private:
    Frontend *frontend;
    Options *options;
    struct timeval start; // Timer
    
public:
    SniperBackend(Frontend *frontend, Options *options) :
    frontend(frontend), options(options) {}
    ~SniperBackend() { }

    /**
     * Run the backend on the target LLVM module/function.
     * This backend essentially does the following operations: 
     * - Run test case generation or process test cases provided by the user,
     * - Encode the target function into a trace formula, 
     * - Encode pre- and post-conditions into logic formulas,
     * - Run formula-based fault localization algorithms.
     */
    void run();
    
};

#endif // _SNIPERBACKEND_H
