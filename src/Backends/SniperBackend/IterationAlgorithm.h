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
#include "Logic/MCS.h"


using namespace llvm;

// Types to hold vector-of-ints (Vi) and vector-of-vector-of-ints (Vvi)
typedef std::vector<unsigned> UVec;
typedef std::vector<UVec>     UVVec;
typedef std::vector<UVVec>    UVVVec;
// Types to hold set-of-ints (Su) and vector-of-set-of-ints (Vsu)
typedef std::set<unsigned> USet;
typedef std::vector<USet>  USVec;
typedef std::vector<USVec> USVVec;
// Pretty printer for vectors and sets
std::ostream& operator<<(std::ostream& os, const UVec& v);
std::ostream& operator<<(std::ostream& os, const UVVec& vv);
std::ostream& operator<<(std::ostream& os, const UVVVec& vvv);
std::ostream& operator<<(std::ostream& os, const USet& s);
std::ostream& operator<<(std::ostream& os, const USVec& s);

//============================================================================
class IterationAlgorithm {

public:
    enum CombMethod {
        MHS,
        PWU,
        FLA,
        NONE
    };
    
private:
    Function *targetFun;
    IWPMaxSATSolver *solver; 
    Formula *formula;
    bool hasArgv;
    Options *options;
    
public:
    IterationAlgorithm(Function *_targetFun, IWPMaxSATSolver *_solver,
                       Formula *_formula, bool _hasArgv, Options *_options);
    ~IterationAlgorithm();
    
    void run(ProgramProfile *prof, CombMethod combineMethod);
    USVVec allDiagnosis(std::vector<ProgramTrace*> traces,
                       YicesSolver *yices);
    USVec allMinMCS2(YicesSolver *yices,
                     std::vector<BoolVarExprPtr> &AV,
                     std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    
    // Combination methods
    USVec combineByMHS(USVVec &MCSes);
    USVec combineByPWU(USVVec &MCSes);
    USVec combineByFlatten(USVVec &MCSes);
    
    // Diagnosis computation methods
    UVVec allMinMCS(IWPMaxSATSolver *solver, Formula *WF,
                    std::vector<BoolVarExprPtr> &AV,
                    std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    UVVec allMinMCS_PP(YicesSolver *yices,
                       std::vector<BoolVarExprPtr> &AV,
                       std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    UVVec allMinMCS_AR(YicesSolver *yices,
                       std::vector<BoolVarExprPtr> &AV,
                       std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    
    // TCAS
    void runTCAS(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    void runTCAS_PWU(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    void runTCAS_PP(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    void runTCAS_PP_PWU(std::vector<ExprPtr> TCExprs,std::vector<ExprPtr> GOExprs);
    void runTCAS_AR(std::vector<ExprPtr> TCExprs, std::vector<ExprPtr> GOExprs);
    
private:
    void   pairwiseUnion(USVVec MCSes, USVec &Diag);
    double getCodeSizeReduction(USVec MCSes, unsigned totalNbLine);
    // Operations on vectors/sets
    void removeDoublons(USVec &v);
    void removeSubsets(USVec &v);
    void copy(UVVec &vv, USVec &sv);
    // For debugging
    void checkControlFlow(YicesSolver *solver);
    int  getBlockTransVal(YicesSolver *s, BasicBlock *bb1, BasicBlock *bb2);
    void dumpTransValues(YicesSolver *solver);
    
    
};
//============================================================================

#endif
