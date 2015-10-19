/**
 * PTFRunner.cpp
 *
 * Same as PTFConcolic without generating new input 
 * value for the program.
 * Values are provided by the user.
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/09/18
 * @copyright : NII 2013
 */

#include "PTFRunner.h"


PTFRunner::PTFRunner(Module *_llvmMod, Function *_targetFun, Options *_options,
                     const std::string &tsFilename, const std::string &goFilename)
: ConcolicModule(_llvmMod, _targetFun, _options) {
    this->targetFun = _targetFun;
    this->inputValuesVec = parseTestsuiteFile(tsFilename);
    if (!goFilename.empty()) {
        this->outputValuesVec = parseGoldenOutputsFile(goFilename);
    }
}

PTFRunner::~PTFRunner() {
     //delete EE;
}

void PTFRunner::run(ProgramProfile *profile, LocalVariables *locVars,
                    LoopInfoPass *loopInfo) {
    // Init
    EE = initialize();
    Executor::init(loopInfo, profile,
                   /*collectTrace*/ options->ptfUsed(),
                   /*collectBlocks*/ options->htfUsed(),
                   /* desactivate symbolic execution */ true,
                   /* no assert */ true);
    // Execute the program for each input list
    int i = 0;
    for (std::vector<Value*> IV : inputValuesVec) {
        if (IV.empty()) {
            error("empty input values list!");
        }
        // Prepare the executor
        VariablesPtr inputs = std::make_shared<Variables>();
        Function::arg_iterator ait = targetFun->arg_begin();
        for (Value *v : IV) {
            if(ConstantInt *ci = dyn_cast<ConstantInt>(v)) {
                int val = (int) ci->getSExtValue();
                inputs->add(ait++, val);
            } else {
                error("no concrete value for variable!");
            }
        }
        Executor::start(targetFun, inputs, locVars, options);
        // Call the function f with the input values as argument
        Value *output = callFunction(EE, llvmMod, targetFun, IV);
        // Get the expected output (if any)
        Value *expectedOutput = NULL;
        if (!outputValuesVec.empty()) {
            expectedOutput = outputValuesVec[i++];
        } else {
            std::cout << "WARNING!!!\n";
        }
        // If an expected ouput is provided,
        // we use it to determine the result
        if (expectedOutput && output) {
            ConstantInt *O = dyn_cast<ConstantInt>(output);
            ConstantInt *EO = dyn_cast<ConstantInt>(expectedOutput);
            if (O && EO) {
                int oval = (int) O->getSExtValue();
                int eoval  = (int) EO->getSExtValue();
                if (oval==eoval) {
                    Executor::Trace->setSuccessful();
                } else {
                    Executor::Trace->setFailing();
                    Executor::Trace->setExpectedOutput(expectedOutput);
                }
            } else {
                error("no concrete value for variables!");
            }
        } else {
            Executor::Trace->setUnknow();
        }
        // Check if there were pointer incompleteness
        if(!Executor::areAllLocDefinite()) {
            std::cout << "warning: pointer incompleteness!" << std::endl;
        }
        // Next round
        Executor::endOfRun();
    }
    if (options->dbgMsg()) {
        std::cout << "\n=== [PTF Runner] terminated ===\n";
        profile->dump();
    }
    // Cleaning
    Executor::clean();    
}


std::vector<std::vector<Value*> > PTFRunner::parseTestsuiteFile(
                                const std::string &filename) {
    std::ifstream infile(filename.c_str());
    if (!infile) {
        error("cannot open the testsuite file.");
    }
    std::string line;
    std::vector< std::vector<int> > all_integers;
    while ( getline( infile, line ) ) {
        std::istringstream is( line );
        all_integers.push_back(
        std::vector<int>( std::istream_iterator<int>(is),
        std::istream_iterator<int>() ) );
    }
    LLVMContext &context = getGlobalContext();
    IRBuilder<> IRB(context);
    std::vector<std::vector<Value*> > TS;
    for(std::vector<int> line_ints : all_integers) {
        std::vector<Value*> TC;
        for (int val : line_ints) {
            Value *V = IRB.getInt32(val);
            TC.push_back(V);
        }
        TS.push_back(TC);
    }
    if (options->dbgMsg()) {
        std::cout << TS.size() << " testcase(s) loaded.\n";
    }
    return TS;
}


std::vector<Value*> PTFRunner::parseGoldenOutputsFile(const std::string &filename) {
    // Open the file
    std::ifstream infile(filename.c_str());
    if (!infile) {
        error("cannot open the golden outputs file.");
    }
    // Read the elements in the file into a vector
    std::vector<int> allValues;
    int value;
    while ( infile >> value ) {
        allValues.push_back(value);
    }
    // Convert the integers into llvm values
    LLVMContext &context = getGlobalContext();
    IRBuilder<> IRB(context);
    std::vector<Value*> GO;
    for (int val : allValues) {
        Value *V = IRB.getInt32(val);
        GO.push_back(V);
    }
    if (options->dbgMsg()) {
        std::cout << GO.size() << " testcase(s) loaded.\n";
    }
    return GO;
}

