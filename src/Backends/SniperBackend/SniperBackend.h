/**
 * \file SniperBackend.h
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
 * \brief This class implements a backend to do automatic 
 * fault localization of multi-fault imperative programs.
 *
 * The backend follows the formula-based approach for 
 * imperative programs. 
 */
class SniperBackend {
    
private:
    Frontend *frontend;
    Options *options;
    struct timeval start; // Timer
    
public:
    /**
     * Default constructor.
     */
    SniperBackend(Frontend *frontend, Options *options) :
    frontend(frontend), options(options) {}
    /**
     * Destructor.
     */
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
