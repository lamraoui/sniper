/**
 * Expression.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/05/31
 * @copyright : NII 2013
 */

#include "Expression.h"

unsigned Expression::ID = 0;
unsigned Expression::NbIntVariables = 0;
unsigned Expression::NbBoolVariables = 0;

TrueExprPtr Expression::mkTrue() {
    return std::make_shared<TrueExpression>();
}

FalseExprPtr Expression::mkFalse() { 
    return std::make_shared<FalseExpression>();
}

UInt32NumExprPtr Expression::mkUInt32Num(unsigned value) { 
    return std::make_shared<UInt32NumExpression>(value);
}

SInt32NumExprPtr Expression::mkSInt32Num(int value) { 
    return std::make_shared<SInt32NumExpression>(value);
}

BoolVarExprPtr Expression::mkBoolVar(std::string name) { 
    return std::make_shared<BoolVarExpression>(name);
}

IntVarExprPtr Expression::mkIntVar(std::string name) { 
    return std::make_shared<IntVarExpression>(name);
}

IntToIntVarExprPtr Expression::mkIntToIntVar(std::string name) { 
    return std::make_shared<IntToIntVarExpression>(name);
}

ToParseExprPtr Expression::parseExpression(std::string s) {
    std::cout << "warning: parseExpression - not implemented!\n"; 
    exit(1);
}

NotExprPtr Expression::mkNot(ExprPtr e1) {
    return std::make_shared<NotExpression>(e1);
}

EqExprPtr Expression::mkEq(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<EqExpression>(e1, e2);
}

DiseqExprPtr Expression::mkDiseq(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<DiseqExpression>(e1, e2);
}

LtExprPtr Expression::mkLt(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<LtExpression>(e1, e2);
}

LeExprPtr Expression::mkLe(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<LeExpression>(e1, e2);
}

GtExprPtr Expression::mkGt(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<GtExpression>(e1, e2);
}

GeExprPtr Expression::mkGe(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<GeExpression>(e1, e2);
}

AndExprPtr Expression::mkAnd(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<AndExpression>(e1, e2);
}

AndExprPtr Expression::mkAnd(std::vector<ExprPtr> es) {
    return std::make_shared<AndExpression>(es);
}
                             
OrExprPtr Expression::mkOr(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<OrExpression>(e1, e2);
}
    
OrExprPtr Expression::mkOr(std::vector<ExprPtr> es) {
    return std::make_shared<OrExpression>(es);
}

XorExprPtr Expression::mkXor(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<XorExpression>(e1, e2);
}

XorExprPtr Expression::mkXor(std::vector<ExprPtr> es) {
    return std::make_shared<XorExpression>(es);
}

IteExprPtr Expression::mkIte(ExprPtr econd, ExprPtr ethen, ExprPtr eelse) {
    return std::make_shared<IteExpression>(econd, ethen, eelse);
}
    
SumExprPtr Expression::mkSum(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<SumExpression>(e1, e2);    
}
    
SubExprPtr Expression::mkSub(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<SubExpression>(e1, e2);
}

MulExprPtr Expression::mkMul(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<MulExpression>(e1, e2);
}

DivExprPtr Expression::mkDiv(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<DivExpression>(e1, e2);    
}

ModExprPtr Expression::mkMod(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<ModExpression>(e1, e2);    
}

AppExprPtr Expression::mkApp(ExprPtr e1, ExprPtr e2) {
    return std::make_shared<AppExpression>(e1, e2); 
}

UpdateExprPtr Expression::mkFunctionUpdate(ExprPtr f, ExprPtr arg, ExprPtr v) {
    return std::make_shared<UpdateExpression>(f, arg, v);     
}

ExprPtr Expression::mkOp(ExprPtr left, ExprPtr right, llvm::CmpInst::Predicate op) {
    switch (op) {
        case llvm::CmpInst::FCMP_UEQ: // True if unordered or equal
        case llvm::CmpInst::FCMP_OEQ: // True if ordered and equal
        case llvm::CmpInst::ICMP_EQ:  // equal
            return mkEq(left, right);
        case llvm::CmpInst::FCMP_ONE: // True if ordered and operands are unequal
        case llvm::CmpInst::FCMP_UNE: // True if unordered or not equal
        case llvm::CmpInst::ICMP_NE:  // not equal
            return mkDiseq(left, right);
        case llvm::CmpInst::FCMP_UGT: // True if unordered or greater than
        case llvm::CmpInst::FCMP_OGT: // True if ordered and greater than
        case llvm::CmpInst::ICMP_UGT: // unsigned greater than
        case llvm::CmpInst::ICMP_SGT: // signed greater than
            return mkGt(left, right);
        case llvm::CmpInst::FCMP_UGE: // True if unordered, greater than, or equal
        case llvm::CmpInst::FCMP_OGE: // True if ordered and greater than or equal
        case llvm::CmpInst::ICMP_UGE: // unsigned greater or equal
        case llvm::CmpInst::ICMP_SGE: // signed greater or equal
            return mkGe(left, right);
        case llvm::CmpInst::FCMP_ULT: //  True if unordered or less than
        case llvm::CmpInst::FCMP_OLT: // True if ordered and less than
        case llvm::CmpInst::ICMP_ULT: // unsigned less than
        case llvm::CmpInst::ICMP_SLT: // signed less than
            return mkLt(left, right);
        case llvm::CmpInst::FCMP_OLE: // True if ordered and less than or equal
        case llvm::CmpInst::FCMP_ULE: // True if unordered, less than, or equal
        case llvm::CmpInst::ICMP_ULE: // unsigned less or equal
        case llvm::CmpInst::ICMP_SLE: // signed less or equal
            return mkLe(left, right);
        default:
            llvm_unreachable("Invalid comparison operator");
            
    }
}

// Signed addition with overlow
//
// Bound: -2^(nbBit-1) <= x <= 2^(nbBit-1)
ExprPtr Expression::mkSSum(ExprPtr left, ExprPtr right, unsigned nbBit) {
    // Let x = y + z, where x, y, z are all signed. 
    // Since the C/C++ language standards do not 
    // specify the resulting value in case of overflow, 
    // we introduce a free integer variable x_0 under 
    // such scenario in the following encoding.
    // x = ITE((−2^(nbBit−1) <= y+z) ^ (y+z<2^(nbBit−1)),y+z,x_0)
    ExprPtr lbound = mkSInt32Num(pow(-2, nbBit-1));
    ExprPtr hbound = mkSInt32Num(pow(2, nbBit-1));
    ExprPtr sum    = mkSum(left, right);
    std::ostringstream oss;
    oss << "_x" << left->getID();
    std::string name = oss.str();
    ExprPtr freex  = mkIntVar(name);
    ExprPtr cond   = mkAnd(mkLe(lbound, sum), mkLt(sum, hbound));
    ExprPtr ite    = mkIte(cond, sum, freex); 
    return ite;
}

// Unsigned addition with overlow
//
// Bound: 0 <= u <= 2^(nbBit)
ExprPtr Expression::mkUSum(ExprPtr left, ExprPtr right, unsigned nbBit) {
    // Let u = v + w, where u, v, w are all unsigned. 
    // Since the C/C++ language standards allows 
    // "wrap around" during overflow, we encode them as follows:
    // u = ITE(v+w<=2^nbBit-1 , v+w, v+w−2^nbBit)
    ExprPtr bound = mkUInt32Num(pow(2, nbBit)-1);
    ExprPtr sum   = mkSum(left, right);
    ExprPtr cond  = mkLe(sum, bound);
    ExprPtr wrap  = mkSub(sum, bound);  
    ExprPtr ite   = mkIte(cond, sum, wrap); 
    return ite;    
}

ExprPtr Expression::mkOp(ExprPtr left, ExprPtr right, llvm::Instruction *i) {
    int opCode = i->getOpcode();
    switch (opCode) {
        case llvm::Instruction::Add:
        case llvm::Instruction::FAdd: /*{
            unsigned n = i->getType()->getIntegerBitWidth();
            return mkUSum(left, right, n);
        } */
             return mkSum(left, right);
        case llvm::Instruction::Sub:
            return mkSub(left, right);
        case llvm::Instruction::Mul:
            return mkMul(left, right);
        case llvm::Instruction::UDiv:
        case llvm::Instruction::SDiv:
            return mkDiv(left, right);
        case llvm::Instruction::And:
            return mkAnd(left, right);
        case llvm::Instruction::Or:
            return mkOr(left, right);
        case llvm::Instruction::Xor:
            return mkXor(left, right);
        case llvm::Instruction::ICmp: {
            llvm::ICmpInst *c = llvm::cast<llvm::ICmpInst>(i);
            return mkOp(left, right, c->getSignedPredicate());
        }
        case llvm::Instruction::URem:
        case llvm::Instruction::SRem:
            // TODO: incorrect semantic
            return mkMod(left, right);
        /** TODO **
        case llvm::Instruction::FSub:
        case llvm::Instruction::FDiv:
        case llvm::Instruction::FMul:
        case llvm::Instruction::FRem:
        case llvm::Instruction::Shl:
        case llvm::Instruction::LShr:   
        case llvm::Instruction::AShr:
         ***/
        default:
            llvm_unreachable("Instruction not supported by SNIPER");
    }
}

llvm::CmpInst::Predicate Expression::negateOp(llvm::CmpInst::Predicate op) {
    switch (op) {
        case llvm::CmpInst::FCMP_UEQ: // True if unordered or equal
            return llvm::CmpInst::FCMP_UNE;
        case llvm::CmpInst::FCMP_OEQ: // True if ordered and equal
            return llvm::CmpInst::FCMP_ONE;
        case llvm::CmpInst::ICMP_EQ:  // equal
            return llvm::CmpInst::ICMP_NE;
        case llvm::CmpInst::FCMP_ONE: // True if ordered and operands are unequal
            return llvm::CmpInst::FCMP_OEQ;
        case llvm::CmpInst::FCMP_UNE: // True if unordered or not equal
            return llvm::CmpInst::FCMP_UEQ;
        case llvm::CmpInst::ICMP_NE:  // not equal
            return llvm::CmpInst::ICMP_EQ;
        case llvm::CmpInst::FCMP_UGT: // True if unordered or greater than
            return llvm::CmpInst::FCMP_ULE;
        case llvm::CmpInst::FCMP_OGT: // True if ordered and greater than
            return llvm::CmpInst::FCMP_OLE;
        case llvm::CmpInst::ICMP_UGT: // unsigned greater than
            return llvm::CmpInst::ICMP_ULE;
        case llvm::CmpInst::ICMP_SGT: // signed greater than
            return llvm::CmpInst::ICMP_SLE;
        case llvm::CmpInst::FCMP_UGE: // True if unordered, greater than, or equal
            return llvm::CmpInst::FCMP_ULT;
        case llvm::CmpInst::FCMP_OGE: // True if ordered and greater than or equal
            return llvm::CmpInst::FCMP_OLT;
        case llvm::CmpInst::ICMP_UGE: // unsigned greater or equal
            return llvm::CmpInst::ICMP_ULT;
        case llvm::CmpInst::ICMP_SGE: // signed greater or equal
            return llvm::CmpInst::ICMP_SLT;
        case llvm::CmpInst::FCMP_ULT: //  True if unordered or less than
            return llvm::CmpInst::FCMP_UGE;
        case llvm::CmpInst::FCMP_OLT: // True if ordered and less than
            return llvm::CmpInst::FCMP_OGE;
        case llvm::CmpInst::ICMP_ULT: // unsigned less than
            return llvm::CmpInst::ICMP_UGE;
        case llvm::CmpInst::ICMP_SLT: // signed less than
            return llvm::CmpInst::ICMP_SGE;
        case llvm::CmpInst::FCMP_OLE: // True if ordered and less than or equal
            return llvm::CmpInst::FCMP_UGT;
        case llvm::CmpInst::FCMP_ULE: // True if unordered, less than, or equal
            return llvm::CmpInst::FCMP_UGT;
        case llvm::CmpInst::ICMP_ULE: // unsigned less or equal
            return llvm::CmpInst::ICMP_UGT;
        case llvm::CmpInst::ICMP_SLE: // signed less or equal
            return llvm::CmpInst::ICMP_SGT;
        default:
            llvm_unreachable("Invalid comparison operator");
    }   
}

ExprPtr Expression::getExprFromValue(llvm::Value *v) {
    llvm::Type *t = v->getType();
    // Char/Integer/Double types
    if (t->isIntegerTy(8)  || t->isIntegerTy(16) 
        || t->isIntegerTy(32)   || t->isIntegerTy(64)) {
        if (llvm::ConstantInt *CI = llvm::dyn_cast<llvm::ConstantInt>(v)) {
            int value = CI->getSExtValue();
            return mkSInt32Num(value);
        } else {
            if (llvm::isa<llvm::GlobalVariable>(v)) {
                return mkIntVar(v->getName().str());
            }
            std::string extra("");
            llvm::Value::use_iterator it = v->use_begin();
            if (it!=v->use_end()) {
                if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(*it)) {
                    llvm::Function *F = I->getParent()->getParent();
                    if (F) {
                        extra = "_"+F->getName().str();
                    }
                }
            }
            return mkIntVar(v->getName().str()+extra);
        }
    } 
    // Bool type
    else if (t->isIntegerTy(1)) {
        if (llvm::ConstantInt *CI = llvm::dyn_cast<llvm::ConstantInt>(v)) {
            int value = CI->getSExtValue();
            if (value==0) {
                return mkFalse();
            } else {
                return mkTrue();
            }
        } else {
            if (llvm::isa<llvm::GlobalVariable>(v)) {
                return mkBoolVar(v->getName().str());
            }
            std::string extra("");
            llvm::Value::use_iterator it = v->use_begin();
            if (it!=v->use_end()) {
                if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(*it)) {
                    llvm::Function *F = I->getParent()->getParent();
                    if (F) {
                        extra = "_"+F->getName().str();
                    }
                }
            }  
            return Expression::mkBoolVar(v->getName().str()+extra);
        }
    // Pointer, Struct, Array
    } else if(t->isPointerTy()) {
        const llvm::Type *cty = t->getContainedType(0);
        // Array on integer
        if(const llvm::ArrayType *aty = llvm::dyn_cast<llvm::ArrayType>(cty)) {
            const llvm::Type *eltty = aty->getElementType();
            // Char/Integer/Double types
            if (eltty->isIntegerTy(8)  || eltty->isIntegerTy(16) 
                || eltty->isIntegerTy(32) || eltty->isIntegerTy(64)) {
                if (llvm::isa<llvm::GlobalVariable>(v)) {
                    return mkIntToIntVar(v->getName().str());
                }
                std::string extra("");
                llvm::Value::use_iterator it = v->use_begin();
                if (it!=v->use_end()) {
                    if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(*it)) {
                        llvm::Function *F = I->getParent()->getParent();
                        if (F) {
                            extra = "_"+F->getName().str();
                        }
                    }
                }
                return mkIntToIntVar(v->getName().str()+extra);
            } 
            // Bool type
            else /*if (eltty->isIntegerTy(1))*/ {
                std::cout << "error: unsupported type.\n";
                exit(1);
            }
        } else {
            std::cout << "error: unsupported type.\n";
            exit(1);
        }
    } else {
        std::cout << "error: unsupported type.\n";
        exit(1);
    }
}

// TODO: order is taken into account in the equality of expressions,
//       which is not correct.
bool operator== (ExprPtr e1, ExprPtr e2) {
    assert((e1.get() && e2.get()) && "Unexpected null expressions");
    if (e1->getOpCode()!=e2->getOpCode()) {
        return false;
    }
    // Same reference
    if (e1.get()==e2.get()) {
        return true;
    }
    // Op code of e1 == op code of e2
    // TODO: use base classes
    switch (e1->getOpCode()) {
        case Expression::True:
            return true;
        case Expression::False:
            return true;
        case Expression::UInt32Num: {
            UInt32NumExprPtr i1 =
            std::static_pointer_cast<UInt32NumExpression>(e1);
            UInt32NumExprPtr i2 =
            std::static_pointer_cast<UInt32NumExpression>(e2);
            return (i1->getValue()==i2->getValue());
        }
        case Expression::SInt32Num: {
            SInt32NumExprPtr i1 =
            std::static_pointer_cast<SInt32NumExpression>(e1);
            SInt32NumExprPtr i2 =
            std::static_pointer_cast<SInt32NumExpression>(e2);
            return (i1->getValue()==i2->getValue());
        }
        case Expression::BoolVar: {
            BoolVarExprPtr be1 = std::static_pointer_cast<BoolVarExpression>(e1);
            BoolVarExprPtr be2 = std::static_pointer_cast<BoolVarExpression>(e2);
            return (be1->getName()==be2->getName());
        }
        case Expression::IntVar: {
            IntVarExprPtr ie1 = std::static_pointer_cast<IntVarExpression>(e1);
            IntVarExprPtr ie2 = std::static_pointer_cast<IntVarExpression>(e2);
            return (ie1->getName()==ie2->getName());
        }
        case Expression::IntToIntVar: {
            IntToIntVarExprPtr ie1 =
            std::static_pointer_cast<IntToIntVarExpression>(e1);
            IntToIntVarExprPtr ie2 =
            std::static_pointer_cast<IntToIntVarExpression>(e2);
            return (ie1->getName()==ie2->getName());
        }
        case Expression::ToParse: {
            ToParseExprPtr te1 = std::static_pointer_cast<ToParseExpression>(e1);
            ToParseExprPtr te2 = std::static_pointer_cast<ToParseExpression>(e2);
            return (te1->getString()==te2->getString());
        }
        case Expression::Gt: {
            GtExprPtr gt1 = std::static_pointer_cast<GtExpression>(e1);
            GtExprPtr gt2 = std::static_pointer_cast<GtExpression>(e2);
            if (gt1->getExpr1()!=gt2->getExpr1())
                return false;
            return (gt1->getExpr2()==gt2->getExpr2());
        }
        case Expression::Ge: {
            GeExprPtr ge1 = std::static_pointer_cast<GeExpression>(e1);
            GeExprPtr ge2 = std::static_pointer_cast<GeExpression>(e2);
            if (ge1->getExpr1() != ge2->getExpr1())
                return false;
            return (ge1->getExpr2()==ge2->getExpr2());
        }
        case Expression::Le: {
            LeExprPtr le1 = std::static_pointer_cast<LeExpression>(e1);
            LeExprPtr le2 = std::static_pointer_cast<LeExpression>(e2);
            if (le1->getExpr1()!=le2->getExpr1())
                return false;
            return (le1->getExpr2()==le2->getExpr2());
        }
        case Expression::Lt: {
            LtExprPtr lt1 = std::static_pointer_cast<LtExpression>(e1);
            LtExprPtr lt2 = std::static_pointer_cast<LtExpression>(e2);
            if (lt1->getExpr1()!=lt2->getExpr1())
                return false;
            return (lt1->getExpr2()==lt2->getExpr2());
        }
        case Expression::Diseq: {
            DiseqExprPtr de1 = std::static_pointer_cast<DiseqExpression>(e1);
            DiseqExprPtr de2 = std::static_pointer_cast<DiseqExpression>(e2);
            if (de1->getExpr1()!=de2->getExpr1())
                return false;
            return (de1->getExpr2()==de2->getExpr2());
        }
        case Expression::Eq: {
            EqExprPtr ee1 = std::static_pointer_cast<EqExpression>(e1);
            EqExprPtr ee2 = std::static_pointer_cast<EqExpression>(e2);
            if (ee1->getExpr1()!=ee2->getExpr1())
                return false;
            return (ee1->getExpr2()==ee2->getExpr2());
        }
        case Expression::Not: {
            NotExprPtr ne1 = std::static_pointer_cast<NotExpression>(e1);
            NotExprPtr ne2 = std::static_pointer_cast<NotExpression>(e2);
            return (ne1->get()==ne2->get());
        }
        case Expression::And: {
            AndExprPtr ae1 = std::static_pointer_cast<AndExpression>(e1);
            AndExprPtr ae2 = std::static_pointer_cast<AndExpression>(e2);
            std::vector<ExprPtr> es1 = ae1->getExprs();
            std::vector<ExprPtr> es2 = ae2->getExprs();
            if (es1.size()!=es2.size())
                return false;
            const unsigned n = es1.size();
            assert(n==0 && "Empty AndExpression");
            if (n==1)
                return (es1.back()==es2.back());
            for(unsigned i=0; i<n; i++) {
                if (es1[i]!=es2[i])
                    return false;
            }
            return true;
        }
        case Expression::Or: {
            OrExprPtr oe1 = std::static_pointer_cast<OrExpression>(e1);
            OrExprPtr oe2 = std::static_pointer_cast<OrExpression>(e2);
            std::vector<ExprPtr> es1 = oe1->getExprs();
            std::vector<ExprPtr> es2 = oe2->getExprs();
            if (es1.size()!=es2.size())
                return false;
            const unsigned n = es1.size();
            assert(n==0 && "Empty MulExpression");
            if (n==1)
                return (es1.back()==es2.back());
            for(unsigned i=0; i<n; i++) {
                if (es1[i]!=es2[i])
                    return false;
            }
            return true;
        }
        case Expression::Xor: {
            XorExprPtr xe1 = std::static_pointer_cast<XorExpression>(e1);
            XorExprPtr xe2 = std::static_pointer_cast<XorExpression>(e2);
            std::vector<ExprPtr> es1 = xe1->getExprs();
            std::vector<ExprPtr> es2 = xe2->getExprs();
            if (es1.size()!=es2.size())
                return false;
            const unsigned n = es1.size();
            assert(n==0 && "Empty XorExpression");
            if (n==1)
                return (es1.back()==es2.back());
            for(unsigned i=0; i<n; i++) {
                if (es1[i]!=es2[i])
                    return false;
            }
            return true;
        }
        case Expression::Sum: {
            SumExprPtr se1 = std::static_pointer_cast<SumExpression>(e1);
            SumExprPtr se2 = std::static_pointer_cast<SumExpression>(e2);
            std::vector<ExprPtr> es1 = se1->getExprs();
            std::vector<ExprPtr> es2 = se2->getExprs();
            if (es1.size()!=es2.size())
                return false;
            const unsigned n = es1.size();
            assert(n==0 && "Empty SumExpression");
            if (n==1)
                return (es1.back()==es2.back());
            for(unsigned i=0; i<n; i++) {
                if (es1[i]!=es2[i])
                    return false;
            }
            return true;
        }
        case Expression::Sub:  {
            SubExprPtr se1 = std::static_pointer_cast<SubExpression>(e1);
            SubExprPtr se2 = std::static_pointer_cast<SubExpression>(e2);
            std::vector<ExprPtr> es1 = se1->getExprs();
            std::vector<ExprPtr> es2 = se2->getExprs();
            if (es1.size()!=es2.size())
                return false;
            const unsigned n = es1.size();
            assert(n==0 && "Empty SubExpression");
            if (n==1)
                return (es1.back()==es2.back());
            for(unsigned i=0; i<n; i++) {
                if (es1[i]!=es2[i])
                    return false;
            }
            return true;
        }
        case Expression::Mul: {
            MulExprPtr me1 = std::static_pointer_cast<MulExpression>(e1);
            MulExprPtr me2 = std::static_pointer_cast<MulExpression>(e2);
            std::vector<ExprPtr> es1 = me1->getExprs();
            std::vector<ExprPtr> es2 = me2->getExprs();
            if (es1.size()!=es2.size())
                return false;
            const unsigned n = es1.size();
            assert(n==0 && "Empty MulExpression");
            if (n==1)
                return (es1.back()==es2.back());
            for(unsigned i=0; i<n; i++) {
                if (es1[i]!=es2[i])
                    return false;
            }
            return true;
        }
        case Expression::Ite: {
            IteExprPtr ie1 = std::static_pointer_cast<IteExpression>(e1);
            IteExprPtr ie2 = std::static_pointer_cast<IteExpression>(e2);
            if (ie1->getExpr1()==ie2->getExpr1())
                return false;
            if (ie1->getExpr2()!=ie2->getExpr2())
                return false;
            return (ie1->getExpr3()==ie2->getExpr3());
        }
        case Expression::App: {
            AppExprPtr ae1 = std::static_pointer_cast<AppExpression>(e1);
            AppExprPtr ae2 = std::static_pointer_cast<AppExpression>(e2);
            if (ae1->getExpr1()!=ae2->getExpr1())
                return false;
            return (ae1->getExpr2()==ae2->getExpr2());
        }
        case Expression::Update: {
            UpdateExprPtr ue1 = std::static_pointer_cast<UpdateExpression>(e1);
            UpdateExprPtr ue2 = std::static_pointer_cast<UpdateExpression>(e2);
            if (ue1->getExpr1()!=ue2->getExpr1())
                return false;
            if (ue1->getExpr2()!=ue2->getExpr2())
                return false;
            return (ue1->getExpr3()==ue2->getExpr3());
        }
        case Expression::Div: {
            DivExprPtr de1 = std::static_pointer_cast<DivExpression>(e1);
            DivExprPtr de2 = std::static_pointer_cast<DivExpression>(e2);
            if (de1->getExpr1()!=de2->getExpr1())
                return false;
            return (de1->getExpr2()==de2->getExpr2());
        }
        case Expression::Mod: {
            ModExprPtr de1 = std::static_pointer_cast<ModExpression>(e1);
            ModExprPtr de2 = std::static_pointer_cast<ModExpression>(e2);
            if (de1->getExpr1()!=de2->getExpr1())
                return false;
            return (de1->getExpr2()==de2->getExpr2());
        }
        default:
            llvm_unreachable("Invalid Expression subclass");
            break;
    }
    return false;
}

bool operator!= (ExprPtr e1, ExprPtr e2) {
    return !(e1 == e2);
}

// TODO: add ostream operator to all kind of expressions
std::ostream& operator<<(std::ostream& os, const ExprPtr e) {
    // Redirect cout
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::ostringstream strCout;
    std::cout.rdbuf(strCout.rdbuf());
    // Write to strCout
    e->dump();
    // Restore old cout
    std::cout.rdbuf(oldCoutStreamBuf);
    // Write to os
    os << strCout.str();
    return os;
}
