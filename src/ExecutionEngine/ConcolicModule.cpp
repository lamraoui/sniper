/**
 * ConcolicModule.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */

#include "ConcolicModule.h"


ConcolicModule::ConcolicModule(Module *_llvmMod, Function *_targetFun, Options *_options) 
: llvmMod(_llvmMod), targetFun(_targetFun), options(_options), lastInputs(NULL) {
    // Create an instruction builder
    LLVMContext &Context = getGlobalContext();
    this->IRB =  new IRBuilder<>(Context);
    // Set the maximum depth for the executions
    Executor::setMaxDepth(options->getMaxDepth());
}


// =============================================================================
// initialize
//
// =============================================================================
ExecutionEngine* ConcolicModule::initialize() {
    // Create execution engine 
    InitializeNativeTarget();
    std::string errStr;
    EngineBuilder builder(llvmMod);
    builder.setEngineKind(EngineKind::JIT)
           .setErrorStr(&errStr)
           .setOptLevel(CodeGenOpt::None);
    ExecutionEngine *EE = builder.create();
    if (!EE) {
        std::cout << "error: could not create ExecutionEngine: " << errStr << std::endl;
        exit(1);
    }
    // Instrument the target function
    IRInstrumentor *IRI = new IRInstrumentor(llvmMod, EE);
    IRI->instrumentModule(llvmMod, targetFun);
    delete IRI;
    // Init the SMT solver
    this->solver = new YicesSolver();
    return EE;
}


// =============================================================================
// callFunction
//
// Create a new llvm function (warper) that takes no arguments.
// This new function will call the instrumented function
// with the generated argument values.
// =============================================================================
Value* ConcolicModule::callFunction(ExecutionEngine *EE, Module *m, Function *f,
                                  std::vector<Value*> args) {
    
    // TODO: call with getPointerToFunction
    // stackoverflow.com
    // /questions/15865191/how-to-call-a-jited-llvm-function-with-unknown-type
    
    Value *output;
    // Make a new function: int f()
    LLVMContext &Context = getGlobalContext();
    //Type *int32ty = Type::getInt32Ty(Context);
    Type *fRetTy = f->getReturnType();
    // Delete the old warper
    Function *w = m->getFunction("warper");
    if (w!=NULL) {
        w->eraseFromParent();
    }
    Constant *c = m->getOrInsertFunction("warper", fRetTy, NULL);
    Function *warper = cast<Function>(c);
    warper->setCallingConv(CallingConv::C);
    // Make the call
    BasicBlock* entry = BasicBlock::Create(Context, "entry", warper);
    IRB->SetInsertPoint(entry);
    Value* fcall = IRB->CreateCall(f, args);
    if (fRetTy->isIntegerTy()) {
        IRB->CreateRet(fcall);
    } else {
        IRB->CreateRetVoid();
    }
    // Validate the generated code, 
    // checking for consistency.
    verifyFunction(*warper);
    //warper->dump();
    // JIT and run the function warper
    EE->finalizeObject();
    void *warperAddr = EE->getPointerToFunction(warper);
    // Call the function with argument n:
    //std::vector<GenericValue> Args;
    //Args[0].IntVal = APInt(32, 1);
    //GenericValue GV = EE->runFunction(warper, Args);
    // int runFunctionAsMain(Function *Fn, 
    // const std::vector<std::string> &argv,
    // const char * const * envp);
    
    if (fRetTy->isIntegerTy(32)) {
        typedef int (*FuncType)();
        FuncType fFunc = (FuncType)warperAddr;
        int r = fFunc();
        output = IRB->getInt32(r);
        //std::cout << "Ret: " << r << std::endl;
    } else {
        if (fRetTy->isVoidTy()) {
            typedef void (*FuncType)();
            FuncType fFunc = (FuncType)warperAddr;
            fFunc();
            output = NULL;
            //std::cout << "\n" << "Ret: void" << std::endl;
        } else {
            std::cout << "error: the return type of the function ";
            std::cout << f->getName().str();
            std::cout << " is not supported.\n";
            exit(1);
        }
    }
    // Deallocate memory used 
    // to code-generate this Function.
    EE->freeMachineCodeForFunction(warper);
    return output;
}

ConcolicModule::~ConcolicModule() {
    delete IRB;
    delete this->solver;
}