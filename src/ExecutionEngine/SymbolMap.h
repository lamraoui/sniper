/**
 * \file SymbolMap.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   15 February 2016
 */

#ifndef _SYMBOLMAP_H
#define _SYMBOLMAP_H

#include <string>
#include <iostream>
#include <map>
#include <memory>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

#include "Symbol.h"

using namespace llvm;

/**
 * \class SymbolMap
 *
 * This class maps LLVM values to symbols.
 * This class is a part of the concolic execution.
 */
class SymbolMap {
    
private:
    /**
     * Map LLVM values to symbols.
     */
    std::map<Value*, SymbolPtr> memory;

public:
    SymbolMap() { }
    /**
     * Destructor.
     */
    ~SymbolMap() {
        memory.clear();
    }

    /**
     * Create and add a new symbol from an LLVM input value.
     *
     * \param v An LLVM value (function argument) to map.
     * \return A new symbol representating the value \p v.
     */
    SymbolPtr addInput(Value *v);
    /**
     * Create and add a new symbol from an LLVM value.
     *
     * \param v An LLVM value to map.
     * \return A new symbol representating the value \p v.
     */
    SymbolPtr create(Value *v);
    /**
     * Return the symbol associated to the LLVM value \p v.
     *
     * \param v An LLVM value.
     * \return The symbol that represents the value \p v.
     */
    SymbolPtr get(Value *v);
    /**
     * Remove the entry <s,v> from the map, with \p s the symbol of \p v.
     * 
     * \param v An LLVM value.
     */
    void remove(Value *v);
    /**
     * Replace the entry <s,v> in the map by <s,v1>.
     * 
     * \param v An LLVM value.
     * \param v1 An LLVM value.
     */
    void update(Value *v, Value *v1);
    /**
     * Replace the entry <s0,v> in the map by <s,v>.
     * 
     * \param v An LLVM value.
     * \param s A symbol.
     */
    void update(Value *v, SymbolPtr s);
    /**
     * Return true if there exist an entry for \p v.
     */
    bool contains(Value *v);
    /**
     * Remove all entries in the map. 
     */
    void clear();
    /**
     * Dump to the standard output the map entries.
     */
    void dump();
    
    /**
     * Create a new symbol s(v) representating a binary operation (add, sub, ...).
     * s(v)= s1 op s2.
     *
     * \param v An LLVM value (binary instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createBinaryOp(Value *v, SymbolPtr s1, SymbolPtr s2);
    /**
     * Create a new symbol s(v) representating a binary operation (add, sub, ...).
     * s(v)= s1 op s(v2).
     *
     * \param v An LLVM value (binary instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param v2 An LLVM value (second argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createBinaryOp(Value *v, SymbolPtr s1, Value *v2);
    /**
     * Create a new symbol s(v) representating a binary operation (add, sub, ...).
     * s(v)= s(v1) op s2.
     *
     * \param v An LLVM value (binary instruction).
     * \param v1 An LLVM value (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createBinaryOp(Value *v, Value *v1,  SymbolPtr s2);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s2 : s3
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \param s3 A symbol (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr s1, SymbolPtr s2, SymbolPtr s3);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s(v2) : s3
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param v2 An LLVM value (second argument of the operation).
     * \param s3 A symbol (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr s1, Value *v2, SymbolPtr s3);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s2 : s(v3)
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \param v3 An LLVM value (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, SymbolPtr s2, Value *v3);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s(v2) : s(v3)
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param v2 An LLVM value (second argument of the operation).
     * \param v3 An LLVM value (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr s1, Value *v2, Value *v3);
    /**
     * Create a new symbol s(v) representating a cast operation.
     * s(v)= cast s1
     *
     * \param v An LLVM value (cast instruction).
     * \param s1 A symbol (argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createCastOp(Value *v, SymbolPtr s1);
    /**
     * Create a new symbol p(ptr) representating a get pointer operation.
     * p(ptr) = gep *a, s1
     * Limitation: only support integer values (*a cannot be symbolic)
     *
     * \param ptr An LLVM value (pointer).
     * \param a An LLVM value (base address).
     * \param s1 A symbol (address offset).
     * \return A new symbol.
     */
    SymbolPtr createGepOp(Value *ptr, Value *a, SymbolPtr s1);
    
};

#endif // _SYMBOLMAP_H
