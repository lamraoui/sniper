/**
 * Formula.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/07/05
 * @copyright : NII 2013
 */

#ifndef _FORMULA_H
#define _FORMULA_H

#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "llvm/IR/BasicBlock.h"
#include "llvm/DebugInfo.h"
#include "Expression.h"

class Formula;
class SetOfFormulas;
typedef std::shared_ptr<Formula> FormulaPtr;
typedef std::shared_ptr<SetOfFormulas> SetOfFormulasPtr;

//============================================================================
class Formula {
    
private:
    std::vector<ExprPtr> exprs;
    
public:
    Formula() { }
    Formula(Formula *f);
    Formula(std::vector<ExprPtr> _exprs) : exprs(_exprs) { }
    ~Formula() { }
    
    Formula(const Formula& other) : exprs(other.exprs) { }
    Formula& operator=(const Formula& other) {
        if (this == &other)
            return *this;
        Formula tmp(other);
        std::swap(exprs, tmp.exprs);
        return *this;
    }
    Formula* clone() {
        return new Formula(exprs);
    }
    
    static FormulaPtr make();
    
    void        add(ExprPtr e);
    void        add(std::vector<ExprPtr> E);
    void        remove(ExprPtr e);
    unsigned    size();
    bool        empty();
    
    std::vector<ExprPtr>    getExprs(llvm::BasicBlock *bb);
    std::vector<ExprPtr>    getSoftExprs(llvm::BasicBlock *bb);
    std::vector<ExprPtr>    getExprs();
    std::vector<unsigned>   getLineNumbers();
    
    unsigned getNbHardExpr();
    unsigned getNbSoftExpr();
    
    void dump();
    void lightDump();
    void dumpLineNumber();
    
    bool operator==(const Formula &other) const;
    bool operator!=(const Formula &other) const;
    
    friend std::ostream& operator<<(std::ostream& os, const FormulaPtr f);
    
}; 
//============================================================================


//============================================================================
class SetOfFormulas {
    
private:
    std::vector<FormulaPtr> formulas;
    
public:
    SetOfFormulas() { }
    SetOfFormulas(std::vector<FormulaPtr> _formulas): formulas(_formulas) { }
    ~SetOfFormulas() { }
    
    static SetOfFormulasPtr make();
    
    void                    add(FormulaPtr M);
    void                    add(std::vector<FormulaPtr> formulas);
    void                    add(SetOfFormulasPtr F);
    std::vector<FormulaPtr> getFormulas();
    FormulaPtr              getAt(unsigned i);
    unsigned                size();
    bool                    empty();
    
    void removeDoublons();
    void removeSubsets();
    
    double getCodeSizeReduction(unsigned totalNbLine);
    
    friend std::ostream& operator<<(std::ostream& os, const SetOfFormulasPtr f);
    friend std::ostream& operator<<(std::ostream& os,
                                    const std::vector<SetOfFormulasPtr> f);

};
//============================================================================
#endif
