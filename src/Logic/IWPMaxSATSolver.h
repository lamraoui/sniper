/**
 * IWPMaxSATSolver.h
 *
 * Interface 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/07/05
 * @copyright : NII 2013
 */

#ifndef _IWPMAXSATSOLVER_H
#define _IWPMAXSATSOLVER_H

#include <vector>
#include <set>

#include "ISolver.h"
#include "Formula.h"
#include "Expression.h"


//============================================================================
class IWPMaxSATSolver : public ISolver {
        
public:
    virtual ~IWPMaxSATSolver() { }
    virtual int         maxSat(Formula *formula)                = 0;
    virtual std::vector<ExprPtr> getUnsatExpressions()          = 0;
    virtual std::vector<ExprPtr> getSatExpressions()            = 0;
    virtual double      getCostAsDouble()                       = 0;
    
};
//============================================================================

#endif
