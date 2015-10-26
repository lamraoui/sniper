/**
 * BMC.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/29
 * @copyright : NII 2013
 */

#include "BMC.h"


BMC::BMC(Function *_targetFun, ISolver *_solver, Formula *_TF, Formula *_AS,
         LoopInfoPass *_loopInfo, Options *_options, bool _hasArgv) 
: targetFun(_targetFun), solver(_solver), TF(_TF), AS(_AS),
loopInfo(_loopInfo), options(_options), hasArgv(_hasArgv) { 

}


// =============================================================================
// runBMCWithPathExploration
//
// Compute a counterexample for all path using BMC (path exploration).
// F = pre^TF^notPost
//
// =============================================================================
void BMC::runBMCWithPathExploration(ProgramProfile *profile) {
    
    GenPath *gp = new GenPath();
    gp->run(targetFun, TF);
    std::vector<ExprPtr> fbb = gp->getFormulas();
    
    // Create a backtracking point.
    TF->push();
    
    // TODO: add all pre-condition (not negated)
    
    // Add all the not(post-condition) to the context
    for (ExprPtr e : AS->getExprs()) {
        ExprPtr ne = Expression::mkNot(e);
        TF->assertHard(ne);
    }
    
    // Compute iterative BMC
    if (options->verbose()) {
        std::cout << "\n==============================================\n";
        std::cout << "Starting Bounded Model Checking with Path Exploration...\n\n";
    }
    
    std::vector<ProgramTrace*> execs;
    std::vector<ExprPtr>::iterator itf;
    for(itf=fbb.begin(); itf!=fbb.end(); ++itf) {
        ExprPtr fbb_expr = *itf;
        
        // Create a backtracking point.
        TF->push();
        
        // Add the path formula to the context
        TF->assertHard(fbb_expr);
        
        switch(solver->check(TF)) {
                // Negated claim is satisfiable, i.e., does not hold
            case l_true: {
            } break;  
                // Negated claim is unsatisfiable: verification successful
            case l_false:
                // Backtrack (remove the path formula)
                TF->pop();
                continue; // go to the next iteration
            case l_undef:
                //std::cout << "unknown: it was not possible to decide due to an incompleteness.\n";
                // Backtrack (remove the path formula)
                TF->pop();
                continue; // go to the next iteration
        }    
        
        
        /*std::vector<ExprPtr> trans = formula->getLatchBlockTrans();
        std::vector<ExprPtr>::const_iterator ittl;
        for(ittl=trans.begin(); ittl!=trans.end(); ++ittl) {
            ExprPtr e = *ittl;
            BoolVarExprPtr bve = (BoolVarExprPtr) e;
            e->dump();
            int v = solver->getBoolValue(bve->getName());
            std::cout << " = " << v << std::endl;
        }*/
        
        
        ProgramTrace *E = new  ProgramTrace(targetFun, FAIL);
        
        // Function of type main(argc,argv)
        if (hasArgv) {
            /*std::string model_str = solver->getModel();
            // Extract the ARGV clauses
            std::string prefix = "(= (ARGV";
            std::string clause;   
            std::istringstream stream(model_str);
            while (std::getline(stream, clause)) {
                if (!clause.compare(0, prefix.size(), prefix)) {
                    E->addProgramInput(clause);
                }
            }*/
            error("argv not supported");
        }      
        
        // Retrieve all main function arguments ,
        // retrieve their value from the model (of BMC)
        const FunctionType *FTy = targetFun->getFunctionType();
        Function::arg_iterator ait;
        for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
            // Check the type of the argument
            const unsigned argNo = ait->getArgNo();
            const Type *argTy = FTy->getParamType(argNo);
            if (argTy->isIntegerTy() && !argTy->isIntegerTy(1)) {
                std::string argName = ait->getName().str();
                long value = solver->getValueOrZero(argName);
                Value *origin = ait;
                E->addProgramInput(origin, value);
            }
        }
        
        // Save the CE if is not already in vector
        bool alreadyIn = false;
        std::vector<ProgramTrace*>::iterator itt;
        for(itt=execs.begin(); itt!=execs.end(); ++itt) {
            ProgramTrace *c = *itt;
            if (c==E) {
                alreadyIn = true;
                break;
            }
        }
        if (!alreadyIn) {
            execs.push_back(E);
        }
        
        // Backtrack (remove the path formula)
        TF->pop();
    }
    
    // Backtrack to the last backtracking point
    // -> remove all notPostConditions
    TF->pop();
    
    const unsigned size = execs.size();
    if (size>0 && options->verbose()) {
        std::cout << "  Number of Error-inducing Inputs found: ";
        std::cout << size << std::endl;
        std::cout << std::endl;
    }
    // TODO: opt
    std::vector<ProgramTrace*>::iterator itt;
    for(itt=execs.begin(); itt!=execs.end(); ++itt) {
        ProgramTrace *e = *itt;
        profile->addProgramTrace(e);
    }
}





// =============================================================================
// runClassicBMC
//
// Compute a counterexample using BMC.
// F = pre^TF^notPost
//
// =============================================================================
void BMC::runClassicBMC(ProgramProfile *profile) {
    
    // Create a backtracking point.
    TF->push();
    
    // TODO: add all pre-condition (not negated)
    
    // Add all the not(post-condition) to the context
    for (ExprPtr e : AS->getExprs()) {
        ExprPtr ne = Expression::mkNot(e);
        TF->assertHard(ne);
    }
    
    // Compute BMC
    if (options->verbose()) {
        std::cout << "\n==============================================\n";
        std::cout << "Starting Bounded Model Checking...\n\n";
    }
        
    switch(solver->check(TF)) {
        // Negated claim is satisfiable, i.e., does not hold
        case l_true: {
        } break;  
        // Negated claim is unsatisfiable: verification successful
        case l_false:
            return;
        case l_undef:
            std::cout << "unknown: it was not possible"; 
            std::cout << " to decide due to an incompleteness.\n";
            return;
    }    
    // Loops: check if the model make the CFG
    //        jump back to a loop header.
    bool propertyViolated = false;
    std::vector<BranchInst*> loopLatchBrs = loopInfo->getLoopLatchBrs();
    std::vector<BranchInst*>::iterator itb;
    for (itb=loopLatchBrs.begin(); itb!=loopLatchBrs.end(); ++itb) {
        BranchInst *I = *itb;
        Value *cond = I->getCondition();
        int val = solver->getBoolValue(cond->getName().str());
        BasicBlock *headerBB = loopInfo->getLoopLatch(I);
        BasicBlock *nextBB1 = I->getSuccessor(0);
        BasicBlock *nextBB2 = I->getSuccessor(1);
        if ((headerBB && headerBB==nextBB1 && val==l_true) 
            || (headerBB && headerBB==nextBB2 && val==l_false)) {
            unsigned line = 0;
            if (MDNode *N = I->getMetadata("dbg")) { 
                DILocation Loc(N); 
                line = Loc.getLineNumber();
            }            
            std::cout << "Violated property: unwinding assertion";
            std::cout << " (line " << line << ")\n";
            propertyViolated = true;
        }
    }
    if (propertyViolated) {
        exit(0);
    }

    ProgramTrace *E = new ProgramTrace(targetFun, FAIL);
    
    // Function of type main(argc,argv)
    if (hasArgv) {
    /*    std::string model_str = solver->getModel();
        // Extract the ARGV clauses
        std::string prefix = "(= (ARGV";
        std::string clause;   
        std::istringstream stream(model_str);
        while (std::getline(stream, clause)) {
            if (!clause.compare(0, prefix.size(), prefix)) {
                E->addProgramInput(clause);
            }
        }
     */
        error("argv not supported");   
    }      
    
    // Retrieve all main function arguments ,
    // retrieve their value from the model (of BMC)
    const FunctionType *FTy = targetFun->getFunctionType();
    Function::arg_iterator ait;
    for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
        // Check the type of the argument
        const unsigned argNo = ait->getArgNo();
        const Type *argTy = FTy->getParamType(argNo);
        if (argTy->isIntegerTy() && !argTy->isIntegerTy(1)) {
            std::string argName = ait->getName().str();
            long value = solver->getValueOrZero(argName);
            Value *origin = ait;
            E->addProgramInput(origin, value);
        }
    }
    profile->addProgramTrace(E);
    
    // Backtrack to the last backtracking point (restores the context)
    // -> remove all notPostConditions
    TF->pop();
}
