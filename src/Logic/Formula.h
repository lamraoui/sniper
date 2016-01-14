/**
 * \file Formula.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   12 January 2015
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

/**
 * \class Formula
 *
 * \brief This class is a container for expressions.
 *
 * A formula does not contains duplicate expressions.
 */
class Formula {
    
private:
    /**
     * Expressions
     */
    std::vector<ExprPtr> exprs;
    
public:
    Formula() { }
    Formula(Formula *f);
    Formula(const Formula& other) : exprs(other.exprs) { }
    Formula(std::vector<ExprPtr> _exprs) : exprs(_exprs) { }
    ~Formula() { }
    
    /**
     * \brief Copy operator
     *
     * \param other A formula to be copied.
     * \return A copy of \a other.
     */
    Formula& operator=(const Formula& other) {
        if (this == &other)
            return *this;
        Formula tmp(other);
        std::swap(exprs, tmp.exprs);
        return *this;
    }
    
    /**
     * \brief Clone operator
     *
     * \return A copy of \a this.
     */
    Formula* clone() {
        return new Formula(exprs);
    }
    
    /**
     * \brief Make an empty formula.
     *
     * \return A formula.
     */
    static FormulaPtr make();
    
    /**
     * \brief Add an expression.
     *
     * The expression \a e is added only if
     * it is not already in the formula.
     *
     * \param e An expression.
     */
    void add(ExprPtr e);
    
    /**
     * \brief Add a vector of expressions.
     *
     * The expressions in \a E are added only if
     * they are not already in the formula.
     *
     * \param E A vector of expressions.
     */
    void add(std::vector<ExprPtr> E);
    
    /**
     * \brief Remove \a e from the formula.
     *
     * The expression \a e has to be
     * previously added to the formula.
     *
     * \param e An expression.
     */
    void remove(ExprPtr e);
    
    /**
     * \brief Return the size (number of expressions) 
     *        of the formula.
     *
     * \return a positive or null size.
     */
    unsigned size();
    
    /**
     * \brief Check if the formula is empty.
     *
     * \return true if there is no expressions in the formula, 
     *         false otherwise.
     */
    bool empty();
    
    /**
     * \brief Return the expressions of the formula that
     *        are associated to the basicblock \a bb.
     *
     * An expression is associated to a basicblock \a bb
     * if this expression was encoded from an
     * LLVM instruction that is in \a bb.
     *
     * \param bb An LLVM basicblock.
     * \return A vector of expressions.
     */
    std::vector<ExprPtr> getExprs(llvm::BasicBlock *bb);
    
    /**
     * \brief Return the soft expressions of the formula that
     *        are associated to the basicblock \a bb.
     *
     * An expression is associated to a basicblock \a bb
     * if this expression was encoded from an
     * LLVM instruction that is in \a bb.
     *
     * \param bb An LLVM basicblock.
     * \return A vector of soft expressions.
     */
    std::vector<ExprPtr> getSoftExprs(llvm::BasicBlock *bb);
    
    /**
     * \brief Return all expressions of the formula.
     *
     * \return A vector of expressions.
     */
    std::vector<ExprPtr> getExprs();
    
    /**
     * \brief Return the line number associated to all the 
     *        expressions of the formula.
     *
     * The line number of an expresssion comes from
     * the line number of its associated LLVM instruction.
     *
     * \return A vector of line number 
     *         (0 for expressions without a line number).
     */
    std::vector<unsigned> getLineNumbers();
    
    /**
     * \brief Return the number of hard expressions of the formula.
     *
     * \return The number of hard expressions.
     */
    unsigned getNbHardExpr();
    
    /**
     * \brief Return the number of soft expressions of the formula.
     *
     * \return The number of soft expressions.
     */
    unsigned getNbSoftExpr();
    
    /**
     * \brief Dump all the formula information to the standard output.
     */
    void dump();
    
    /**
     * \brief Dump some of the formula information to the standard output.
     */
    void lightDump();
    
    /**
     * \brief Dump the expression line numbers to the standard output.
     */
    void dumpLineNumber();
    
    /**
     * \brief Check if the formula \a other is a subset of this formula.
     *
     * \param other A formula.
     * \return true if \a other is a subset, false otherwise.
     */
    bool contains(const FormulaPtr other);
    
    /**
     * \brief Check if the formula \a other is equal to this formula.
     *
     * \param other A formula.
     * \return true if \a other is equal, false otherwise.
     */
    bool operator==(const Formula &other) const;
    
    /**
     * \brief Check if the formula \a other is not equal to this formula.
     *
     * \param other A formula.
     * \return true if \a other is not equal, false otherwise.
     */
    bool operator!=(const Formula &other) const;
    
    /**
     * \brief Check if the formula \a f1 is equal to the formula \a f2.
     *
     * \param f1 A formula.
     * \param f2 A formula.
     * \return true if \a f1 and \a f2 are equal, false otherwise.
     */
    friend bool operator== (FormulaPtr f1, FormulaPtr f2);
    
    /**
     * \brief Check if the formula \a f1 is not equal to the formula \a f2.
     *
     * \param f1 A formula.
     * \param f2 A formula.
     * \return true if \a f1 and \a f2 are not equal, false otherwise.
     */
    friend bool operator!= (FormulaPtr f1, FormulaPtr f2);
    
    /**
     * \brief Utility to dump a formula to the standard ouput.
     *
     * \detail Usage: std::out << \a f << std::endl;
     */
    friend std::ostream& operator<<(std::ostream& os, const FormulaPtr f);
    
};


/**
 * \class SetOfFormulas
 *
 * \brief This class is a container for formulas.
 *
 * A setOfFormula does not contains duplicate formulas or subsets.
 */
class SetOfFormulas {
    
private:
    /**
     * Formulas
     */
    std::vector<FormulaPtr> formulas;
    
public:
    SetOfFormulas() { }
    SetOfFormulas(std::vector<FormulaPtr> _formulas): formulas(_formulas) { }
    ~SetOfFormulas() { }
    
    /**
     * \brief Make an empty set of formulas.
     *
     * \return An empty set of formulas.
     */
    static SetOfFormulasPtr make();
    
    /**
     * \brief Add a formula.
     *
     * The formula \a M is added only if
     * (1) it is not already in the formula,
     * (2) it is not a subset of an existing formula,
     * If \a M is added, supersets are removed.
     *
     * \param M A formula.
     */
    void add(FormulaPtr M);
    
    /**
     * \brief A vector of formulas.
     *
     * The formula in \a formulas are added only if
     * (1) they are not already in the formula,
     * (2) they are not subsets of existing formulas,
     * If formulas of \a formulas are added, supersets are removed.
     *
     * \param formulas A vector of formulas.
     */
    void add(std::vector<FormulaPtr> formulas);
    
    /**
     * \brief Return the size (number of formulas)
     *        of the set of formulas.
     *
     * \return a positive or null size.
     */
    unsigned size();
    
    /**
     * \brief Check if the set of formula is empty.
     *
     * \return true if there is no formulas in the set of formulas,
     *         false otherwise.
     */
    bool empty();
    
    /**
     * \brief Return all formulas of the set of formulas.
     *
     * \return A vector of formulas.
     */
    std::vector<FormulaPtr> getFormulas();
    
    /**
     * \brief Return the formula at index \a i.
     *
     * \param i An index.
     * \return A formula.
     */
    FormulaPtr getAt(unsigned i);
    
    /**
     * \brief Return the CSR for this set of formulas.
     *
     * CSR (code size reduction) is the ratio of fault compare to 
     * the total number of lines in a program.
     * If this set of formulas were encoded from a program P,
     * it is possible to calculate a CSR for P.
     *
     * \param totalNbLine The total number of lines of the input program.
     * \return a CSR.
     */
    double getCodeSizeReduction(unsigned totalNbLine);
    
    /**
     * \brief Utility to dump a set of formulas to the standard ouput.
     *
     * Usage: std::out << \a f << std::endl;
     */
    friend std::ostream& operator<<(std::ostream& os, const SetOfFormulasPtr f);
    
    /**
     * \brief Utility to dump a vector of set of formulas to the standard ouput.
     *
     * Usage: std::out << \a f << std::endl;
     */
    friend std::ostream& operator<<(std::ostream& os,
                                    const std::vector<SetOfFormulasPtr> f);

};
#endif // _FORMULA_H
