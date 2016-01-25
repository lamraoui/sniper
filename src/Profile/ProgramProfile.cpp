/**
 * \file ProgramProfile.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   18 January 2015
 */

#include "ProgramProfile.h"


std::set<BasicBlock*> ProgramProfile::getFailingRunBB() {
    std::set<BasicBlock*> bbs;
    for (ProgramTrace *t : traces) {
        if (t->isFailing()) {
            std::set<BasicBlock*> b = t->getExecutedBB();
            bbs.insert(b.begin(), b.end());
        }
    }
    return bbs;
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
}

void ProgramProfile::computeBugFreeBlocks(Options *o) {
    
    assert(!bugFreeBlocksComputed && "Bug free blocks already computed!");
    std::set<BasicBlock*> successfulBlocks = getSuccessfulRunBB();
    std::set<BasicBlock*> failingBlocks = getFailingRunBB();
    std::set<BasicBlock*> unknownBlocks = getOtherRunBB();
    if (unknownBlocks.size()>0) {
        std::cout << "warning: some basic blocks were not covered!\n";
    }
    
    // bugFreeBlocks = successfulBlocks - failingBlocks
    std::set_difference(successfulBlocks.begin(), successfulBlocks.end(),
                        failingBlocks.begin(), failingBlocks.end(),
                        std::inserter(bugFreeBlocks, bugFreeBlocks.end()));
    bugFreeBlocksComputed = true;
    
    if (o->verbose()) {
        std::cout << "Bug free blocks coverage: ";
        std::cout << bugFreeBlocks.size() << "/";
        std::cout << targetFun->size() << std::endl;
    }
    
    // Debug
    if (o->dbgMsg()) {
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
    }
}

bool ProgramProfile::isBugFreeBlock(BasicBlock *bb) {
    assert(bb && "Expecting a basic block!");
    assert(bugFreeBlocksComputed && "Bug free blocks not yet computed!");
    const bool isIn = bugFreeBlocks.find(bb)!=bugFreeBlocks.end();
    return isIn;
}

std::set<BasicBlock*> ProgramProfile::getBugFreeBlocks() {
    assert(bugFreeBlocksComputed && "Bug free blocks not yet computed!");
    return bugFreeBlocks;
}

// TODO: use std::find
void ProgramProfile::addProgramTrace(ProgramTrace *e) {
    assert(e && "Expecting a program trace!");
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

std::vector<ProgramTrace*> ProgramProfile::getFailingProgramTraces() {
    std::vector<ProgramTrace*> failingTraces;
    for (ProgramTrace *t : traces) {
        if (t->isFailing()) {
            failingTraces.push_back(t);
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

void ProgramProfile::dump() {
    std::vector<ProgramTrace*> ST = getSuccessfulProgramTraces();
    std::vector<ProgramTrace*> FT = getFailingProgramTraces();
    std::vector<ProgramTrace*> UT = getUnkownProgramTraces();
    std::cout << "Collected program inputs (";
    std::cout << ST.size() << " + "
    << FT.size() << " + "
    << UT.size()  << "):\n";
    std::cout << std::endl;
}
