/**
 * \file ProgramProfile.cpp
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
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

bool ProgramProfile::loadTestcases(std::string inputsFilename,
                                   std::string outputsFilename,
                                   Options *o) {
    // Load testcases (inputs)
    std::ifstream infile(inputsFilename.c_str());
    if (!infile) {
        std::cerr << "error: cannot open the testcase file.\n";
        exit(1);
    }
    if (o->verbose()) {
        std::cout << "Opened testcase file for reading..." << std::endl;
    }
    std::string line;
    std::vector<std::vector<int> > allInputs;
    while (getline(infile, line)) {
        std::istringstream is( line );
        allInputs.push_back(
        std::vector<int>( std::istream_iterator<int>(is),
        std::istream_iterator<int>() ) );
    }

    // Load golden outputs ===
    std::ifstream outfile(outputsFilename.c_str());
    if (!outfile) {
        std::cerr << "error: cannot open the golden output file.\n";
        exit(1);
    }
    if (o->verbose()) {
        std::cout << "Opened golden output file for reading..." << std::endl;
    }
    std::vector<int> allGOutputs;
    int value;
    while (outfile >> value) {
        allGOutputs.push_back(value);
    }
    
    // Check the if the file is wellformed
    if (allInputs.size()!=allGOutputs.size()) {
        std::cerr << "error: number of testcases does not";
        std::cerr << " match the number of golden outputs.\n";
        exit(1);
    }
    int lastSize = -1;
    for(std::vector<int> lineInts : allInputs) {
        if (lineInts.size()!=lastSize && lastSize!=-1) {
            std::cerr << "error: malformed testcase file.\n";
            exit(1);
        }
        lastSize = lineInts.size();
    }
    
    // Save the inputs/outputs
    LLVMContext &context = getGlobalContext();
    IRBuilder<> IRB(context);
    std::vector<std::vector<Value*> > TS;
    unsigned i = 0;
    for(std::vector<int> lineInts : allInputs) {
        std::vector<Value*> TC;
        for (int val : lineInts) {
            Value *V = IRB.getInt32(val);
            TC.push_back(V);
        }
        ProgramTrace *trace = new ProgramTrace(targetFun, TC, FAIL);
        Value *O = IRB.getInt32(allGOutputs[i++]);
        trace->setExpectedOutput(O);
        addProgramTrace(trace);
    }
    if (o->verbose()) {
        std::cout << allInputs.size() << " testcase(s) loaded.\n";
    }
    return true;
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
