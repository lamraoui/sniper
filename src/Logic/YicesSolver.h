/**
 * YicesSolver.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/07/05
 * @copyright : NII 2013
 */

#ifndef _YICESSOLVER_H
#define _YICESSOLVER_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include "yices_c.h"

#include "IWPMaxSATSolver.h"
#include "Expression.h"

//============================================================================
class YicesSolver : public IWPMaxSATSolver {
        
    
private:
    unsigned round;
    yices_context ctx;
    yices_type int8_ty, int32_ty, int64_ty;
    yices_type int32toint32_ty;
    yices_expr intdiv_op;
    yices_expr intmod_op;
    yices_model model;
    std::map<ExprPtr, assertion_id> expr2ids;
    std::map<ExprPtr, yices_expr> expr2yexpr;
    
public:
    YicesSolver();
    ~YicesSolver();
    
    // ISolver
    virtual int         getValue(std::string name);
    int                 getValueOrZero(std::string name);
    int                 getBoolValue(std::string name); 
    virtual int         check(Formula *formula);
    virtual std::string getModel();
    
    // IWPMaxSATSolver
    virtual int         maxSat(Formula *formula);
    virtual std::vector<ExprPtr> getUnsatExpressions();
    virtual std::vector<ExprPtr> getSatExpressions();
    virtual double      getCostAsDouble();
    virtual void        setHard(Formula *formula, ExprPtr e);
    
    // TMP
    int getValue(std::string name, bool &error);
    int check();
    int maxSat();
    void addToContext(ExprPtr e);
    void addToContext(Formula *f);
    assertion_id addToContextRetractable(ExprPtr e);
    void retractFromContext(assertion_id i);   
    void push();
    void pop();
    void init();
    void clean();
    
private:
    yices_expr makeYicesExpression(ExprPtr e);
    
      
}; 
//============================================================================

#endif
