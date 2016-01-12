/**
 * SniperBackend.h
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/09/03
 * @copyright : NII 2013
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
#include "Backends/SniperBackend/IterationAlgorithm.h"
#include "Backends/SniperBackend/EncoderPass.h"
#include "Encoder/Context.h"
#include "Logic/Combine.h"
#include "Logic/BMC.h"
#include "Logic/YicesSolver.h"
#include "ExecutionEngine/ConcolicProfiler.h"
#include "ExecutionEngine/IRRunner.h"
#include "Utils/Utils.h"


using namespace llvm;


//============================================================================
class SniperBackend {
    
private:
    Frontend *frontend;
    Options *options;
    struct timeval start; // Timer
    
public:
    SniperBackend(Frontend *frontend, Options *options) :
    frontend(frontend), options(options) {}
    ~SniperBackend() { }
    void run();
    
};
//============================================================================

#endif
