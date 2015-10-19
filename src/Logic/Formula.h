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

#include "llvm/IR/BasicBlock.h"
#include "llvm/DebugInfo.h"
#include "Expression.h"


//============================================================================
class Formula {
        
    
private:
    std::vector<ExprPtr> exprs;
    std::vector<ExprPtr> oldSoftExprs;
    std::vector<ExprPtr> notPostConditions;
    std::vector<ExprPtr> postConditions;
    std::vector<ExprPtr> loopAssertExprs;
    unsigned nbHardExpressions;
    unsigned nbSoftExpressions;
    unsigned totalWeight;
    unsigned currentPushPopId;
    bool lock;
    std::vector<unsigned> pushPopIds;
    
public:
    
    // TMP
    std::vector<ExprPtr> changeWeights(std::vector<ExprPtr> es, unsigned w);
    ExprPtr changeWeight(ExprPtr e, unsigned w);
    std::vector<ExprPtr> getLoopAssertExprs() { return loopAssertExprs; }
    void addLoopAssert(ExprPtr e) { 
        loopAssertExprs.push_back(e); 
    }
    //
    
    Formula();
    Formula(Formula *f);
    ~Formula();
    void assertHard(ExprPtr e);
    void assertSoft(ExprPtr e);
    void remove(ExprPtr e);
    void setHard(ExprPtr e);
    void setHard(std::set<ExprPtr> es);
    void addPostCondition(ExprPtr e);
    void addNotPostCondition(ExprPtr e);
    std::vector<ExprPtr> getPostConditions();
    std::vector<ExprPtr> getNotPostConditions();
    std::vector<ExprPtr> getExprs(llvm::BasicBlock *bb);
    std::vector<ExprPtr> getSoftExprs(llvm::BasicBlock *bb);
    std::vector<ExprPtr> getExprs();
    unsigned getNbHardExpr() { return nbHardExpressions; }
    unsigned getNbSoftExpr() { return nbSoftExpressions; }
    unsigned getTotalWeight() { return totalWeight; }
    void setAsLocked() { lock = true; }
    void push();
    void pop();
    void dump();

}; 
//============================================================================

#endif
