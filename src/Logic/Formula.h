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

// Pretty printer for Formula and SetOfFormulas
std::ostream& operator<<(std::ostream& os, const FormulaPtr f);
std::ostream& operator<<(std::ostream& os, const SetOfFormulasPtr f);
std::ostream& operator<<(std::ostream& os, const std::vector<SetOfFormulasPtr> f);


//============================================================================
class Formula {
    
private:
    std::vector<ExprPtr> exprs;
    std::vector<ExprPtr> oldSoftExprs;
    
    unsigned nbHardExpressions;
    unsigned nbSoftExpressions;
    
    unsigned currentPushPopId;
    bool     lock;
    std::vector<unsigned> pushPopIds;
    
public:
    Formula() : nbHardExpressions(0), nbSoftExpressions(0),
                currentPushPopId(0), lock(false) { }
    Formula(Formula *f);
    Formula(std::vector<ExprPtr> _exprs): exprs(_exprs) { }
    ~Formula() { }
    
    static FormulaPtr make();
    
    void        add(ExprPtr e);
    void        add(FormulaPtr f);
    void        add(std::set<ExprPtr> E);
    void        remove(ExprPtr e);
    unsigned    size();
    bool        empty();
    
    void assertHard(ExprPtr e);
    void assertSoft(ExprPtr e);
    void setHard(ExprPtr e);
    void setHard(std::set<ExprPtr> es);
    
    std::vector<ExprPtr>    getExprs(llvm::BasicBlock *bb);
    std::vector<ExprPtr>    getSoftExprs(llvm::BasicBlock *bb);
    std::vector<ExprPtr>    getExprs();
    std::vector<unsigned>   getLineNumbers();
    
    unsigned getNbHardExpr() { return nbHardExpressions; }
    unsigned getNbSoftExpr() { return nbSoftExpressions; }
    
    void setAsLocked() { lock = true; }
    void push();
    void pop();
    
    void dump();
    void lightDump();
    void dumpLineNumber();
    
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
    //void copy(std::vector<std::vector<unsigned> > &vv, MCSesPtr M);
    
    double getCodeSizeReduction(unsigned totalNbLine);
    
};
//============================================================================
#endif
