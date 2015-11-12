/**
 * Executor.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
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


#define NON_LINEAR_SUPPORTED 0

using namespace llvm;

// Executor status
enum Status {
    IDLE,
    INIT,
    START,
    END,
};

//============================================================================
class Executor {
    
public:
    static ProgramTrace *Trace;
    
private:
    static bool         Terminated;
    static unsigned     MaxDepth;
    static unsigned     AllLocDefinite;
    static bool         CollectInstTraces;
    static bool         CollectBlockTraces;
    static bool         NoAssert;
    static unsigned     RunID;;
    static unsigned     GotoCount;
    static unsigned     InvokeCount;
    static AssertResult LastRunAssertResult;
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
    Executor() { }
    ~Executor() { }
    static void init(LoopInfoPass *lip , ProgramProfile *p,
                     bool collectTrace, bool collectBlockTraces,
                     bool disableSymbExe = false, bool noAssert = false);
    static void start(Function *f, VariablesPtr vals, LocalVariables *lv, Options *o);
    
    static void ExecuteInst (Instruction *i);
    static void ExecuteInst1(Instruction *i, Value *val);
    static void ExecuteInst2(Instruction *i, Value *val1, Value *val2);
    static void ExecuteInst3(Instruction *i, Value *val1, Value *val2, Value *val3);
    
    static void ReportAssert(Value *i, int assertResult);
    static void ReportAssume(Value *i, int assumeResult);
    static void ReportEnd();
    static void PushArgs(Value *i);
    static void PopArgs(Value *i);
    static void setMaxDepth(unsigned d);
    static bool areAllLocDefinite();
    static SymbolicPath* getSymbPath();
    static void endOfRun();
    static void clean();
    
private:
    static void executeGep(Instruction *i);
    static void executeLoad(Instruction *i, Value *addr);
    static void executeStore(Instruction *i, Value *addr);
    static void executeCast(Instruction *i);
    static void executeCall(Instruction *i, Value *input, Value *output);
    static void executeBinaryOp(Instruction *i, Value *val1, Value *val2);
    static void executeSelect(Instruction *i, Value *cond, Value *trueval, Value *falseval);
    static void executePhi(Instruction *i);
    static void executeBranch(Instruction *i, bool cond);
        
    static bool isNonLinear(Instruction *i);
    
    static void addToFormula(Instruction *i);
    
};
//============================================================================

#endif
