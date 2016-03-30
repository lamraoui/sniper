/**
 * \file Context.cpp
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
 * \date   1 March 2016
 */

#include "Context.h"

// =============================================================================
// getLocalVariable
// =============================================================================
ExprPtr Context::getLocalVariable(Instruction *i, Value *arg, int pos) {
    
    // Replace only constante values
    if(!isa<ConstantInt>(arg) && arg->getValueID()!=Value::UndefValueVal) {
        return NULL;
    }
    
    // Retrieve pointer of the local variable
    std::string name = locVar->getPtr(i, pos);
    if (name=="") {
        return NULL;
    }
    
    // TODO opt
    // Already created?
    name2expr_map::iterator it;
    for(it = name2expr.begin(); it != name2expr.end(); ++it) {
        std::string n = it->first;
        if (n.compare(name)==0) {
            return it->second;
        } 
    }
    
    ExprPtr localVar;
    assert(arg->getType()->isIntegerTy() && "Not supported type!");
    // Boolean
    if(arg->getType()->isIntegerTy(1)) {
        localVar = Expression::mkBoolVar(name);
    }
    // Integer 8,16,32,64bits
    else {
        localVar = Expression::mkIntVar(name);
    }
    // Assert the local variable initialization
    if(arg->getValueID()!=Value::UndefValueVal) {
        
        /*******************************************
         *   TODO                                  *
         *******************************************/

        //std::cout << "[REMOVED CODE]:[Context:68]" << std::endl;

         /*
         ExprPtr argExpr = newVariable(arg);
         ExprPtr eqExpr = Expression::mkEq(localVar, argExpr);
         // Assert as soft
         assert(formula && "Formula is null!");
         unsigned line = locVar->getLine(name);
         eqExpr->setInstruction(i);
         formula->assertSoft(eqExpr);
         eqExpr->setLine(line);
         */
    }
    // Add the newly created number into the map
    this->name2expr[name] = localVar;
    return localVar;
}

// =============================================================================
// newVariable
//
// default: bb=0,
// =============================================================================
ExprPtr Context::newVariable(Value *val) {
    ExprPtr expr = NULL;
    // Constant
    // --------------------------------------------------------
    if(ConstantInt *ci = dyn_cast<ConstantInt>(val)) {
        // Make a constant number if it is not already created 
        expr = this->val2num[val];
        if(expr==NULL) {
            int cst = (int) ci->getSExtValue();
            // Boolean
            if(val->getType()->isIntegerTy(1)) {
                if(cst==0)
                    expr = Expression::mkFalse();  
                else
                    expr = Expression::mkTrue();
            } 
            // Integer 8,16,32,64bits
            else {
                expr = Expression::mkSInt32Num(cst);
            }
            // Add the newly created number intto the map
            this->val2num[val] = expr;
        }
    } 
    // Label
    // --------------------------------------------------------
    else if(val->getType()->isLabelTy() || isa<BasicBlock>(val)) {
        // Create a new variable if it is not already created 
        expr = getCondVariable(val);
        if(expr==NULL) {
            expr = Expression::mkBoolVar(val->getName());
            // Add the newly created number into the map
            this->val2lbl[val] = expr;
        }
    } 
    // Alloca
    // --------------------------------------------------------
    else if (isa<AllocaInst>(val)) {
        assert("Pointer/struct types are not supported!");
    }
    // Global variable
    // --------------------------------------------------------
    else if (GlobalVariable *gv = dyn_cast<GlobalVariable>(val)) {
        
        // Check the map before
        /*expr = getVariable(val);
         if(expr!=NULL) {
         return expr;
         }*/

        // Update the version number of the variable
        this->gv2v[gv]++; 
        std::stringstream vers;
        vers << this->gv2v[gv];
        std::string varName = gv->getName().str() + vers.str();
        
        const PointerType *pty = dyn_cast<PointerType>(gv->getType());
        const Type *ety = pty->getElementType();
        if(ety->isIntegerTy()) {
            // Create a new variable 
            // Boolean
            if(val->getType()->isIntegerTy(1)) {
                expr = Expression::mkBoolVar(varName);
            } 
            // Integer 8,16,32,64bits
            else {
                expr = Expression::mkIntVar(varName);
            }
            // Add the newly created number into the map
            this->val2expr[val] = expr;
        }
        // Array 
        else if(isa<ArrayType>(ety)) {
            //TODO 
            std::string s = val->getName();
            std::cout << s << std::endl;
            assert("Global array types are not supported!");
        }
        // Pointer & Struct
        else {
            //TODO 
            std::string s = val->getName();
            std::cout << s << std::endl;
            assert("Pointer/struct types are not supported!");
        }
    }    
    // Pointer, Struct, Array
    // --------------------------------------------------------
    else if(val->getType()->isPointerTy()) {
        
        
        /********/
        // TODO: verify pointer type!
        /********/
        
        // Create a new variable if it is not already created 
        expr = getVariable(val);
        if(expr==NULL) {
            expr = Expression::mkIntVar(val->getName());
            // Add the newly created number into the map
            this->val2expr[val] = expr;
        }
        
        /********/
        
        
        
    }
    // ICMP (variable for the result of a comparison)
    // --------------------------------------------------------
    else if(isa<ICmpInst>(val)) { 
        
        // TODO new map
        
        // Create a new variable if it is not already created 
        expr = getVariable(val);
        if(expr==NULL) {
            expr = Expression::mkBoolVar(val->getName());
            // Add the newly created number into the map
            this->val2expr[val] = expr;
        }        
    }
    // Variable
    // --------------------------------------------------------
    else {
        assert(val->getType()->isIntegerTy() && "Not supported type!");
        // Create a new variable if it is not already created
        expr = getVariable(val);
        if(expr==NULL) {
            // Boolean
            if(val->getType()->isIntegerTy(1)) {
                expr = Expression::mkBoolVar(val->getName());
            }
            // Integer 8,16,32,64bits
            else {
                expr = Expression::mkIntVar(val->getName());
            }
            // Add the newly created number into the map
            this->val2expr[val] = expr;
        }
    }
    return expr;
}

// =============================================================================
// newTransition
// =============================================================================
ExprPtr Context::newTransition(BasicBlock *pred, BasicBlock *cur) {
    std::string name = pred->getName().str()+"_"+cur->getName().str();    
    ExprPtr trans_var = Expression::mkBoolVar(name);
    this->str2trans[name] = trans_var;
    return trans_var;
}

// =============================================================================
// getTransition
// =============================================================================
ExprPtr Context::getTransition(BasicBlock *pred, BasicBlock *cur) {
    std::string name = pred->getName().str()+"_"+cur->getName().str();
    return this->str2trans[name];
}

// =============================================================================
// getCondVariable
// 
// Retrieve a yices conditional variable from the key-value map
// =============================================================================
ExprPtr Context::getCondVariable(Value *val) {
    if(val->getType()->isLabelTy()) {
        return this->val2lbl[val];
    } else {
        return NULL;
    }
}

// =============================================================================
// getVariable
// 
// Retrieve a yices variable from the key-value map
// =============================================================================
ExprPtr Context::getVariable(Value *val) {
    val2expr_map::iterator it = val2expr.find(val);
    if(it!=val2expr.end()) {
       return it->second; // element found
    } else {
        return NULL;
    }
}


// =============================================================================
// propagatePointers
// =============================================================================
ExprPtr Context::propagatePointers(BasicBlock *bb) {
    
    // ------------------------
    // Entry BB 
    // ------------------------
    BasicBlock *entryBB = &bb->getParent()->getEntryBlock();
    if (bb==entryBB) {
        // Nothing to propagate
        this->bb2id[bb] = 0;
        return NULL;
    }
    // ------------------------
    // Only one pred BB
    // ------------------------
    BasicBlock *singlePredBB = bb->getSinglePredecessor();
    if (singlePredBB) {
        this->bb2id[bb] = this->bb2id[singlePredBB];
        return NULL;
    }
    // ------------------------
    // More than one pred BB
    // ------------------------
    std::vector<BasicBlock*> bbs;
    for(pred_iterator PI=pred_begin(bb), PE=pred_end(bb); PI!=PE; ++PI) {
        BasicBlock *predBB = *PI;
        if(this->bb2id[predBB]>0) {
            bbs.push_back(predBB);
        }
    }
    // No store in pred BBs
    if (bbs.size()==0) {
        // Nothing to propagate
        this->bb2id[bb] = 0;
        return NULL;
    } 
    // One store in one of the pred BBs
    else if (bbs.size()==1) {  
        this->bb2id[bb] = this->bb2id[bbs.front()];
        return NULL;
    } 
    // else: More than one store in pred BBs
    std::vector<BasicBlock*>::reverse_iterator rit;
    rit = bbs.rbegin();
    ExprPtr iteExpr = NULL;
    ExprPtr lastValVar = NULL;
    // Check if MEMs used in the previous
    // blocks are the same
    bool differentMems = false;
    for(BasicBlock *bb1 : bbs) {
        int id1 = this->bb2id[bb1];
        for(BasicBlock *bb2 : bbs) {
            if (bb1==bb2) {
                continue;
            }
            int id2 = this->bb2id[bb2];
            if (id1!=id2) {
                differentMems = true;
                break;
            }
        }
        if (differentMems) {
            break;
        }
    }
    if (!differentMems) {
        // Propagate the ID to the current block
        this->bb2id[bb] = memID;
        return NULL;
    }
    // Last mem variable
    std::stringstream sstm;
    sstm << "MEM" << this->bb2id[bbs.back()];
    std::string name = sstm.str();
    lastValVar = Expression::mkIntToIntVar(name);
    rit++;
    for (; rit!=bbs.rend(); ++rit) { // reverse order
        BasicBlock *pbb = *rit;
        // Get the mem variable
        std::stringstream sstm1;
        sstm1 << "MEM" << this->bb2id[pbb];
        std::string name1 = sstm1.str();
        ExprPtr valVar = Expression::mkIntToIntVar(name1);
        // Make the PHI formula
        // (= MEM3 (ite bb1 MEM1 MEM2))
        //ExprPtr arg1 = getCondVariable(pbb); // bb0
        //iteExpr = Expression::mkIte(arg1, valVar, lastValVar);
        
        // (= MEM3 (ite predbb_bb MEM1 MEM2))
        ExprPtr transExpr = getTransition(pbb, bb);
        iteExpr = Expression::mkIte(transExpr, valVar, lastValVar);
        lastValVar = iteExpr;
    }      
    // New mem variable
    memID++;
    this->bb2id[bb] = memID;
    std::stringstream sstm2;
    sstm2 << "MEM" << memID;
    std::string name2 = sstm2.str();    
    ExprPtr phiVar = Expression::mkIntToIntVar(name2);
    // (assert (= a [ITE]))
    ExprPtr eqExpr = Expression::mkEq(phiVar, iteExpr);
    return eqExpr; // hard
}

// =============================================================================
// propagatePointers (for concolic module)
// =============================================================================
void Context::propagatePointers(BasicBlock *bb, BasicBlock *nextbb) {
    
    assert(nextbb && "Cannot propagate pointers!");
    /*
    // ------------------------
    // Entry BB
    // ------------------------
    BasicBlock *entryBB = &bb->getParent()->getEntryBlock();
    if (bb==entryBB) {
        // Nothing to propagate
        //this->bb2id[bb] = 0;
        return;
    }*/
    // ------------------------
    // One or more than one pred BB
    // ------------------------
    this->bb2id[nextbb] = this->bb2id[bb];
}

// =============================================================================
// dump
// 
// Dump the expression maps
// =============================================================================
void Context::dump() {
    std::cout << "-------" << std::endl;
    val2expr_map::iterator it;
    for (it = val2expr.begin (); it != val2expr.end (); ++it) {
        Value *key = (*it).first;
        ExprPtr var = (*it).second;
        std::cout << "name = " << key->getName().str() 
        << "  expr = " << var << std::endl;
    }
    val2expr_map::iterator it2;
    for (it2 = val2num.begin (); it2 != val2num.end (); ++it2) {
        Value *key = (*it2).first;
        ExprPtr var = (*it2).second;
        if(ConstantInt *ci = dyn_cast<ConstantInt>(key)) {
            int val = (int) ci->getSExtValue();
            std::cout << "num = " << val
            << "  expr = " << var << std::endl;
        }
    }
    val2expr_map::iterator it3;
    for (it3 = val2lbl.begin (); it3 != val2lbl.end (); ++it3) {
        Value *key = (*it3).first;
        ExprPtr var = (*it3).second;
        std::cout << "label = " << key->getName().str()
        << "  expr = " << var << std::endl;        
    }
    
    std::cout << "-------" << std::endl;
}