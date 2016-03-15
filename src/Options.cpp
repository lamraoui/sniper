/**
 * \file Options.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   25 January 2015
 */

#include "Options.h"

/*==== Global options ====*/

static cl::opt <std::string>
InputIRFilename(cl::Positional, cl::desc("Input bitcode file"),
              cl::init("-"), cl::value_desc("filename"));

static cl::opt <std::string>
InputCSourceFilename("cfile", cl::desc("Input C source file"),
            cl::init(""), cl::value_desc("string"));

static cl::opt <std::string>
FunctionName("function", cl::desc("Function name"),
             cl::init(""), cl::value_desc("string"));

static cl::opt <std::string>
TestSuiteFileName("ts", cl::desc("Test suite"),
             cl::init(""), cl::value_desc("string"));

static cl::opt <std::string>
GoldenOutputsFileName("go", cl::desc("Golden outputs"),
                  cl::init(""), cl::value_desc("string"));

static cl::opt <bool>
PrintModIR("print-ir",  cl::desc("Print module Intermediate Representation"));

static cl::opt <bool>
PrintFunIR("print-fun",  cl::desc("Print target function Intermediate representation"));

static cl::opt <bool>
PrintTF("print-tf",  cl::desc("Print the Trace Formula"));

static cl::opt <bool>
PrintDuration("print-dura", cl::desc("Print the processing duration"));

static cl::opt <bool>
PrintMUS("print-mus", cl::desc("Print the MUSes"));

static cl::opt <bool>
PrintMCS("print-mcs", cl::desc("Print the MCSes (before combination)"));

static cl::opt <bool> 
DbgMsg("dbg-msg", cl::desc("Print debug messages"));

static cl::opt <bool>
Verbose("v", cl::desc("Display messages"));

/**
 * \brief Granularity level of the encoding of target programs  
 * into trace formulas. 
 *  
 * With the instruction-level granularity, each LLVM instruction is 
 * encoded in a single clause. With the line-level granularity all 
 * instructions belonging to the same statement line in the original 
 * source are grouped in a single clause. With the block-level granularity, 
 * all instructions belonging to the same LLVM basic block are grouped 
 * in a single clause.
 *
 * A trace formula, with an instruction-level granularity, contains the 
 * same number or more soft clauses than a trace formula with a line-level 
 * granularity or a block-level granularity. The comparison between a trace 
 * formula with a line-level granularity to a formula with a block-level 
 * granularity depends on the way the source code is arranged. Usually, 
 * however, there are more instructions in a line than in a basic blocks. 
 * Note that the granularity level has a big impact on both the computing 
 * time and the precision of the fault localization algorithm.
 */
enum Granularity {
    line, inst, block
};
cl::opt<Granularity> TFGranularityLevel(cl::desc("Choose a level of granularity for the trace formula:"),
    cl::values(
      clEnumVal(line,  "Line-level (default)"),
      clEnumVal(inst , "Instruction-level"),
      clEnumVal(block, "Block-level"),
      clEnumValEnd));

/**
 * \brief Program input generation methods.  
 *
 * The bounded model checking (bmc) method can be used to 
 * generate a single failing input. 
 * The concolic execution method can generate with a high program 
 * coverage many inputs, both failing and successful inputs. 
 */
enum Method {
    bmc, ce
};
cl::opt<Method> TracesGenerationMethod(cl::desc("Choose a traces generation method:"),
cl::values(
clEnumVal(bmc , "Bounded Model Checking, one counterexample"),
clEnumVal(ce,   "Concolic Execution, failing/successful traces (fast)"),
clEnumValEnd));

static cl::opt <unsigned>
UnrollCount("unroll", cl::desc("Maximum unroll count"),
            cl::init(0), cl::value_desc("count"));

static cl::opt <unsigned>
MaxDepth("max-depth", cl::desc("Maximum depth"),
            cl::init(10000), cl::value_desc("depth"));

static cl::opt <bool>
HTF("htf", cl::desc("Use Hardened TF (needs -ce)"));

static cl::opt <unsigned>
NbLOC("loc", cl::desc("Number of line of code"),
            cl::init(0), cl::value_desc("loc"));

static cl::opt <bool>
CheckCFGModel("check-cfg-model", cl::desc("Check CFG model"));

static cl::opt <unsigned>
MCSMaxSize("max-mcs-size", cl::desc("Maximum size of MCSes"),
              cl::init(UINT_MAX), cl::value_desc("MCSMaxSize"));

static cl::opt <bool>
OutputCFGDotFile("cfg-dot", cl::desc("Output the CFG in a dot file."));

/**
 * \brief Diagnosis combination methods. 
 *
 * SNIPER implements three combination techniques: 
 * a flattening-based combination (fla), 
 * a pair-wise-based combination (pwu), 
 * and a hitting-set-based combination (mhs). 
 */
enum CombineMethod {
    none, fla, pwu, mhs
};
cl::opt<CombineMethod>
ChoosedCombineMethod(cl::desc("Choose a combination method:"),
    cl::values(
    clEnumVal(none, "None (default)"),
    clEnumVal(fla,  "Flatten"),
    clEnumVal(pwu,  "Pair-wise union"),
    clEnumVal(mhs,  "Minimal hitting-set"),
    clEnumValEnd));

/*==== Implementation ====*/

void printVersionInformation() {
    std::cout << "SNIPER:\n";
    std::cout << "  Version 2.0\n";
    std::cout << "  Built Jan 11 2014 (16:19:35).\n\n";
    std::cout << "  Yices version 1.0.39 (http://yices.csl.sri.com/)\n";
    std::cout << "  LLVM version 3.3 (http://llvm.org/)\n";
}

Options::Options(int argc, char **argv) {
    hideOptions();
    cl::SetVersionPrinter(printVersionInformation);
    cl::ParseCommandLineOptions(argc, argv, 
    "Automatic Error Localizer for Imperative Programs (SNIPER)\n\n"
    "  This program automatically locates root causes\n" 
    "  in imperative programs such as ANSI-C\n\n"
    "  Si-mohamed Lamraoui, Shin Nakajima\n"
    "  SOKENDAI University & National Institute of Informatics (Tokyo, Japan)\n"
    "  contact: simo@nii.ac.jp\n");
}

std::string Options::getInputIRFilename() {
    return InputIRFilename;
}

std::string Options::getInputCSourceFilename() {
    return InputCSourceFilename;
}

std::string Options::getFunctionName() {
    return FunctionName;
}

std::string Options::getTestSuiteFileName() {
    return TestSuiteFileName;
}

std::string Options::getGoldenOutputsFileName() {
    return GoldenOutputsFileName;
}

bool Options::dbgMsg() {
    return DbgMsg;
}

bool Options::verbose() {
    return Verbose;
}

bool Options::printDuration() {
    return PrintDuration;
}

bool Options::printModIR() {
    return PrintModIR;
}

bool Options::printFunIR() {
    return PrintFunIR;
}

bool Options::printTF() {
    return PrintTF;
}

bool Options::printMUS() {
    return PrintMUS;
}

bool Options::printMCS() {
    return PrintMCS;
}

bool Options::instructionGranularityLevel() {
    return (TFGranularityLevel==inst);
}

bool Options::lineGranularityLevel() {
    return (TFGranularityLevel==line);
}

bool Options::blockGranularityLevel() {
    return (TFGranularityLevel==block);
}

bool Options::methodBMC() {
    return (TracesGenerationMethod==bmc);
}

bool Options::methodConcolic() {
    return (TracesGenerationMethod==ce);
}

unsigned Options::getUnrollCount() {
    return UnrollCount;
}

unsigned Options::getMaxDepth() {
    return MaxDepth;
}

bool Options::htfUsed() {
    return HTF;
}

unsigned Options::getNbLOC() {
    return NbLOC;
}

bool Options::checkCFGModel() {
    return CheckCFGModel;
}

unsigned Options::mcsMaxSize() {
    return MCSMaxSize;
}

bool Options::outputCFGDotFile() {
    return OutputCFGDotFile;
}

unsigned Options::getCombineMethod() {
    if (ChoosedCombineMethod==fla) {
        return Combine::FLA;
    } else if (ChoosedCombineMethod==pwu) {
        return Combine::PWU;
    } else if (ChoosedCombineMethod==mhs) {
        return Combine::MHS;
    }
    return Combine::NONE;
}

// Hide unwanted options
void Options::hideOptions() {
    StringMap<cl::Option*> Map;
    cl::getRegisteredOptions(Map);
    assert(Map.count("x86-use-vzeroupper") > 0);
    Map["x86-use-vzeroupper"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("x86-early-ifcvt") > 0);
    Map["x86-early-ifcvt"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("x86-asm-syntax") > 0);
    Map["x86-asm-syntax"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("verify-scev") > 0);
    Map["verify-scev"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("verify-regalloc") > 0);
    Map["verify-regalloc"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("verify-dom-info") > 0);
    Map["verify-dom-info"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("verify-loop-info") > 0);
    Map["verify-loop-info"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("time-passes") > 0);
    Map["time-passes"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("struct-path-tbaa") > 0);
    Map["struct-path-tbaa"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("spiller") > 0);
    Map["spiller"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("shrink-wrap") > 0);
    Map["shrink-wrap"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("regalloc") > 0);
    Map["regalloc"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("print-machineinstrs") > 0);
    Map["print-machineinstrs"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("print-before-all") > 0);
    Map["print-before-all"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("print-after-all") > 0);
    Map["print-after-all"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("pre-RA-sched") > 0);
    Map["pre-RA-sched"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("mc-x86-disable-arith-relaxation") > 0);
    Map["mc-x86-disable-arith-relaxation"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("limit-float-precision") > 0);
    Map["limit-float-precision"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("limit-float-precision") > 0);
    Map["limit-float-precision"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("join-liveintervals") > 0);
    Map["join-liveintervals"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("ffunction-sections") > 0);
    Map["ffunction-sections"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("fdata-sections") > 0);
    Map["fdata-sections"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("enable-tbaa") > 0);
    Map["enable-tbaa"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("enable-correct-eh-support") > 0);
    Map["enable-correct-eh-support"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("disable-spill-fusing") > 0);
    Map["disable-spill-fusing"]->setHiddenFlag(cl::Hidden);
    assert(Map.count("asm-verbose") > 0);
    Map["asm-verbose"]->setHiddenFlag(cl::Hidden);
}

Options::~Options() {
    
}

