/**
 * \file GlobalVariables.h
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
 * \date   29 January 2015
 */

#ifndef _GLOBALVARIABLES_H
#define _GLOBALVARIABLES_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/InstIterator.h>

#include "Utils/Utils.h"

using namespace llvm;

/**
 * \class GlobalVariables
 *
 * This class provides a method to move global variable
 * declarations to the target function.
 */
class GlobalVariables {
    
private:
    /**
     * Map a LLVM global variable to its 
     * associated load/store instruction.
     */
    std::map<GlobalVariable*, Value*> gv2val;
    /**
     * Map a LLVM global variable to its 
     * associated alloca instruction.
     */
    std::map<Value*, AllocaInst*> gv2a;
    
public:
    /**
     * Default constructor.
     */
    GlobalVariables() { }
    /**
     * Destructor.
     */
    ~GlobalVariables() { }
    
    /**
     * Move global variable declarations of the target LLVM module 
     * to the entry basic block of the LLVM function \p targetFun.
     *
     * \param targetFun An LLVM function.
     */
    void process(Function *targetFun);
    
private:
    /**
     * Replace variables in load or store instructions that 
     * points to a global variables so that it points to 
     * the local declaration of the global variable.
     *
     * \param inst An LLVM load/store instruction.
     */
    void replacePointer(Instruction *inst);
    
};

#endif // _GLOBALVARIABLES_H
