/**
 * MCS.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/09/14
 * @copyright : NII 2013
 */

#include "MCS.h"


MCS::MCS() {
    
}

MCS::MCS(std::vector<Expression*> _exprs): exprs(_exprs) {
    
}

std::vector<unsigned> MCS::getLineNumbers() {
    std::vector<unsigned> lines;
    std::vector<Expression*>::iterator it;
    for(it=exprs.begin(); it!=exprs.end(); it++) {
        Expression *e = *it;
        lines.push_back(e->getLine());
    }
    return lines;
}

void MCS::add(Expression *e) {
    exprs.push_back(e);
}


void MCS::dump() {
    std::cout << "{ ";
    std::vector<Expression*>::iterator it;
    for (it=exprs.begin(); it!=exprs.end(); ++it) {
        Expression *e = *it;
        e->dump();
        std::cout << " ";
    }
    std::cout << "}";
}


void MCS::dumpLine() {
    std::cout << "{";
    std::vector<Expression*>::iterator it;
    for (it=exprs.begin(); it!=exprs.end(); ++it) {
        Expression *e = *it;
        std::cout << e->getLine();
        if ((it+1)!=exprs.end()) {
            std::cout << ", ";
        }
    }
    std::cout << "}";
}

MCS::~MCS() {

}