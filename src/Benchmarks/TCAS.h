/**
 * TCAS.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/29
 * @copyright : NII 2013
 */


#ifndef _TCAS_H
#define _TCAS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set> 
#include <algorithm>

#include "Options.h"
#include "Logic/Expression.h"


//============================================================================
class TCAS {
    
private:
    Options *options;
    std::string filepath;
    std::string assertVarName;
    std::vector<ExprPtr> testCaseExprs;
    std::vector<ExprPtr> goldenOutputExprs;
    std::vector<ExprPtr> notGoldenOutputExprs;
    
public:
    TCAS(Options *_options, std::string _filepath, std::string _assertVarName);
    ~TCAS();
    void load(std::vector<ExprPtr> &TCExprs, std::vector<ExprPtr> &GOExprs);
    
private:
    bool loadTestCases(std::string filename);
    bool loadGoldenOutputs(std::string filename);
    std::vector<unsigned> 
    intersect(std::vector< std::set<unsigned> > const& sets);
};
//============================================================================

#endif
