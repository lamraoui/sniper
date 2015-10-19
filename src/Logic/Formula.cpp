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


Formula::Formula() {
    this->nbHardExpressions = 0;
    this->nbSoftExpressions = 0;
    this->totalWeight = 0;
    this->currentPushPopId = 0;
    this->lock = false;
}

// Clone
Formula::Formula(Formula *f) {
    this->nbHardExpressions = f->getNbHardExpr();
    this->nbSoftExpressions = f->getNbSoftExpr();
    this->totalWeight = f->getTotalWeight();
    this->exprs = f->getExprs();
    this->notPostConditions = f->getNotPostConditions();
    this->postConditions = f->getPostConditions();
    // OK?
    this->currentPushPopId = 0;
    this->lock = f->lock; 
    // pushPopIds
    // currentPushPopId
}

Formula::~Formula() {
    
}


void Formula::assertHard(ExprPtr e) {
    e->setHard();
    nbHardExpressions++;
    exprs.push_back(e);
    currentPushPopId = exprs.size();
    if (lock) {
        e->invalidate();
    }
}

void Formula::assertSoft(ExprPtr e) {
    e->setSoft();
    nbSoftExpressions++;
    if (e->getWeight()<=0) {
        std::cout << "error: negative or null weight!\n";
        exit(1);
    }
    totalWeight = totalWeight + e->getWeight();
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
    nbHardExpressions++;
    nbSoftExpressions--;
    totalWeight = totalWeight - e->getWeight();
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
    if (e->isSoft()) {
        nbSoftExpressions--;
        totalWeight = totalWeight - e->getWeight();
    } else {
        nbHardExpressions--;
    }
    exprs.erase(std::remove(exprs.begin(), exprs.end(), e), exprs.end());
    currentPushPopId = exprs.size();
    if (lock) {
        e->invalidate();
    }
}

void Formula::addPostCondition(ExprPtr e) {
    postConditions.push_back(e);
}

void Formula::addNotPostCondition(ExprPtr e) {
    notPostConditions.push_back(e);
}

std::vector<ExprPtr> Formula::getPostConditions() {
    return this->postConditions;
}

std::vector<ExprPtr> Formula::getNotPostConditions() {
    return this->notPostConditions;
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

// TODO
std::vector<ExprPtr> Formula::changeWeights(std::vector<ExprPtr> es, 
                                                    unsigned w) {
    std::cout << "Formula: changeWeights!\n";
    exit(1);
}

// TODO
ExprPtr Formula::changeWeight(ExprPtr e, unsigned w) {
    std::cout << "Formula: changeWeight!\n";
    exit(1);
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
        // Update formula information
        if (e->isHard()) {
            nbHardExpressions--;
        } else {
            nbSoftExpressions--;
            totalWeight = totalWeight - e->getWeight();
        }     
        exprs.pop_back();
    }
    std::vector<ExprPtr>::iterator it;
    for(it = oldSoftExprs.begin(); it != oldSoftExprs.end(); ++it) {
        ExprPtr e = *it;
        nbHardExpressions--;
        nbSoftExpressions++;
        totalWeight = totalWeight + e->getWeight();
        e->setSoft();
    }
    oldSoftExprs.clear();
}

void Formula::dump() {
    std::cout << "--------------------------------------------\n";
    std::cout << "Formula dump\n";
    std::cout << " nb hard expressions: " << nbHardExpressions << std::endl;
    std::cout << " nb soft expressions: " << nbSoftExpressions << std::endl;
    std::cout << " total weight       : " << totalWeight << std::endl;
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