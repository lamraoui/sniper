/**
 * ProgramProfile.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/11/15
 * @copyright : NII 2013
 */

#include "ProgramProfile.h"

/*
void ProgramProfile::addFailingBlocks(std::vector<BasicBlock*> &bb) {
    bugFreeBlocksComputed = false;
    std::copy(bb.begin(), bb.end(), 
              std::inserter(failingBlocks, failingBlocks.end()));
}
void ProgramProfile::addSuccessfulBlocks(std::vector<BasicBlock*> &bb) {
    bugFreeBlocksComputed = false;
    std::copy(bb.begin(), bb.end(), 
              std::inserter(successfulBlocks, successfulBlocks.end()));
}
void ProgramProfile::addUnknowBlocks(std::vector<BasicBlock*> &bb) {
    bugFreeBlocksComputed = false;
    std::copy(bb.begin(), bb.end(), 
              std::inserter(unknownBlocks, unknownBlocks.end()));
}*/


std::set<BasicBlock*> ProgramProfile::getFailingRunBB() {
    std::set<BasicBlock*> bbs;
    for (ProgramTrace *t : traces) {
        if (t->isFailing()) {
            std::set<BasicBlock*> b = t->getExecutedBB();
            bbs.insert(b.begin(), b.end());
        }
    }
    return bbs;
    // OLD
    //return failingBlocks;
}

std::set<BasicBlock*> ProgramProfile::getSuccessfulRunBB() {
    std::set<BasicBlock*> bbs;
    for (ProgramTrace *t : traces) {
        if (t->isSuccessful()) {
            std::set<BasicBlock*> b = t->getExecutedBB();
            bbs.insert(b.begin(), b.end());
        }
    }
    return bbs;
    // OLD
    //return successfulBlocks;
}

std::set<BasicBlock*> ProgramProfile::getOtherRunBB() {
    std::set<BasicBlock*> bbs;
    for (ProgramTrace *t : traces) {
        if (t->isUnknow()) {
            std::set<BasicBlock*> b = t->getExecutedBB();
            bbs.insert(b.begin(), b.end());
        }
    }
    return bbs;
    // OLD
    //return unknownBlocks;
}

void ProgramProfile::removeNFirstFailingProgramTraces(unsigned n) {
    /*failingTraces.erase(failingTraces.begin(),
    failingTraces.size()>n? failingTraces.begin()+n : failingTraces.end());*/
    assert("ProgramProfile::removeNFirstFailingProgramTraces");
}

void ProgramProfile::removeNFirstSuccessfulProgramTraces(unsigned n) {
    /*successfulTraces.erase(successfulTraces.begin(),
    successfulTraces.size()>n? successfulTraces.begin()+n : successfulTraces.end());*/
    assert("ProgramProfile::removeNFirstSuccessfulProgramTraces");
}

void ProgramProfile::computeBugFreeBlocks(Options *o) {
    
    std::set<BasicBlock*> successfulBlocks = getSuccessfulRunBB();
    std::set<BasicBlock*> failingBlocks = getFailingRunBB();
    std::set<BasicBlock*> unknownBlocks = getOtherRunBB();
    if (unknownBlocks.size()>0) {
        std::cout << "warning: unknown blocks!\n";
        //exit(1);
    }
    assert(!bugFreeBlocksComputed && "Bug free blocks already computed!");
    
    std::set_difference(successfulBlocks.begin(), successfulBlocks.end(),
                        failingBlocks.begin(), failingBlocks.end(),
                        std::inserter(bugFreeBlocks, bugFreeBlocks.end()));

    // Remove from the bug free basicblocks the
    // basicblocks with pointers (gep, load, store)
    if (o->ptfUsed()) { // Not needed for HFTF
        if (o->verbose()) {
            std::cout << "Filtering bug free blocks.\n";
        }
        std::set<BasicBlock*>::iterator its;
        for(its = bugFreeBlocks.begin(); its != bugFreeBlocks.end();) {
            BasicBlock *bb = *its;
            bool containsPointerInst = false;
            for (BasicBlock::iterator i=bb->begin(), e=bb->end(); i!=e; ++i) {
                Instruction *inst = i;
                if (isa<GetElementPtrInst>(inst)
                    || isa<LoadInst>(inst)
                    || isa<StoreInst>(inst)) {
                    containsPointerInst = true;
                    break;
                }
            }
            if (containsPointerInst) {
                its = bugFreeBlocks.erase(its);
            } else {
                ++its;
            }
        }
    }
    
    bugFreeBlocksComputed = true;
    if (o->verbose()) {
        std::cout << "Bug free blocks coverage: ";
        std::cout << bugFreeBlocks.size() << "/" << targetFun->size() << std::endl;
    }
    
    
    // TEST
    /*std::set<BasicBlock*> allBlocks;
    for (Function::iterator i=targetFun->begin(), e=targetFun->end(); i!=e; ++i) {
        allBlocks.insert(i);
    }
    std::set<BasicBlock*> PB;
    std::set_difference(allBlocks.begin(), allBlocks.end(),
                        bugFreeBlocks.begin(), bugFreeBlocks.end(),
                        std::inserter(PB, PB.end()));
    std::cout << PB.size() << "\t" << std::flush;
    
    std::cout << std::endl;
    std::cout << "PB: ";
    std::set<BasicBlock*>::iterator itpb;
    for (itpb=PB.begin(); itpb!=PB.end(); ++itpb) {
        BasicBlock *bb = *itpb;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;*/
    
    
    // Debug
    /*if (o->dbgMsg()) {
        std::cout << "Failing basicblocks: ";
        std::set<BasicBlock*>::iterator its;
        for (its=failingBlocks.begin(); its!=failingBlocks.end(); ++its) {
            BasicBlock *bb = *its;
            std::cout << bb->getName().str() << " ";
        }
        std::cout << std::endl;
        std::cout << "Successful basicblocks: ";
        for (its=successfulBlocks.begin(); its!=successfulBlocks.end(); ++its) {
            BasicBlock *bb = *its;
            std::cout << bb->getName().str() << " ";
        }
        std::cout << std::endl;
        std::cout << "Unknow basicblocks: ";
        for (its=unknownBlocks.begin(); its!=unknownBlocks.end(); ++its) {
            BasicBlock *bb = *its;
            std::cout << bb->getName().str() << " ";
        }
        std::cout << std::endl;
        std::cout << "Bug free basicblocks: ";
        for (its=bugFreeBlocks.begin(); its!=bugFreeBlocks.end(); ++its) {
            BasicBlock *bb = *its;
            std::cout << bb->getName().str() << " ";
        }
        std::cout << std::endl;
    }*/
}

bool ProgramProfile::isBugFreeBlock(BasicBlock *bb) {
    assert(bugFreeBlocksComputed && "Bug free blocks not yet computed!");
    const bool isIn = bugFreeBlocks.find(bb)!=bugFreeBlocks.end();
    return isIn;
}

std::set<BasicBlock*> ProgramProfile::getBugFreeBlocks() {
    assert(bugFreeBlocksComputed && "Bug free blocks not yet computed!");
    return bugFreeBlocks;
}

void ProgramProfile::addProgramTrace(ProgramTrace *e) {
    bool found = false;
    for (ProgramTrace *t : traces) {
        if (t==e) {
            found = true;
            break;
        }
    }
    if (!found) {
        traces.push_back(e);
    }
}

std::vector<ProgramTrace*> ProgramProfile::getProgramTraces() {
    return traces;
}

std::vector<ProgramTrace*> ProgramProfile::getFailingProgramTraces(Options *o) {
    unsigned traceAdded = 0;
    unsigned maxFailTraces = 0;
    if (o) {
        maxFailTraces = o->getMaxFailTraces();
    }
    std::vector<ProgramTrace*> failingTraces;
    for (ProgramTrace *t : traces) {
        if (t->isFailing() && (traceAdded<maxFailTraces || maxFailTraces<=0)) {
            failingTraces.push_back(t);
            traceAdded++;
        }
    }
    return failingTraces;
}

std::vector<ProgramTrace*> ProgramProfile::getSuccessfulProgramTraces() {
    std::vector<ProgramTrace*> successfulTraces;
    for (ProgramTrace *t : traces) {
        if (t->isSuccessful()) {
            successfulTraces.push_back(t);
        }
    }
    return successfulTraces;
}

std::vector<ProgramTrace*> ProgramProfile::getUnkownProgramTraces() {
    std::vector<ProgramTrace*> otherTraces;
    for (ProgramTrace *t : traces) {
        if (t->isUnknow()) {
            otherTraces.push_back(t);
        }
    }
    return otherTraces;
}

bool ProgramProfile::hasFailingProgramTraces() {
    for (ProgramTrace *t : traces) {
        if (t->isFailing()) {
            return true;
        }
    }
    return false;
}

// Save the function summary (inputs/output values)
void ProgramProfile::addFunSummary(Function *f, Value *input, Value *output) {
    std::string name = f->getName().str();
    FunctionSummary *FS = funSummariesMap[name];
    if (!FS) {
        FS = new FunctionSummary(f);
        funSummariesMap[name] = FS;
    }
    FS->addIO(input, output);
}

FunctionSummary* ProgramProfile::getFunSummary(Function *f) {
    // TODO
    std::map<std::string,FunctionSummary*>::const_iterator it;
    for (it=funSummariesMap.begin(); it!=funSummariesMap.end(); ++it) {
        std::string funName = it->first;
        if (funName==f->getName()) {
            return it->second;
        }
    }
    return NULL;
}


// Save the non-linear operation summary (y and z values of x=y*z)
void ProgramProfile::addNLOpSummary(DILocation loc, Value *y, Value *z) {
    NLOperationSummary *NLS = nlOpSummariesMap[loc];
    if (!NLS) {
        NLS = new NLOperationSummary(loc);
        nlOpSummariesMap[loc] = NLS;
    }
    NLS->addValues(y, z);
}

NLOperationSummary* ProgramProfile::getNLOpSummary(BinaryOperator *bo) {
    // TODO
    if (MDNode *N = bo->getMetadata("dbg")) {
        DILocation loc(N);
        std::map<DILocation,NLOperationSummary*>::const_iterator it;
        for (it=nlOpSummariesMap.begin(); it!=nlOpSummariesMap.end(); ++it) {
            DILocation l = it->first;
            if (l.getLineNumber()==loc.getLineNumber() 
                && l.getColumnNumber()==loc.getColumnNumber()) {
                return it->second;
            }
        }   
    }
    return NULL;
}

void ProgramProfile::dump() {
    std::vector<ProgramTrace*> ST = getSuccessfulProgramTraces();
    std::vector<ProgramTrace*> FT = getFailingProgramTraces();
    std::vector<ProgramTrace*> UT = getUnkownProgramTraces();
    std::cout << "Collected program inputs (";
    std::cout << ST.size() << " + "
    << FT.size() << " + "
    << UT.size()  << "):\n";
    /*for (ProgramTrace *t : traces) {
        t->dump();
        std::cout << std::endl;
    }
    std::cout << std::endl;*/
    
    /*std::cout << "Failing basicblocks: ";
    std::set<BasicBlock*>::iterator its;
    for (its=failingBlocks.begin(); its!=failingBlocks.end(); ++its) {
        BasicBlock *bb = *its;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
    std::cout << "Successful basicblocks: ";
    for (its=successfulBlocks.begin(); its!=successfulBlocks.end(); ++its) {
        BasicBlock *bb = *its;
        std::cout << bb->getName().str() << " ";
    }
    std::cout << std::endl;
    std::cout << "Unknow basicblocks: ";
    for (its=unknownBlocks.begin(); its!=unknownBlocks.end(); ++its) {
        BasicBlock *bb = *its;
        std::cout << bb->getName().str() << " ";
    }*/
    std::cout << std::endl;
    /*std::cout << "Bug free basicblocks: ";
    if (bugFreeBlocksComputed) {
        for (its=bugFreeBlocks.begin(); its!=bugFreeBlocks.end(); ++its) {
            BasicBlock *bb = *its;
            std::cout << bb->getName().str() << " ";
        }
    } else {
        std::cout << " [not computed]";
    }
    std::cout << std::endl;   
    if (funSummariesMap.size()>0) {
        std::cout << "Function summaries:\n";
        std::map<std::string, FunctionSummary*>::const_iterator it1;
        for (it1=funSummariesMap.begin(); it1!=funSummariesMap.end(); ++it1) {
            FunctionSummary *fs = it1->second;    
            fs->dump();
        }
    }
    if (nlOpSummariesMap.size()>0) {
        std::cout << "NL operation summaries:\n";
        std::map<DILocation, NLOperationSummary*>::const_iterator it2;
        for (it2=nlOpSummariesMap.begin(); it2!=nlOpSummariesMap.end(); ++it2) {
            NLOperationSummary *nls = it2->second;    
            nls->dump();
        }
    }*/
}


//========= NLOperationSummary ======================


NLOperationSummary::NLOperationSummary(DILocation _loc) : loc(_loc) {
}

void NLOperationSummary::addValues(Value *y, Value *z) {
    values.push_back(std::make_pair(y, z));
}

std::vector<std::pair<Value*,Value*> > NLOperationSummary::getValues() {
    return values;
}

void NLOperationSummary::dump() {
    std::cout << "[";
    std::vector<std::pair<Value*,Value*> >::const_iterator it1;
    for (it1=values.begin(); it1!=values.end(); ++it1) {
        ConstantInt *CI1 = dyn_cast<ConstantInt>(it1->first);
        int x = CI1->getSExtValue();
        ConstantInt *CI2 = dyn_cast<ConstantInt>(it1->second);
        int y = CI2->getSExtValue();
        std::cout << "x = " << x << " op " << y << ", " << std::endl;
    }
    std::cout << "]\n";
}

NLOperationSummary::~NLOperationSummary() { 
    values.clear();
}


//========= FunctionSummary ======================

FunctionSummary::FunctionSummary(Function *_foo) : foo(_foo) {
}

void FunctionSummary::addIO(Value *input, Value *output) {
    io_t i;
    i.inputs.push_back(input);
    i.output = output;
    inOuts.push_back(i);
}

std::vector<io_t> FunctionSummary::getInputOutputs() {
    return inOuts;
}

void FunctionSummary::dump() {
    std::string name = foo->getName().str();
    if (name=="") {
        name = "foo";
    }
    std::cout << "[";
    std::vector<io_t>::const_iterator it1;
    for (it1=inOuts.begin(); it1!=inOuts.end(); ++it1) {
        io_t i = *it1;
        ConstantInt *CI1 = dyn_cast<ConstantInt>(i.output);
        int output = CI1->getSExtValue();
        std::cout << output << " = " << name << "(";
        std::vector<Value*>::const_iterator it2;
        for (it2=i.inputs.begin(); it2!=i.inputs.end(); ++it2) {
            ConstantInt *CI1 = dyn_cast<ConstantInt>(*it2);
            int input = CI1->getSExtValue();
            std::cout << input << " ";
        }
        std::cout << "), ";
    }
    std::cout << "]\n";
}

FunctionSummary::~FunctionSummary() { 
    inOuts.clear();
}

