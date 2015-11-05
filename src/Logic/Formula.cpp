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
    std::vector<ExprPtr> E = f->getExprs();
    this->exprs.insert(this->exprs.end(), E.begin(), E.end());
}

// Alloc a new empty formula
FormulaPtr Formula::make() {
    return std::make_shared<Formula>();
}

// Insert an expression
void Formula::add(ExprPtr e) {
    this->exprs.push_back(e);
}

// Merge the formula f into this one
void Formula::add(FormulaPtr f) {
    std::vector<ExprPtr> E = f->getExprs();
    this->exprs.insert(this->exprs.end(), E.begin(), E.end());
}

// Insert a set of expressions
void Formula::add(std::set<ExprPtr> E) {
    std::copy(E.begin(), E.end(), std::back_inserter(this->exprs));
}

void Formula::remove(ExprPtr e) {
    exprs.erase(std::remove(this->exprs.begin(), this->exprs.end(), e),
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
        if ((it+1)!=exprs.end()) {
            std::cout << ", ";
        }
    }
    std::cout << "}";
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
    this->formulas.push_back(f);
}

void SetOfFormulas::add(std::vector<FormulaPtr> F) {
    this->formulas.insert(formulas.end(), F.begin(), F.end());
}

void SetOfFormulas::add(SetOfFormulasPtr F) {
    assert(F.get()==this && "Self copy!");
    std::vector<FormulaPtr> formulas2 = F->getFormulas();
    this->formulas.insert(this->formulas.end(),
                          formulas2.begin(), formulas2.end());
}

std::vector<FormulaPtr> SetOfFormulas::getFormulas() {
    return this->formulas;
}

FormulaPtr SetOfFormulas::getAt(unsigned i) {
    assert((i<0 || i>=formulas.size()) && "Out of bound");
    return this->formulas[i];
}

unsigned SetOfFormulas::size() {
    return this->formulas.size();
}

// Return true if it contains Formulas, false otherwise
bool SetOfFormulas::empty() {
    return this->formulas.empty();
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

// Transform a vector of expressions (MCS)
// into a set of expressions (MCS).
// Copy vv to M
/*void copy(std::vector<std::vector<unsigned> > &vv, MCSesPtr M) {
 UVVec::const_iterator itv;
 for (itv=vv.begin(); itv!=vv.end(); ++itv) {
 // Copy the MCS (vector) into a MCS (set)
 std::set<unsigned> subset((*itv).begin(), (*itv).end());
 M.add(subset);
 }
 }*/


// Remove all subset doublons
// {{x,y}, {a,b}, {x,y}} -> {{a,b}}
void SetOfFormulas::removeDoublons() {
    //std::sort(exprs.begin(), exprs.end());
    //exprs.erase(std::unique(exprs.begin(),
    //exprs.end()), exprs.end());
}

// Remove all subsets of subsets
// {{x}, {a,b}, {x,y}} -> {{a,b},{x,y}}
void SetOfFormulas::removeSubsets() {
    /*std::vector<std::set<unsigned> >::iterator itm;
    for (itm=exprs.begin(); itm!=exprs.end();) {
        std::set<unsigned> s(*itm);
        unsigned i = 0;
        bool isIn = false;
        while (i<v.size()) {
            if (s!=v[i]) {
                isIn = std::includes(v[i].begin(), v[i].end(), s.begin(),s.end());
                if (isIn) {
                    break;
                }
            }
            i++;
        }
        if (isIn) {
            itm = exprs.erase(itm);
        } else {
            ++itm;
        }
    }*/
}

// Return true if f1 is equal to f2, false otherwise
bool Formula::operator==(const Formula &other) const {
    return (this->exprs == other.exprs);
}

// Return true if f1 is not equal to f2, false otherwise
bool Formula::operator!=(const Formula &other) const {
    return !(*this == other);
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
