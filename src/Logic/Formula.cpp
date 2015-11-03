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
    // OK?
    this->currentPushPopId = 0;
    this->lock = f->lock; 
    // pushPopIds
    // currentPushPopId
}

// Alloc a new empty formula
FormulaPtr Formula::make() {
    return std::make_shared<Formula>();
}

void Formula::assertHard(ExprPtr e) {
    e->setHard();
    exprs.push_back(e);
    currentPushPopId = exprs.size();
    if (lock) {
        e->invalidate();
    }
}

void Formula::assertSoft(ExprPtr e) {
    e->setSoft();
    if (e && e->getInstruction()) {
        if (llvm::MDNode *N = e->getInstruction()->getMetadata("dbg")) { 
            llvm::DILocation Loc(N); 
            unsigned l = Loc.getLineNumber();
            //StringRef File = Loc.getFilename();
            //StringRef Dir = Loc.getDirectory();
            e->setLine(l);
        } else {
            e->setLine(0);
        }
    }
    exprs.push_back(e);
    currentPushPopId = exprs.size();
    if (lock) {
        e->invalidate();
    }
}

void Formula::setHard(ExprPtr e) {
    e->setHard();
    // TODO : multi push/pop compliant
    oldSoftExprs.push_back(e);
}

void Formula::setHard(std::set<ExprPtr> es) {
    std::set<ExprPtr>::iterator it;
    for (it=es.begin(); it!=es.end(); it++) {
        ExprPtr e = *it;
        setHard(e);
    }
}

void Formula::remove(ExprPtr e) {
    exprs.erase(std::remove(exprs.begin(), exprs.end(), e), exprs.end());
    currentPushPopId = exprs.size();
    if (lock) {
        e->invalidate();
    }
}

std::vector<ExprPtr> Formula::getExprs(llvm::BasicBlock *bb) {
    std::vector<ExprPtr> bbExpr;
    std::vector<ExprPtr>::iterator it;
    for(it = exprs.begin(); it != exprs.end(); ++it) {
        ExprPtr e = *it;
        if(e->getBB()==bb) {
            bbExpr.push_back(e);
        }
    }
    return bbExpr;
}

std::vector<ExprPtr> Formula::getSoftExprs(llvm::BasicBlock *bb) {
    std::vector<ExprPtr> bbExpr;
    std::vector<ExprPtr>::iterator it;
    for(it = exprs.begin(); it != exprs.end(); ++it) {
        ExprPtr e = *it;
        if(e->isSoft() && e->getBB()!=NULL && e->getBB()==bb) {
            bbExpr.push_back(e);
        }
    }
    return bbExpr;
}

std::vector<ExprPtr> Formula::getExprs() {
    return this->exprs;
}

// Merge the formula f into this one
void Formula::add(FormulaPtr f) {
    std::vector<ExprPtr> E = f->getExprs();
    this->exprs.insert(exprs.end(), E.begin(), E.end());
    // TODO: Push/pop
}

void Formula::push() {
    pushPopIds.push_back(currentPushPopId);
}

void Formula::pop() {
    // Nothing to pop
    if (currentPushPopId==0 || pushPopIds.empty()) {
        std::cout << "nothing to pop...\n";
        return;
    }
    // Update push/pop IDs
    currentPushPopId = pushPopIds.back();
    pushPopIds.pop_back();
    // Remove from the formula all expressions
    // that were asserted after the last push 
    int n = exprs.size() - currentPushPopId;
    for(int i=0; i<n; i++) {
        ExprPtr e = exprs.back();
        exprs.pop_back();
    }
    std::vector<ExprPtr>::iterator it;
    for(ExprPtr e : oldSoftExprs) {
        e->setSoft();
    }
    oldSoftExprs.clear();
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

// Insert an expression
void Formula::add(ExprPtr e) {
    exprs.push_back(e);
}

// Insert a set of expressions
void Formula::add(std::set<ExprPtr> E) {
    std::copy(E.begin(), E.end(), std::back_inserter(exprs));
}

unsigned Formula::size() {
    return exprs.size();
}

// Return true if the MCS contains expressions, false otherwise
bool Formula::empty() {
    return exprs.empty();
}

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
