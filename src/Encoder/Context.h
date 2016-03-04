/**
 * \file Context.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   1 March 2016
 */

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <string>
#include <iostream>

#include "llvm/IR/LLVMContext.h"

#include "Options.h"
#include "Logic/Formula.h"
#include "Frontend/LocalVariables.h"
#include "Frontend/LoopInfoPass.h"

using namespace llvm;


// TODO : unordered_map
/**
 * Map LLVM values to logic expression.
 */
typedef std::map<Value*, ExprPtr> val2expr_map;
/**
 * Map string representations of transition expressions.
 * For example a transition from a basic block bb0 to a 
 * basic block bb1 is represented by the string bb0_bb1.
 */
typedef std::map<std::string, ExprPtr> str2trans_map;
/**
 * Map variable name to logic expressions.
 */
typedef std::map<std::string, ExprPtr> name2expr_map;

/**
 * \class Context
 *
 * A context stores a collection of variables and expressions.
 * The context maps LLVM variables to SNIPER expressions.  
 * It also provides functions to manipulate the map, 
 * such as add or get.
 * In addition, it maps assert functions in the IR to line numbers.
 * The main goal of a context is to avoid duplicated expressions 
 * and to make the processing of IR faster and convenient.
 *
 */
class Context {

private:
    LocalVariables *locVar;
    bool is64Bit;
    unsigned phiCounter;
    unsigned memPtr, memID;
    
    // Value
    val2expr_map val2expr;
    val2expr_map val2num;
    val2expr_map val2lbl;
    
    str2trans_map str2trans;
    name2expr_map name2expr;
    
    // GlobalVariable 
    std::map<Value*, int> gv2v;
    
    // Pointers ID
    std::map<Value*, int> ptr2id;
    std::map<BasicBlock*, int> bb2id;
    
    std::map<unsigned,bool> assertCall2lines;
    
    // TCAS
    std::string assertVarName;
    unsigned arrayCheckId;
    std::vector<ExprPtr> arrayCheckExprs;
    
public:
    Context(LocalVariables *_locVar)
    : locVar(_locVar), phiCounter(0), memPtr(0), memID(0),
    /*TCAS*/ arrayCheckId(0) { }
    /**
     * Destructor.
     */
    ~Context() { }
    
    /**
     * Create a new expression representating an LLVM value 
     * and store it to the context.
     *
     * \param val An LLVM value to be encoded.
     * \param bb An LLVM basic block.
     * \return a value expression.
     */
    ExprPtr newVariable(Value *val);
    /**
     * Create a new expression representating a transition 
     * between two basic blocks and store it to the context.
     *
     * \param pred An LLVM basic block (start).
     * \param cur An LLVM basic block (end).
     * \return a transition expression.
     */
    ExprPtr newTransition(BasicBlock *pred, BasicBlock *cur);
    /**
     * Return an existing (already stored in the context) 
     * basic block transition expression.
     *
     * \param pred An LLVM basic block (start).
     * \param cur An LLVM basic block (end).
     * \return a transition expression.
     */
    ExprPtr getTransition(BasicBlock *pred, BasicBlock *cur);
    /**
     * Return an existing (already stored in the context)  
     * value expression.
     *
     * \param val An LLVM value.
     * \return a value expression.
     */
    ExprPtr getVariable(Value *val);
    /**
     * Return an existing (already stored in the context)  
     * value expression which has a "label" type.
     *
     * \param val An LLVM basic block label value.
     * \return a label expression, or null if it does not exist 
     * or if the type of \p val is not "label".
     */
    ExprPtr getCondVariable(Value *val);
     /**
     * Return an existing (already stored in the context)  
     * value expression which has a local variable.
     *
     * \param i An LLVM instruction in which the variable is used.
     * \param arg An LLVM value to be encoded.
     * \param pos The argument position of \p arg in \p i.
     * \return a local varibale expression.
     */
    ExprPtr getLocalVariable(Instruction *i, Value *arg, int pos);
    
    /**
     * Mark the line \p line as containing an assert function.
     */
    void setAssertCallLine(unsigned line) { 
        assertCall2lines[line] = true; 
    }
     /**
     * Return true if at the line \p line there 
     * is an assert function.
     */
    bool isAssertCall(unsigned line) { 
        return assertCall2lines[line]; 
    }

    /**
     * Return the ID of a memory allocation instruction. 
     */
    unsigned getPtrId(AllocaInst *a) {
       return ptr2id[a]; 
    }
    /**
     * Update the ID of a memory allocation instruction. 
     */
    void updateMemId(StoreInst *s) {
        memID++; 
        bb2id[s->getParent()] = memID;
    }
    /**
     * Return the ID of a memory allocation instruction 
     * at a given basic block location. 
     */
    unsigned getMemId(BasicBlock *bb) { 
        return bb2id[bb]; 
    }
    /**
     * Return the current a memory allocation ID. 
     */
    unsigned getCurrentMemId() { 
        return memID; 
    }
    /**
     * Map a memory allocation instruction to a memory ID.
     *
     * \param alloca An LLVM memory allocation instruction.
     * \param size The size of the allocated memory space.
     */
    void addPtrId(AllocaInst *alloca, uint64_t size) {
        ptr2id[alloca] = memPtr;
        memPtr = memPtr + size;
    }

    /**
     * Set the variable name which will be used by the oracle.
     * This function is only used when processing the TCAS 
     * benchmark programs.
     */
    void setTCASAssertVarName(std::string name) { 
        assertVarName = name; 
    }
    /**
     * Get the variable name which will be used by the oracle.
     * This function is only used when processing the TCAS 
     * benchmark programs.
     */
    std::string getTCASAssertVarName() { 
        return assertVarName; 
    }
    
    /**
     * Propagate pointers in the LLVM control flow graph.
     * This function is used to know what memory IDs should be used 
     * when encoding an IR containing pointers.
     * 
     * \param bb An LLVM basic block to populate.
     * \return null when there is no predecessor basic blocks,
     *         null when there is a single predecessor basic block, or
     *         return an expression that represente a phi node which 
     *         selects pointers from predecessor basic blocks.
     */
    ExprPtr propagatePointers(BasicBlock *bb);
    /**
     * Propagate pointers in the LLVM control flow graph.
     * This function is used to know what memory IDs should be used 
     * when encoding an IR containing pointers.
     * As opposed to the other propagatePointers function, 
     * this function is used to propage pointers when executing 
     * the target program in the concolic execution module. 
     * Therefore, only pointers from the previously executed 
     * basic block must be propagated. Hence, phi nodes are 
     * not necessary here. 
     *
     * \param bb An LLVM basic block to populate.
     * \param predbb An LLVM basic block which was previously executed.
     */
    void propagatePointers(BasicBlock *bb, BasicBlock *predbb);

private:
    /**
     * Dump to the standard output the current context.
     */
    void dump();

};

#endif // _CONTEXT_H