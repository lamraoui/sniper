/**
 * TCAS.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/29
 * @copyright : NII 2013
 */

#include "TCAS.h"


// =============================================================================
// TCAS 
// 
// Constructor
// 
// =============================================================================
TCAS::TCAS(Options *_options, std::string _filepath, std::string _assertVarName) 
: options(_options), filepath(_filepath), assertVarName(_assertVarName) {
    unsigned line1 = options->getTCASline1(); 
    unsigned line2 = options->getTCASline2();
    unsigned line3 = options->getTCASline3();
    unsigned line4 = options->getTCASline4();
    std::cout << "Location(s): ";
     if (line1!=0)
     std::cout << line1;
     else 
     std::cout << "?";
     if (line2!=0)
     std::cout << ", " << line2;
     if (line3!=0)
     std::cout << ", " << line3;
     if (line4!=0)
     std::cout << ", " << line4;
     std::cout << std::endl;
}

// =============================================================================
// load
//
// Output: TCExprs the inputs, GOExprs the golden outputs
// =============================================================================
void TCAS::load(std::vector<ExprPtr> &TCExprs, std::vector<ExprPtr> &GOExprs)  {
    // Load the failing testcases for this version
    unsigned vers = options->getTCASVersion();
    std::ostringstream oss1;
    oss1 << "./examples/tcas2/v" << vers << "/finputs.txt";
    loadTestCases(oss1.str());
    std::ostringstream oss2;
    oss2 << "./examples/tcas2/v" << vers << "/golden-outputs.txt";
    loadGoldenOutputs(oss2.str());
    TCExprs.insert(TCExprs.end(), testCaseExprs.begin(), testCaseExprs.end());
    GOExprs.insert(GOExprs.end(), goldenOutputExprs.begin(), goldenOutputExprs.end());
}

bool TCAS::loadGoldenOutputs(std::string filename) {
    
    std::ifstream infile(filename.c_str());
    if (!infile) {
        std::cerr << "error: cannot open the golden_outputs file.\n";
        exit(1);
    }
    IntVarExprPtr exitVar1 = Expression::mkIntVar(assertVarName); //alt_sep.i.X
    BoolVarExprPtr exitVar2 = Expression::mkBoolVar("alt_sep_test.exit");
    SInt32NumExprPtr nbArg = Expression::mkSInt32Num(13);
    IntVarExprPtr argcVar = Expression::mkIntVar("argc");
    ExprPtr argcExpr = Expression::mkEq(argcVar, nbArg);
    unsigned val;
    while (infile>>val) {
        // (and alt_sep_test.exit (= iftmp.30.i.0 1))
        // (= alt_sep.0.i X)        
        ExprPtr valExpr = Expression::mkSInt32Num(val);
        std::vector<ExprPtr> andExprVec;
        ExprPtr e1 = Expression::mkEq(exitVar1, valExpr);
        andExprVec.push_back(e1);
        andExprVec.push_back(exitVar2);
        andExprVec.push_back(argcExpr);
        ExprPtr andExpr = Expression::mkAnd(andExprVec);
        andExpr->setHard();
        goldenOutputExprs.push_back(andExpr);
        // Not(golden output)        
        std::vector<ExprPtr> andExprVec2;
        ExprPtr e11 = Expression::mkEq(exitVar1,  valExpr);
        ExprPtr ne1 = Expression::mkNot(e11);
        andExprVec2.push_back(ne1);
        andExprVec2.push_back(exitVar2);
        andExprVec2.push_back(argcExpr);
        ExprPtr andExpr2 = Expression::mkAnd(andExprVec2);
        andExpr2->setHard();
        notGoldenOutputExprs.push_back(andExpr2);
    }   
    //std::cout << "Number of testcases: " << golden_outputs.size() << std::endl;
    return true;
}


// =============================================================================
// load
//
// Load the testcases and the golden output for the TCAS version 'vers', 
// and save it into the program profile 'PP'.
//
// The testcase file must be in the following form:
// arg1 arg2 ... argn\n arg1 arg2 ... argn\n ...
// =============================================================================
/*bool TCAS::load(unsigned vers, ProgramProfile &PP) {
    
       
    std::ostringstream oss2;
    oss2 << filepath << "/v" << vers << "/golden-outputs.txt";
    tcas->loadGoldenOutputs(oss2.str());
    
    // Open the file for reading
    std::ostringstream oss1;
    oss1 << filepath << "/v" << vers << "/finputs.txt";
    std::string filename = oss1.str();
    std::ifstream infile(filename.c_str());    
    if (!infile) {
        std::cerr << "error: cannot open the testcases file.\n";
        exit(1);
    }
    //std::cout << "Opened testcase file for reading..." << std::endl;
    std::string line;
    std::vector< std::vector<int> > all_integers;
    while (getline(infile, line)) {
        std::istringstream is(line);
        all_integers.push_back( 
        std::vector<int>(std::istream_iterator<int>(is),
        std::istream_iterator<int>()));
    }
    
    std::vector< std::vector<int> >::iterator it1;
    for(it1 = all_integers.begin(); it1 != all_integers.end(); ++it1) {
        std::vector<int> line_ints = *it1;
        if (line_ints.size()!=12) {
            std::cerr << "error: cannot load the testcases file.\n";
            exit(1);
        }
        // Create a new testcase execution
        Execution *E = new  Execution(targetFun, FAIL);
        unsigned n = 1;
        std::ostringstream oss;
        std::vector<int>::iterator it2;
        for(it2 = line_ints.begin(); it2 != line_ints.end(); ++it2) {
            int arg = *it2;                
            // (= ARGV n) arg)
            oss << "(= (ARGV " << n << ") " << arg << ")";
            E->addProgramInput(oss.str());
            n++;
        }
        // Add the testcase to the program profile
        PP.addExecution(E);
    }
    std::cout << "failing trace: " << PP.getNbExecution() << std::endl;
    return true;
}*/



// =============================================================================
// loadTestCases
//
// The testcase file must be in the following form:
// arg1 arg2 ... argn\n arg1 arg2 ... argn\n ...
//
// =============================================================================
bool TCAS::loadTestCases(std::string filename) {

    std::ifstream infile(filename.c_str());    
    if (!infile) {
        std::cerr << "error: cannot open the testcases file.\n";
        exit(1);
    }
    //std::cout << "Opened testcase file for reading..." << std::endl;
    std::string line;
    std::vector< std::vector<int> > all_integers;
    while ( getline( infile, line ) ) {
        std::istringstream is( line );
        all_integers.push_back( 
        std::vector<int>( std::istream_iterator<int>(is),
                        std::istream_iterator<int>() ) );
    }
    testCaseExprs.clear();
    IntToIntVarExprPtr ARGV = Expression::mkIntToIntVar("ARGV");
    unsigned nbError = 0;
    std::vector< std::vector<int> >::iterator it1;
    for(it1 = all_integers.begin(); it1 != all_integers.end(); ++it1) {
        std::vector<int> line_ints = *it1;
        if (line_ints.size()==12) {
            unsigned n = 1;
            std::vector<ExprPtr> andExprVec;
            std::vector<int>::iterator it2;
            for(it2 = line_ints.begin(); it2 != line_ints.end(); ++it2) {
                int arg = *it2;                
                // (= ARGV i) arg)
                AppExprPtr appe = Expression::mkApp(ARGV, Expression::mkSInt32Num(n));
                ExprPtr arge = Expression::mkSInt32Num(arg);
                ExprPtr eqe = Expression::mkEq(appe, arge);
                andExprVec.push_back(eqe);
                n++;
            }
            // (and () () ())
            ExprPtr andExpr = Expression::mkAnd(andExprVec);
            andExpr->setHard();
            testCaseExprs.push_back(andExpr);
        } else {
            nbError++;
        }       
    }
    if (nbError>0) {
        std::cout << nbError << "/" << all_integers.size() 
        << " testcase(s) loading failed.\n";
    }
    //std::cout << "failing trace: " << testCaseExprs.size() << std::endl;
    return true;
}

// =============================================================================
// http://stackoverflow.com/questions/12875993/efficient-set-intersection-of-a-collection-of-sets-in-c
// Do not call this method if you have a single set...
// And the pointers better not be null either!
// =============================================================================
std::vector<unsigned> 
TCAS::intersect(std::vector< std::set<unsigned> > const& sets) {
    
    // for (auto s: sets) { assert(s && "I said no null pointer"); }

    std::vector<unsigned> result; // only return this one, for NRVO to kick in
    
    // 0. Check obvious cases
    if (sets.empty()) { 
        return result; 
    }
    if (sets.size() == 1) {
        result.assign(sets.front().begin(), sets.front().end());
        return result;
    }
    // 1. Merge first two sets in the result
    std::set_intersection(sets[0].begin(), sets[0].end(),
                          sets[1].begin(), sets[1].end(),
                          std::back_inserter(result));
    if (sets.size() == 2) { 
        return result; 
    }
    // 2. Merge consecutive sets with result into buffer, then swap them around
    //    so that the "result" is always in result at the end of the loop.
    std::vector<unsigned> buffer; // outside the loop so that we reuse its memory
    for (size_t i = 2; i < sets.size(); ++i) {
        buffer.clear();
        std::set_intersection(result.begin(), result.end(),
                              sets[i].begin(), sets[i].end(),
                              std::back_inserter(buffer));
        swap(result, buffer);
    }
    return result;
}

TCAS::~TCAS() {

}


/*
 
 //====
 std::stringstream sfilename;
 sfilename << "./examples/tcas/v" << version << "/outputs.txt";
 std::string filename = sfilename.str();
 std::cout << "Version: " << filename << std::endl;
 std::ifstream outfile1(filename.c_str());
 if (!outfile1) {
 error("cannot open the outputs.txt file.");
 }
 std::vector<unsigned> v1outputs;
 unsigned val1;
 while (outfile1>>val1) {
 v1outputs.push_back(val1);
 }  
 std::cout << "SIZE O: " << v1outputs.size() << std::endl; 
 
 */