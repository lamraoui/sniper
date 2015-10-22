/**
 * IterationAlgorithm.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/07/05
 * @copyright : NII 2013
 */

#ifndef _ITERATIONALGORITHM_H
#define _ITERATIONALGORITHM_H

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
//#include <dispatch/dispatch.h>

#include "Options.h"
#include "Utils/HittingSet.h"
#include "Profile/ProgramTrace.h"
#include "Profile/ProgramProfile.h"
#include "Logic/GenPath.h"
#include "Logic/IWPMaxSATSolver.h"
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"
#include "Logic/Combine.h"


using namespace llvm;


//============================================================================
class IterationAlgorithm {
    
private:
    Function *targetFun;
    IWPMaxSATSolver *solver; 
    bool hasArgv;
    Options *options;
    
public:
    IterationAlgorithm(Function *_targetFun, IWPMaxSATSolver *_solver,
                       bool _hasArgv, Options *_options) :
                       targetFun(_targetFun), solver(_solver),
                       hasArgv(_hasArgv), options(_options) { }
    ~IterationAlgorithm() { }
    
    void run(Formula *TF, ProgramProfile *prof, Combine::Method combineMethod);
    void run_dynamic(Formula *TF, ProgramProfile *prof,
                     Combine::Method combineMethod);
    
    std::vector<SetOfFormulasPtr> allDiagnosis(Formula *TF,
                                               std::vector<ProgramTrace*> traces,
                                               YicesSolver *yices);
    SetOfFormulasPtr allMinMCS2(YicesSolver *yices,
                     std::vector<BoolVarExprPtr> &AV,
                     std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    
    // Diagnosis computation methods
    SetOfFormulasPtr allMinMCS(IWPMaxSATSolver *solver, Formula *WF,
                    std::vector<BoolVarExprPtr> &AV,
                    std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    SetOfFormulasPtr allMinMCS_PP(YicesSolver *yices,
                       std::vector<BoolVarExprPtr> &AV,
                       std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    SetOfFormulasPtr allMinMCS_AR(YicesSolver *yices,
                       std::vector<BoolVarExprPtr> &AV,
                       std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    
    // TCAS
   /* void runTCAS(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    void runTCAS_PWU(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    void runTCAS_PP(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    void runTCAS_PP_PWU(std::vector<ExprPtr> TCExprs,std::vector<ExprPtr> GOExprs);
    void runTCAS_AR(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    */
private:
    SetOfFormulasPtr avToClauses(SetOfFormulasPtr M,
                                 std::map<BoolVarExprPtr, ExprPtr> AVMap);
    // For debugging
    void checkControlFlow(YicesSolver *solver);
    int  getBlockTransVal(YicesSolver *s, BasicBlock *bb1, BasicBlock *bb2);
    void dumpTransValues(YicesSolver *solver);
    
    
};
//============================================================================

#endif
