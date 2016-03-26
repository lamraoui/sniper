/**
 * \file Executor.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   25 February 2016
 */

#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include <string>
#include <iostream>
#include <map>
#include <memory>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/InstIterator.h"

#include "ExecutionEngine/Symbol.h"
#include "ExecutionEngine/SymbolMap.h"
#include "ExecutionEngine/SymbolicPath.h"
#include "Profile/ProgramTrace.h"
#include "Profile/ProgramProfile.h"
#include "Frontend/LoopInfoPass.h"
#include "Frontend/LocalVariables.h"
#include "Encoder/Encoder.h"

using namespace llvm;

/**
 * If set to 1 non-linear operations are allowed 
 * in the concolic execution module. If set to 0 
 * linear operations are not allowed and these operations 
 * will be replaced by concrete values. 
 */
#define NON_LINEAR_SUPPORTED 0

/**
 * Executor running statuses.
 */
enum Status {
    IDLE,  /*< Executor is stopped and not yet initialized. */
    INIT,  /*< Executor is initialized. */
    START, /*< Executor is started and running. */
    END,   /*< Executor is terminated and cleaned. */
};

/**
 * \class Executor
 *
 * This class provides static functions that are called 
 * in the instrumented version of the target function. 
 * It collects various information related to program exections. 
 * It also collects path constraints and symbols for concolic execution. 
 */
class Executor {
    
public:
    static ProgramTrace *Trace;
    
private:
    static bool         Terminated;
    static unsigned     MaxDepth;
    static unsigned     AllLocDefinite;
    static bool         CollectBlockTraces;
    static bool         NoAssert;
    static unsigned     RunID;;
    static unsigned     GotoCount;
    static unsigned     InvokeCount;
    static AssertResult LastRunAssertResult;
    static AssertResult LastRunAssumeResult;
    static bool         HasAssume;
    static bool         DisabledSymbolicExe;
    static bool         DisabledSymbolicExeCurRun;
    
    static SymbolMap      *SMAP;
    static SymbolMap      *PMAP;
    static SymbolicPath   *Path;
    static ProgramProfile *Profile;
    static Function       *TargetFun;
    static BasicBlock     *LastExecutedBB;
    static BasicBlock     *NextBB;
    static LoopInfoPass   *PathLoopInfo;
    static Formula        *PathFormula;
    static Encoder        *PathEncoder;
    static Context        *PathContext;
    static Value          *ExpectedOuput;
    
    static std::vector<SymbolPtr>   argStack;
    static std::vector<BasicBlock*> ExecutedBlocks;
    
    static Status status;
    
public:
    /**
     * Default constructor.
     */
    Executor() { }
    /**
     * Destructor.
     */
    ~Executor() { }

    /**
     * Initialize the executor.
     * This procedure should be called before executing 
     * the instrumented target function. 
     *
     * \param lip Information about loops in the target function. 
     * \param p Information about the target function.
     * \param collectBlockTraces If true, profile executed basic blocks.
     * \param disableSymbExe If true, disable symbolic excution (default: false). 
     * \param noAssert If true, executor ignores assert functions.
     */
    static void init(LoopInfoPass *lip , ProgramProfile *p,
                     bool collectBlockTraces,
                     bool disableSymbExe = false, bool noAssert = false);
    /**
     * Start the executor. 
     * this procedure should be called before executing the 
     * instrumented target function and right after Executor::init. 
     *
     * \param f An instrumented LLVM function (target function).
     * \param vals Input values. 
     * \param lv Information about local variables in the target function. 
     */
    static void start(Function *f, VariablesPtr vals, LocalVariables *lv);
    
    /**
     * Symbolically execute \p i (memory operation).
     *
     * \param i An LLVM instruction to be symbolically executed.
     */
    static void ExecuteInst (Instruction *i);
     /**
     * Symbolically execute \p i (one the arguments is a concrete value).
     *
     * \param i An LLVM instruction to be symbolically executed.
     * \param val A concrete value. 
     */
    static void ExecuteInst1(Instruction *i, Value *val);
    /**
     * Symbolically execute \p i (two of the arguments are concrete values).
     *
     * \param i An LLVM instruction to be symbolically executed.
     * \param val1 A concrete value. 
     * \param val2 A concrete value. 
     */
    static void ExecuteInst2(Instruction *i, Value *val1, Value *val2);
    /**
     * Symbolically execute \p i (three of the arguments are concrete values).
     *
     * \param i An LLVM instruction to be symbolically executed.
     * \param val1 A concrete value. 
     * \param val2 A concrete value. 
     * \param val3 A concrete value.
     */
    static void ExecuteInst3(Instruction *i,
                             Value *val1, Value *val2, Value *val3);
    
    /**
     * Symbolically execute an assert check. 
     *
     * \param i The argument of the assert function (property to check).
     * \param assertResult Result of the assert function (fail or succeed).
     */
    static void ReportAssert(Value *i, int assertResult);
    /**
     * Symbolically execute an assume check. 
     *
     * \param i The argument of the assume function (property to check).
     * \param assumeResult Result of the assume function (fail or succeed).
     */
    static void ReportAssume(Value *i, int assumeResult);
    /**
     * Report the end of the target function execution.
     */
    static void ReportEnd();
    /**
     * Push an argument before calling a function (stack emulation). 
     * This function is required when the target function has 
     * function calls. 
     * See Executor::PopArgs.
     *
     * \param i A value to be pushed onto to the stack. 
     */
    static void PushArgs(Value *i);
    /**
     * Pop an argument after a function call (stack emulation). 
     * This function is required when the target function has 
     * function calls. 
     * See Executor::PushArgs.
     *
     * \param i A value to be popped from the stack. 
     */
    static void PopArgs(Value *i);
    /**
     * Set the maximum program depth. 
     * The executor stops when the maximum program depth is reached, 
     * which prevents long or infinite run of the concolic execution 
     * algorithm. 
     *
     * \param d A depth (number of maximum conditional branches to be executed).
     */
    static void setMaxDepth(unsigned d);
    /**
     * Return true if the memory state is all known, 
     * otherwise return false if there are some incertitudes 
     * about the program memory, for example because of a 
     * pointer deferencement. 
     */
    static bool areAllLocDefinite();
    /**
     * Return the current path constraint (symbolic path).
     */
    static SymbolicPath* getSymbPath();
    /**
     *  Stop the executor.
     */
    static void endOfRun();
    /**
     *  Clean the executor.
     */
    static void clean();
    
private:
    /**
     * Symbolically execute a GEP instruction. 
     * 
     * \param i A GEP instruction.
     */
    static void executeGep(Instruction *i);
     /**
     * Symbolically execute a load instruction. 
     * 
     * \param i A load instruction.
     * \param addr An address value.
     */
    static void executeLoad(Instruction *i, Value *addr);
     /**
     * Symbolically execute a store instruction. 
     * 
     * \param i A store instruction.
     * \param addr An address value.
     */
    static void executeStore(Instruction *i, Value *addr);
     /**
     * Symbolically execute a cast instruction. 
     * 
     * \param i A cast instruction.
     */
    static void executeCast(Instruction *i);
     /**
     * Symbolically execute a call instruction. 
     * 
     * \param i A call instruction.
     * \param input An input argument.
     * \param output The output of the function.  
     */
    static void executeCall(Instruction *i, Value *input, Value *output);
     /**
     * Symbolically execute a binary instruction. 
     * 
     * \param i A binary instruction.
     * \param val1 First argument.
     * \param val2 Second argument.
     */
    static void executeBinaryOp(Instruction *i, Value *val1, Value *val2);
     /**
     * Symbolically execute a select instruction. 
     * 
     * \param i A select instruction.
     * \param cond The select condition.
     * \param trueval The "then" argument.
     * \param falseval The "else" argument.
     */
    static void executeSelect(Instruction *i,
                              Value *cond, Value *trueval, Value *falseval);
     /**
     * Symbolically execute a phi instruction. 
     * 
     * \param i A phi instruction.
     */
    static void executePhi(Instruction *i);
     /**
     * Symbolically execute a branch (br) instruction. 
     * 
     * \param i A branch instruction.
     * \param cond If true, the "then" was taken, 
     * otherwise the "else" branch was taken.
     */
    static void executeBranch(Instruction *i, bool cond);
    
    /**
     * Return true if \p i is a non-linear operation.
     */
    static bool isNonLinear(Instruction *i);

};

#endif // _EXECUTOR_H
