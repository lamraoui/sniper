/**
 * \file SymbolMap.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   15 February 2016
 */

#include "SymbolMap.h"
 

// S[v->I(v)]
SymbolPtr SymbolMap::addInput(Value *v) {
    SymbolPtr s = std::make_shared<InputSymbol>(v);
    update(v, s);
    return s;
}

// S[v->S(v)]
SymbolPtr SymbolMap::create(Value *v) {
    SymbolPtr s = std::make_shared<Symbol>(v);
    update(v, s);
    return s;
}

// S[v->S(v)=S1 op S2]
SymbolPtr SymbolMap::createBinaryOp(Value *v, SymbolPtr s1, SymbolPtr s2) {
    SymbolPtr s = std::make_shared<SSBinaryOpSymbol>(v, s1, s2);
    update(v, s);
    return s;
}

// S[v->S(v)=S1 op V2]
SymbolPtr SymbolMap::createBinaryOp(Value *v, SymbolPtr s1, Value *v2) {
    SymbolPtr s = std::make_shared<SVBinaryOpSymbol>(v, s1, v2);
    update(v, s);
    return s;
}

// S[v->S(v)=V1 op S2]
SymbolPtr SymbolMap::createBinaryOp(Value *v, Value *v1, SymbolPtr s2) {
    SymbolPtr s = std::make_shared<VSBinaryOpSymbol>(v, v1, s2);
    update(v, s);
    return s;
}

// S[v->S(v)=(S1?S2:S3)]
SymbolPtr SymbolMap::createSelectOp(Value *v, SymbolPtr s1, SymbolPtr s2, SymbolPtr s3) {
    SymbolPtr s = std::make_shared<SSSelectOpSymbol>(v, s1, s2, s3);
    update(v, s);
    return s;
}

// S[v->S(v)=(S1?V2:S3)]
SymbolPtr SymbolMap::createSelectOp(Value *v, SymbolPtr s1, Value *v2, SymbolPtr s3) {
    SymbolPtr s = std::make_shared<VSSelectOpSymbol>(v, s1, v2, s3);
    update(v, s);
    return s;
}

// S[v->S(v)=(S1?S2:V3)]
SymbolPtr SymbolMap::createSelectOp(Value *v, SymbolPtr s1, SymbolPtr s2, Value *v3) {
    SymbolPtr s = std::make_shared<SVSelectOpSymbol>(v, s1, s2, v3);
    update(v, s);
    return s;
}

// S[v->S(v)=(S1?V2:V3)]
SymbolPtr SymbolMap::createSelectOp(Value *v, SymbolPtr s1, Value *v2, Value *v3) {
    SymbolPtr s = std::make_shared<VVSelectOpSymbol>(v, s1, v2, v3);
    update(v, s);
    return s;
}

// S[v->S(v)= cast S1]
SymbolPtr SymbolMap::createCastOp(Value *v, SymbolPtr s1) {
    SymbolPtr s = std::make_shared<CastOpSymbol>(v, s1);
    update(v, s);
    return s;
}

// P[ptr-> gep *a, S(v1) ]
SymbolPtr SymbolMap::createGepOp(Value *ptr, Value *a, SymbolPtr s1) {
    SymbolPtr s = std::make_shared<GepOpSymbol>(ptr, a, s1);
    update(ptr, s);
    return s;
}

// S(v)
SymbolPtr SymbolMap::get(Value *v) {
    return memory[v];
}

// S[v->NULL]
void SymbolMap::remove(Value *v) {
    memory.erase(v);
}

// S[v->S(v1)]
void SymbolMap::update(Value *v, Value *v1) {
    SymbolPtr s = memory[v1];
    memory[v] = s;
}

// S[v->s]
void SymbolMap::update(Value *v, SymbolPtr s) {
    memory[v] = s;
}

bool SymbolMap::contains(Value *v) {
    std::map<Value*, SymbolPtr>::iterator it = memory.find(v);
    // Element found
    if(it!=memory.end()) {
        return true;
    } else {
        return false;
    }
}

void SymbolMap::clear() {
    memory.clear();
}

void SymbolMap::dump() {
    std::cout << "\n<SymbolMap Dump>\n";
    std::map<Value*,SymbolPtr>::iterator it;
    for (it=memory.begin(); it!=memory.end(); ++it) {
        Value *v = it->first;
        SymbolPtr s = it->second;
        if (v!=NULL) {
            if (ConstantInt *CI = dyn_cast<ConstantInt>(v)) {
                int val = CI->getSExtValue();
                std::cout << val << " : ";
            }
            else if (isa<GlobalVariable>(v)) {
                std::cout << v->getName().str() << " : ";
            } else {
                std::string fname = "U";
                Value::use_iterator it = v->use_begin();
                if (it==v->use_end()) {
                    // br
                    v->dump();
                    std::cout << " : ";
                    return;
                }
                else if (Instruction *I = dyn_cast<Instruction>(*it)) {
                    Function *F = I->getParent()->getParent();
                    fname = F->getName().str();
                }
                std::cout << v->getName().str() << "_" << fname << " : ";
            }            
        } else {
            std::cout << "NULL";
        }
        if (s!=NULL) {
            if (s->isInput()) {
                InputSymbolPtr IS = std::static_pointer_cast<InputSymbol>(s);
                IS->dump();
            } else {
                s->dump();
            }
        } else {
            std::cout << "NULL";
        }
        std::cout << std::endl;
    }
}