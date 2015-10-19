/**
 * MCS.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/01
 * @copyright : NII 2013
 */


#ifndef _MCS_H
#define _MCS_H

#include <vector>
#include <map>
#include <sstream>

#include "IWPMaxSATSolver.h"
#include "YicesSolver.h"
#include "Formula.h"
#include "Expression.h"


//============================================================================
class MCS {
    
private:
    std::vector<Expression*> exprs;
    
public:
    MCS();
    MCS(std::vector<Expression*> _exprs);
    ~MCS();
    std::vector<unsigned> getLineNumbers();
    void add(Expression *e);
    void dump();
    void dumpLine();
    
   
};
//============================================================================

#endif
