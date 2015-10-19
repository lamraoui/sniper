/**
 * DynamicDiagnosesEnum.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/04/27
 * @copyright : NII 2013
 */

#ifndef _DYNAMICDIAGNOSESENUM_H
#define _DYNAMICDIAGNOSESENUM_H

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "Options.h"
#include "Utils/HittingSet.h"
#include "Profile/ProgramTrace.h"
#include "Profile/ProgramProfile.h"
#include "Logic/GenPath.h"
#include "Logic/IWPMaxSATSolver.h"
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"
#include "Logic/MCS.h"
#include "Backends/SniperBackend/IterationAlgorithm.h"


using namespace llvm;

// Types to hold vector-of-ints (Vi) and vector-of-vector-of-ints (Vvi)
typedef std::vector<unsigned> UVec;
typedef std::vector<UVec>     UVVec;
typedef std::vector<UVVec>    UVVVec;
// Types to hold set-of-ints (Su) and vector-of-set-of-ints (Vsu)
typedef std::set<unsigned> USet;
typedef std::vector<USet>  USVec;
typedef std::vector<USVec> USVVec;

// Types to hold vector-of-exprs (Ve) and vector-of-vector-of-exprs (Vve)
typedef std::vector<ExprPtr> EVec;
typedef std::vector<EVec>     EVVec;
typedef std::vector<EVVec>    EVVVec;
// Types to hold set-of-exprs (Se) and vector-of-set-of-exprs (Vse)
typedef std::set<ExprPtr> ESet;
typedef std::vector<ESet>  ESVec;
typedef std::vector<ESVec> ESVVec;


// Pretty printer for vectors and sets
/*std::ostream& operator<<(std::ostream& os, const UVec& v);
std::ostream& operator<<(std::ostream& os, const UVVec& vv);
std::ostream& operator<<(std::ostream& os, const UVVVec& vvv);
std::ostream& operator<<(std::ostream& os, const USet& s);
std::ostream& operator<<(std::ostream& os, const USVec& s);*/

//============================================================================
class DynamicDiagnosesEnum {

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
    DynamicDiagnosesEnum(Function *_targetFun, IWPMaxSATSolver *_solver,
                       Formula *_formula, bool _hasArgv, Options *_options);
    ~DynamicDiagnosesEnum();
    
    void run(ProgramProfile *prof, CombMethod combineMethod);
    
    USVVec allDiagnosis(Formula *TF, std::vector<ProgramTrace*> traces,
                       YicesSolver *yices);
    ESVVec allDiagnosis2(Formula *TF, std::vector<ProgramTrace*> traces,
                         YicesSolver *yices);
    
    USVec allMinMCS2(YicesSolver *yices,
                     std::vector<BoolVarExprPtr> &AV,
                     std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    ESVec allMinMCS3(YicesSolver *yices,
                     std::vector<BoolVarExprPtr> &AV,
                     std::map<BoolVarExprPtr, ExprPtr> &AVMap);
    
    // Combination methods
    USVec combineByMHS(USVVec &MCSes);
    USVec combineByPWU(USVVec &MCSes);
    USVec combineByFlatten(USVVec &MCSes);
    
    
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
