/**
 * \file ProgramTrace.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   20 January 2015
 */

#include "ProgramTrace.h"

unsigned ProgramTrace::ID = 0;

void ProgramTrace::setExecutedBlocks(std::vector<BasicBlock*> &bb) {
    std::copy(bb.begin(), bb.end(),
              std::inserter(executedBlocks, executedBlocks.end()));
}

void ProgramTrace::addProgramInput(Value *origin, Value *val) {
    assert((origin && val) && "Expecting values!");
    InputVarTracePtr vt = std::make_shared<InputVariableTrace>(origin, val);
    inputVars->add(vt);
}

void ProgramTrace::addProgramInput(Value *origin, int val) {
    assert(origin && "Expecting a value!");
    InputVarTracePtr vt =  std::make_shared<InputVariableTrace>(origin, val);
    inputVars->add(vt);
}

void ProgramTrace::addProgramInput(InputVarTracePtr ivt) {
    assert(ivt && "Expecting an input variable trace!");
    inputVars->add(ivt);
}

std::set<BasicBlock*> ProgramTrace::getExecutedBB() {
    return executedBlocks;
}

// Create the formula that encode the program inputs:
// (and (= var1 val1) (= var2 val2) ...) 
ExprPtr ProgramTrace::getProgramInputsFormula(Formula *formula) {

    assert(formula && "Expecting a formula!");
    std::vector<ExprPtr> and_args;   
    std::vector<InputVarTracePtr> vars = inputVars->getVector();
    for(InputVarTracePtr v : vars) {
        ExprPtr val_expr = Expression::mkSInt32Num(v->getInt32());    
        ExprPtr d_expr = Expression::mkIntVar(v->getName());
        and_args.push_back(Expression::mkEq(d_expr, val_expr));
    }
    // Oracle
    if (expectedOutput!=NULL) {
        // Get the return value of targetFun
        assert(!targetFun->doesNotReturn() &&
               "expectedOutput set but fun does not return.");
        BasicBlock *lastBlock = &targetFun->back();
        assert(lastBlock && "Basic block is null!");
        Instruction *lastInst = &lastBlock->back();
        assert(lastInst && "Instruction is null!");
        ReturnInst *RI = dyn_cast<ReturnInst>(lastInst);
        assert(RI && "No ret instruction!");
        Value *V = RI->getReturnValue();
        assert(V && "No return value!");
        ExprPtr v_expr = Expression::mkIntVar(V->getName().str());
        // Get the concrete value
        ConstantInt *EO = dyn_cast<ConstantInt>(expectedOutput);
        assert(EO && "No concrete value!");
        int oval = (int) EO->getSExtValue();
        ExprPtr oval_expr = Expression::mkSInt32Num(oval);
        // (= ret_var oval)
        and_args.push_back(Expression::mkEq(v_expr, oval_expr));
    }
    ExprPtr expr;
    if (and_args.size()==1) {
        expr = and_args[0];
    }
    else if (and_args.size()>1) {
        expr = Expression::mkAnd(and_args); 
    } else {
        return NULL;
    }
    return expr;
}

void ProgramTrace::dumpProgramInputs() {
    inputVars->dump();
}

void ProgramTrace::dump() {
    dumpProgramInputs();
    std::string st = "unknow";
    if (type==FAIL) {
        st = "failing";
    } else if (type==SUCCESS) {
        st = "successful";
    }
    std::cout << " (ID=" << myID << ")  [" << st << "]\n";
}
