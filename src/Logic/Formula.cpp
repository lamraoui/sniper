/**
 * Formula.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/07/05
 * @copyright : NII 2013
 */

#include "Formula.h"


// Clone
Formula::Formula(Formula *f) {
    this->exprs = f->getExprs();
}

// Alloc a new empty formula
FormulaPtr Formula::make() {
    return std::make_shared<Formula>();
}

// Insert an expression
void Formula::add(ExprPtr e) {
    auto it  = std::find_if(this->exprs.begin(), this->exprs.end(),
                 [e](ExprPtr e2) -> bool { return e2 == e; });
    // If e is not already in exprs, add it
    if (it==this->exprs.end()) {
        this->exprs.push_back(e);
    }
}

// Insert a set of expressions
void Formula::add(std::vector<ExprPtr> E) {
    //std::copy(E.begin(), E.end(), std::back_inserter(this->exprs));
    for (ExprPtr e : E) {
        add(e);
    }
}

void Formula::remove(ExprPtr e) {
    this->exprs.erase(std::remove(this->exprs.begin(), this->exprs.end(), e),
                      this->exprs.end());
}

// Retrun the number of clauses in this formula
unsigned Formula::size() {
    return this->exprs.size();
}

// Return true if the MCS contains expressions, false otherwise
bool Formula::empty() {
    return this->exprs.empty();
}

// Return all expressions with an instruction that belongs to bb
std::vector<ExprPtr> Formula::getExprs(llvm::BasicBlock *bb) {
    assert(bb && "Exptected a basic block!");
    std::vector<ExprPtr> bbExpr;
    for(ExprPtr e : this->exprs) {
        if(e->getBB()==bb) {
            bbExpr.push_back(e);
        }
    }
    return bbExpr;
}

// Return all soft expressions with an instruction that belongs to bb
std::vector<ExprPtr> Formula::getSoftExprs(llvm::BasicBlock *bb) {
    assert(bb && "Exptected a basic block!");
    std::vector<ExprPtr> bbExpr;
    for(ExprPtr e : this->exprs) {
        if(e->isSoft() && e->getBB()==bb) {
            bbExpr.push_back(e);
        }
    }
    return bbExpr;
}

// Return all expressions
std::vector<ExprPtr> Formula::getExprs() {
    return this->exprs;
}

// Return the instruction line numbers for all expressions
std::vector<unsigned> Formula::getLineNumbers() {
    std::vector<unsigned> lines;
    for(ExprPtr e : exprs) {
        lines.push_back(e->getLine());
    }
    return lines;
}

// Return the number of hard clauses in this formula
unsigned Formula::getNbHardExpr() {
    unsigned count = 0;
    for (ExprPtr e : exprs) {
        if (e->isHard()) {
            count++;
        }
    }
    return count;
}

// Return the number of soft clauses in this formula
unsigned Formula::getNbSoftExpr() {
    unsigned count = 0;
    for (ExprPtr e : exprs) {
        if (e->isSoft()) {
            count++;
        }
    }
    return count;
}

void Formula::dump() {
    std::cout << "--------------------------------------------\n";
    std::cout << "Formula dump\n";
    std::cout << " nb hard expressions: " << getNbHardExpr() << std::endl;
    std::cout << " nb soft expressions: " << getNbSoftExpr() << std::endl;
    for (ExprPtr e : exprs) {
        e->dump();
        if (e->isHard()) {
            std::cout << " [h]";
        } else {
            std::cout << " [s]";
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------------------------------\n";
}

void Formula::lightDump() {
    std::cout << "{ ";
    for (ExprPtr e : exprs) {
        e->dump();
        std::cout << " ";
    }
    std::cout << "}";
}

void Formula::dumpLineNumber() {
    std::cout << "{";
    std::vector<ExprPtr>::iterator it;
    for (it=exprs.begin(); it!=exprs.end(); ++it) {
        ExprPtr e = *it;
        std::cout << e->getLine();
        if (std::distance(it, exprs.end())>1) {
            std::cout << ", ";
        }
    }
    std::cout << "}";
}

// Check if other is a subset of this formula
bool Formula::contains(const FormulaPtr other) {
    if (this->size()<other->size()) {
        return false;
    }
    std::vector<ExprPtr> E1 = this->exprs;
    std::vector<ExprPtr> E2 = other->getExprs();
    std::vector<ExprPtr>::const_iterator it1;
    for (it1 = E1.begin(); it1 != E1.end(); ++it1) {
        bool match = true;
        std::vector<ExprPtr>::const_iterator it2 = it1;
        std::vector<ExprPtr>::const_iterator it3;
        for (it3 = E2.begin(); it3 != E2.end(); ++it3) {
                if (it2==E1.end() || *it3!=*it2) {
                    match = false;
                    break;
                }
                it2++;
        }
        if (match) {
            return true;
        }
    }
    return false;
}

// Return true if f1 is equal to f2, false otherwise
bool Formula::operator==(const Formula &other) const {
    //assert((f1 && f2) && "Unexpected null formulas");
    return (this->exprs == other.exprs);
}

// Return true if f1 is not equal to f2, false otherwise
bool Formula::operator!=(const Formula &other) const {
    return !(*this == other);
}

// Return true if f1 is equal to f2, false otherwise
bool operator== (FormulaPtr f1, FormulaPtr f2) {
    assert((f1.get() && f2.get()) && "Unexpected null formulas");
    return (f1->exprs == f2->exprs);
}

// Return true if f1 is not equal to f2, false otherwise
bool operator!= (FormulaPtr f1, FormulaPtr f2) {
    return !(f1 == f2);
}


//============================================================================
// SetOfFormulas
//============================================================================

// Alloc a new empty set of formulas
SetOfFormulasPtr SetOfFormulas::make() {
    return std::make_shared<SetOfFormulas>();
}

// Insert a formula
void SetOfFormulas::add(FormulaPtr f) {
    // Add if A is not subset (or equal) of B
    // If A is a superset of B, remove B and add A
    bool hasSubsets   = false;
    bool hasSupersets = false;
    auto i = std::begin(formulas);
    while (i != std::end(formulas)) {
        FormulaPtr f2 = *i;
        // f2 \subset f
        if (f2->contains(f)) {
            hasSubsets = true;
            break;
        }
        // f \subset f2
        else if (f->contains(f2)) {
            i = formulas.erase(i);
            hasSupersets = true;
        } else {
            ++i;
        }
    }
    assert((hasSubsets && hasSupersets)
           && "Something is wrong with this set of formulas!");
    if (!hasSubsets) {
        this->formulas.push_back(f);
    }
}

void SetOfFormulas::add(std::vector<FormulaPtr> F) {
    for (FormulaPtr f : F) {
        add(f);
    }
}

std::vector<FormulaPtr> SetOfFormulas::getFormulas() {
    return this->formulas;
}

unsigned SetOfFormulas::size() {
    return this->formulas.size();
}

// Return true if it contains Formulas, false otherwise
bool SetOfFormulas::empty() {
    return this->formulas.empty();
}

FormulaPtr SetOfFormulas::getAt(unsigned i) {
    assert((i<0 || i>=formulas.size()) && "Out of bound");
    return this->formulas[i];
}

double SetOfFormulas::getCodeSizeReduction(unsigned totalNbLine) {
    
    std::vector<double> CSR(formulas.size());
    unsigned i = 0;
    for (FormulaPtr MCS : formulas) {
        CSR[i] = ((100.0*(double)MCS->size())/(double)totalNbLine);
        i++;
    }
    double sum = 0;
    std::vector<double>::const_iterator it2;
    for (it2=CSR.begin(); it2!=CSR.end(); ++it2) {
        double crs_i = *it2;
        sum = sum + crs_i;
    }
    return sum/(double)CSR.size();
}

// Pretty printer for Formula
std::ostream& operator<<(std::ostream& os, const FormulaPtr f) {
    os << "{";
    const std::vector<ExprPtr> E = f->getExprs();
    for(std::vector<ExprPtr>::const_iterator it = E.begin(); it != E.end(); it++) {
        os << *it;
        if (std::distance(it, E.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

// Pretty printer for SetOfFormulas
std::ostream& operator<<(std::ostream& os, const SetOfFormulasPtr f) {
    os << "{";
    const std::vector<FormulaPtr> F = f->getFormulas();
    for(std::vector<FormulaPtr>::const_iterator it = F.begin(); it != F.end(); it++) {
        os << *it;
        if (std::distance(it, F.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

// Pretty printer for a vector of SetOfFormulas
std::ostream& operator<<(std::ostream& os, const std::vector<SetOfFormulasPtr> f) {
    os << "{";
    for(std::vector<SetOfFormulasPtr>::const_iterator it = f.begin(); it != f.end(); it++) {
        os << *it;
        if (std::distance(it, f.end())>1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}
