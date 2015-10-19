/**
 * ISolver.h
 *
 * Interface 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/07/05
 * @copyright : NII 2013
 */

#ifndef _ISOLVER_H
#define _ISOLVER_H

#include <string>
#include "Formula.h"

//============================================================================
class ISolver {
    
public:
    virtual ~ISolver() { }
    virtual int         check(Formula *formula)          = 0; 
    virtual int         getValue(std::string name)       = 0;
    virtual int         getValueOrZero(std::string name) = 0;
    virtual int         getBoolValue(std::string name)   = 0; 
    virtual std::string getModel()                       = 0;
    
};
//============================================================================

#endif
