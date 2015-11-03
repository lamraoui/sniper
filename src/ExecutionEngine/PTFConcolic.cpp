/**
 * PTFConcolic.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */

#include "PTFConcolic.h"


static unsigned MAX_RUN = 20000; // TODO: --max-time
static int RND_MIN = -2147483648;
static int RND_MAX =  2147483647;

PTFConcolic::PTFConcolic(Module *_llvmMod, Function *_targetFun, Options *_options) 
: ConcolicModule(_llvmMod, _targetFun, _options) {
    this->targetFun = _targetFun;
    srand(time(NULL));
}

PTFConcolic::~PTFConcolic() {
     //delete EE;
}

// =============================================================================
// run - Concolic algorithm used in the tools CUTE and DART.
//
// =============================================================================
void PTFConcolic::run(ProgramProfile *profile, LocalVariables *locVars, 
                      LoopInfoPass *loopInfo) {
    
    EE = initialize();
    unsigned nbRun = 0;
    terminated = false;
    roundID    = 1;
    Executor::init(loopInfo, profile,
    /*collectTrace*/ options->ptfUsed(),
    /*collectBlocks*/ options->htfUsed());

    while (roundID<=MAX_RUN) 
    {
        // Generate the inputs for the target function
        VariablesPtr inputValues = generateInputValues();
        lastInputs = inputValues;
        if (inputValues->empty()) {
            break;
        }
        if (options->dbgMsg()) {
            std::cout << "\n=== (S/F) RUN " << roundID << " ====================\n";
        }
        // Prepare the executor
        Executor::start(targetFun, inputValues, locVars, options);
        // Call the function f with the input values as argument
        std::vector<Value*> args = inputValues->getValues();
        callFunction(EE, llvmMod, targetFun, args);
        // Check if there were pointer incompleteness
        if(!Executor::areAllLocDefinite()) {
            std::cout << "warning: pointer incompleteness!" << std::endl;
        }
        // Next round
        roundID++;
        nbRun++;
        Executor::endOfRun();
    }
    if (options->dbgMsg()) {
         std::cout << std::endl;
         std::cout << std::endl;
    }
    // Check pending inputs
    SymbolicPath *symbPath = Executor::getSymbPath();
    std::vector<ProgramTrace*> traces = profile->getProgramTraces();    
    while (!pendingInputs.empty()) {
        std::pair<VariablesPtr,std::vector<State_t> > p = pendingInputs.back();
        pendingInputs.pop_back();
        VariablesPtr inputValues = p.first;
        std::vector<State_t> stack = p.second;
        if (inputValues->empty()) {
            continue;
        }
        // Check if we already computed these inputs
        bool isIn = false;
        for (ProgramTrace *t : traces) {
            VariablesPtr v = t->getInputVariables();
            if(inputValues->isValueEqualTo(v)) {
                isIn = true; break;
            }
        }
        if (isIn) {
            continue;
        }
        if (options->dbgMsg()) {
            std::cout << "\n=== (P) RUN " << roundID << " ====================\n";
        }
        // Prepare the executor
        symbPath->setStack(stack);
        Executor::start(targetFun, inputValues, locVars, options);
        // Call the function f with the input values as argument
        std::vector<Value*> args = inputValues->getValues();
        callFunction(EE, llvmMod, targetFun, args);
        // Check if there were pointer incompleteness
        if(!Executor::areAllLocDefinite()) {
            std::cout << "warning: pointer incompleteness!" << std::endl;
        }
        // Next round
        roundID++;
        nbRun++;
        Executor::endOfRun();
    }
    if (options->dbgMsg()) {
        std::cout << "\n=== [PTF Concolic] terminated ===\n";
        profile->dump();
    }
       // Cleaning
    Executor::clean();    
}


// =============================================================================
// generateInputValues
//
// Version 2: no tree, keep only the last path in memory
// =============================================================================
VariablesPtr PTFConcolic::generateInputValues() {
    
    VariablesPtr inputs = std::make_shared<Variables>();
    if (terminated) {
        return inputs;
    }
    // Random inputs for the first round
    if (roundID==1) {
        //srand(time(NULL)); 
        //std::cout << "Initial input values:\n";
        Function::arg_iterator ait;
        for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
            //std::string argName = ait->getName().str();
            //std::cout << argName << " = 0" << std::endl;
            /*int val = RND_MIN + rand() % (RND_MAX - RND_MIN);
            if (argName=="arg7") {
                val = 0 + rand() % (4 - 0);
            }*/
            //std::cout << argName << " = " << val << std::endl;
            inputs->add(ait, 0);
        }
        return inputs;
    }   
    // Get the deepset path to an unexplored branch node
    SymbolicPath *symbPath = Executor::getSymbPath();
    std::vector<ExprCellPtr> path = symbPath->getPath();
    // There is no more unexplored path if the path is empty
    if (path.empty()) {
        terminated = true;
        return inputs;
    }
    VariablesPtr failingInputs = std::make_shared<Variables>();
    std::vector<ExprCellPtr> asserts = symbPath->getAsserts();    
    std::vector<State_t> stack = symbPath->getStack();
    bool hasSuccSolution = false;
    bool hasFailSolution = false;
    int j = symbPath->getNbBranch()-1;
    while (j>=0) {
        if (stack[j].done==false) {
            stack[j].branch = !stack[j].branch;
            // Shrink: path[0..j]
            while ((int)path.size()>j+1) {
                path.pop_back();
            }
            hasFailSolution = solve(path, asserts, true, failingInputs);
            //if (!hasFailSolution || options->ptfUsed()) {
                hasSuccSolution = solve(path, asserts, false, inputs);
            //}
            if (hasSuccSolution || hasFailSolution) {
                break;
            }
        }
        j = j - 1;
    }
    if (j<0) {
        terminated = true;
        return inputs;
    }
    if (hasSuccSolution || hasFailSolution) {
        // Shrink: stack[0..j]
        while ((int)stack.size()>j+1) {
            stack.pop_back();
        }
        symbPath->setStack(stack);
        if (hasSuccSolution && hasFailSolution) {
            if (inputs->isValueEqualTo(failingInputs)) {
                return inputs;
            }
            // The generated failing inputs are already been saved?
            bool isIn = false;
            for (std::pair<VariablesPtr,std::vector<State_t> > p : pendingInputs) {
                if(p.first->isValueEqualTo(failingInputs)) {
                    isIn = true; break;
                }
            }
            if (!isIn) {
                std::pair<VariablesPtr,std::vector<State_t> > p 
                = std::make_pair(failingInputs, stack); 
                pendingInputs.push_back(p);
            }
            return inputs;
        } 
        else if (hasSuccSolution) {
            return inputs;
        }
        else if (hasFailSolution) {
            return failingInputs;
        }
    }
    std::cout << "error: concolic module!\n";
    return inputs;
}



// =============================================================================
// solve
//
// =============================================================================
bool PTFConcolic::solve(std::vector<ExprCellPtr> path, std::vector<ExprCellPtr> asserts,
                         bool genFailing, VariablesPtr inputs) {
    // Create a formula to hold the path constraints
    Formula *formula = new Formula();
    for (unsigned i=0; i<path.size()-1; i++) {
        ExprCellPtr n = path[i];
        // Create and add the constraint
        ExprPtr e;
        // Branch
        if (n->isBranch()) {
            BranchExprCellPtr cn = std::static_pointer_cast<BranchExprCell>(n);
            if (cn->getBranchTaken()) {
                e = cn->getExpr();
            } else {
                e = cn->getNotExpr(); // negate
            }
        }
        // FunctionCall, Assert
        else if (n->isFunCall()) {
            e = n->getExpr();
        } else {
            std::cout << "error: Concolic Module.\n";
            exit(1);
        }
        e->setHard();
        formula->add(e);
    }
    // Create and add the constraint in the formula:
    // If the branch taken in the last
    // execution was the true branch
    // we negate the selected constraint
    ExprPtr e;
    BranchExprCellPtr selectedNode = std::static_pointer_cast<BranchExprCell>(path.back());
    //std::cout << "Selected Node: ";
    //selectedNode->dump();
    //std::cout << std::endl;
    // Negate the last branch
    if (selectedNode->getBranchTaken()) {
        e = selectedNode->getNotExpr();  // negate
    } else {
        e = selectedNode->getExpr();
    }
    e->setHard();
    formula->add(e);
    
    // Add the asserts to the formula
    for (unsigned i=0; i<asserts.size(); i++) {
        ExprCellPtr n = asserts[i];
        ExprPtr e;
        if (genFailing) {
            e = n->getNotExpr();
        } else {
            e = n->getExpr();
        }
        //std::cout << "assert: ";
        //e->dump();
        //std::cout << std::endl;
        e->setHard();
        formula->add(e);
    }
    //formula->dump();
    
    //solver->init();
    // The formula is satisfiable
    int status = solver->check(formula);
    if(status==l_true) {
        //formula->dump();
        // Retrieve all main function arguments ,
        // retrieve their value from the model (of Yices)
        //std::cout << "Input values:\n";
        std::vector<InputVarTracePtr> lastInputsVec = lastInputs->getVector();
        unsigned i = 0;
        std::string fname("_"+targetFun->getName().str());
        Function::arg_iterator ait;
        for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
            std::string argName = ait->getName().str()+fname;
            bool error = false;
            int val = solver->getValue(argName, error);
            // Arg not involved in the solution: take the last value
            if (error) {
                int val = RND_MIN + rand() % (RND_MAX - RND_MIN);
                if (argName=="arg7") {
                    val = 0 + rand() % (4 - 0);
                }
                //std::cout << argName << " = " << val << "(err)" << std::endl;
                if (i<lastInputsVec.size()) {
                    //inputs->add(ait, val);
                    //inputs->add(ait, 0));
                    inputs->add(lastInputsVec[i]);
                } else {
                    std::cout << "error: Concolic Module.\n";
                    exit(1);
                }
            } else {
                //std::cout << argName << " = " << val << std::endl;
                inputs->add(ait, val);
            }
            i++;
        }
        //solver->clean();
        delete formula;
        return true; // found a solution
    } else if (status==l_false) {
        // The formula is unsatisfiable
        //solver->clean();
        delete formula;
        return false; // no solution
    } else {
        // The formula is undef
        //solver->clean();
        delete formula;
        std::cout << "error: solver (status " << status << ")\n";
        exit(1);
    }

}