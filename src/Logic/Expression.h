/**
 * Expression.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/01
 * @copyright : NII 2013
 */


#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"

class Expression;
class SingleExpression;
class TrueExpression;
class FalseExpression;
class UInt32NumExpression;
class SInt32NumExpression;
class BoolVarExpression;
class IntVarExpression;
class IntToIntVarExpression;
class ToParseExpression;
class BinaryExpression;
class TrinaryExpression;
class UnaryExpression;
class GtExpression;
class GeExpression;
class LeExpression;
class LtExpression;
class DiseqExpression;
class EqExpression;
class NotExpression;
class AndExpression;
class OrExpression;
class SumExpression;
class SubExpression;
class MulExpression;
class IteExpression;
class AppExpression;
class UpdateExpression;
class DivExpression;
class ModExpression;
class XorExpression;

typedef std::shared_ptr<Expression>             ExprPtr;
typedef std::shared_ptr<SingleExpression>       SingleExprPtr;
typedef std::shared_ptr<TrueExpression>         TrueExprPtr;
typedef std::shared_ptr<FalseExpression>        FalseExprPtr;
typedef std::shared_ptr<UInt32NumExpression>    UInt32NumExprPtr;
typedef std::shared_ptr<SInt32NumExpression>    SInt32NumExprPtr;
typedef std::shared_ptr<BoolVarExpression>      BoolVarExprPtr;
typedef std::shared_ptr<IntVarExpression>       IntVarExprPtr;
typedef std::shared_ptr<IntToIntVarExpression>  IntToIntVarExprPtr;
typedef std::shared_ptr<ToParseExpression>      ToParseExprPtr;
typedef std::shared_ptr<BinaryExpression>       BinaryExprPtr;
typedef std::shared_ptr<TrinaryExpression>      TrinaryExprPtr;
typedef std::shared_ptr<UnaryExpression>        UnaryExprPtr;
typedef std::shared_ptr<GtExpression>           GtExprPtr;
typedef std::shared_ptr<GeExpression>           GeExprPtr;
typedef std::shared_ptr<LeExpression>           LeExprPtr;
typedef std::shared_ptr<LtExpression>           LtExprPtr;
typedef std::shared_ptr<DiseqExpression>        DiseqExprPtr;
typedef std::shared_ptr<EqExpression>           EqExprPtr;
typedef std::shared_ptr<NotExpression>          NotExprPtr;
typedef std::shared_ptr<AndExpression>          AndExprPtr;
typedef std::shared_ptr<OrExpression>           OrExprPtr;
typedef std::shared_ptr<SumExpression>          SumExprPtr;
typedef std::shared_ptr<SubExpression>          SubExprPtr;
typedef std::shared_ptr<MulExpression>          MulExprPtr;
typedef std::shared_ptr<IteExpression>          IteExprPtr;
typedef std::shared_ptr<AppExpression>          AppExprPtr;
typedef std::shared_ptr<UpdateExpression>       UpdateExprPtr;
typedef std::shared_ptr<DivExpression>          DivExprPtr;
typedef std::shared_ptr<ModExpression>          ModExprPtr;
typedef std::shared_ptr<XorExpression>          XorExprPtr;

// Pretty printer for Expression
std::ostream& operator<<(std::ostream& os, const ExprPtr e);

class Expression {
    
public:
    static const unsigned True          = 1;
    static const unsigned False         = 2;
    static const unsigned UInt32Num     = 3;
    static const unsigned SInt32Num     = 4;
    static const unsigned BoolVar       = 5;
    static const unsigned IntVar        = 6;
    static const unsigned IntToIntVar   = 7;
    static const unsigned ToParse       = 8;
    static const unsigned Gt            = 9;
    static const unsigned Ge            = 10;
    static const unsigned Le            = 11;
    static const unsigned Lt            = 12;
    static const unsigned Diseq         = 13;
    static const unsigned Eq            = 14;
    static const unsigned Not           = 15;
    static const unsigned And           = 16;
    static const unsigned Or            = 17;
    static const unsigned Sum           = 18;
    static const unsigned Sub           = 19;
    static const unsigned Mul           = 20;
    static const unsigned Ite           = 21;
    static const unsigned App           = 22;
    static const unsigned Update        = 23;
    static const unsigned Div           = 24;
    static const unsigned Mod           = 25;
    static const unsigned Xor           = 26;
    
private:
    static unsigned ID;
    const unsigned currentID;
    bool soft;
    bool valid;
    llvm::Instruction *instruction;
    unsigned line;
protected:
    static unsigned NbIntVariables;
    static unsigned NbBoolVariables;
protected:
    Expression();
    virtual ~Expression() { }
    
public:
    static TrueExprPtr         mkTrue();
    static FalseExprPtr        mkFalse();
    static UInt32NumExprPtr    mkUInt32Num(unsigned value);
    static SInt32NumExprPtr    mkSInt32Num(int value);
    static BoolVarExprPtr      mkBoolVar(std::string name);
    static IntVarExprPtr       mkIntVar(std::string name);
    static IntToIntVarExprPtr  mkIntToIntVar(std::string name);
    static ToParseExprPtr      parseExpression(std::string s);
    
    static NotExprPtr      mkNot(ExprPtr e1);
    static EqExprPtr       mkEq(ExprPtr e1, ExprPtr e2);
    static DiseqExprPtr    mkDiseq(ExprPtr e1, ExprPtr e2);
    static LtExprPtr       mkLt(ExprPtr e1, ExprPtr e2);
    static LeExprPtr       mkLe(ExprPtr e1, ExprPtr e2);
    static GtExprPtr       mkGt(ExprPtr e1, ExprPtr e2);
    static GeExprPtr       mkGe(ExprPtr e1, ExprPtr e2);
    static AndExprPtr      mkAnd(ExprPtr e1, ExprPtr e2);
    static AndExprPtr      mkAnd(std::vector<ExprPtr> es);
    static OrExprPtr       mkOr(ExprPtr e1, ExprPtr e2);
    static OrExprPtr       mkOr(std::vector<ExprPtr> es);
    static XorExprPtr      mkXor(ExprPtr e1, ExprPtr e2);
    static XorExprPtr      mkXor(std::vector<ExprPtr> es);
    static IteExprPtr      mkIte(ExprPtr econd, ExprPtr ethen, ExprPtr eelse);
    static SumExprPtr      mkSum(ExprPtr e1, ExprPtr e2);
    static ExprPtr         mkSSum(ExprPtr e1, ExprPtr e2, unsigned nbBit);
    static ExprPtr         mkUSum(ExprPtr e1, ExprPtr e2, unsigned nbBit);
    static SubExprPtr      mkSub(ExprPtr e1, ExprPtr e2);
    static MulExprPtr      mkMul(ExprPtr e1, ExprPtr e2);
    static DivExprPtr      mkDiv(ExprPtr e1, ExprPtr e2);
    static ModExprPtr      mkMod(ExprPtr e1, ExprPtr e2);
    static AppExprPtr      mkApp(ExprPtr e1, ExprPtr e2);
    static UpdateExprPtr   mkFunctionUpdate(ExprPtr f, ExprPtr arg, ExprPtr v);
    static ExprPtr         mkOp(ExprPtr left, ExprPtr right, llvm::CmpInst::Predicate op);
    static ExprPtr         mkOp(ExprPtr left, ExprPtr right, llvm::Instruction *i);
    
    friend bool operator== (ExprPtr e1, ExprPtr e2);
    friend bool operator!= (ExprPtr e1, ExprPtr e2);
    
    static llvm::CmpInst::Predicate negateOp(llvm::CmpInst::Predicate op);
    static ExprPtr getExprFromValue(llvm::Value *v);
    
    static unsigned getNbIntVariables() {
        return NbIntVariables;
    }
    static unsigned getNbBoolVariables() {
        return NbBoolVariables;
    }
    bool isValid() {
        return valid;
    }
    void invalidate() {
        valid = false;;
    }
    bool isSoft() {
        return soft;
    }
    bool isHard() {
        return !soft;
    }
    void setLine(unsigned l) {
        line = l;
    }
    void setHard() {
        soft = false;
    }
    void setSoft() {
        soft = true;
    }
    unsigned getLine() {
        return line;
    }
    llvm::Instruction* getInstruction() {
        return instruction;
    }
    void setInstruction(llvm::Instruction *i) {
        instruction = i;
    }
    llvm::BasicBlock* getBB() {
        if (instruction==NULL) {
            //std::cout << "warning: no instruction for <";
            //this->dump();
            //std::cout << ">" << std::endl;
            return NULL;
        }
        return instruction->getParent();
    }
    unsigned getID() {
        return currentID;   
    }
    virtual unsigned getOpCode() = 0;
    virtual void dump() = 0;
    
};

class SingleExpression : public Expression {
protected:
    std::string name;
    SingleExpression(std::string _name) 
    : name(_name) { }
public:
    std::string getName() {
        return name;
    }
};

class BinaryExpression : public Expression {
protected:
    ExprPtr e1;
    ExprPtr e2;
    BinaryExpression(ExprPtr _e1, ExprPtr _e2) 
    : e1(_e1),e2(_e2) { }
public:
    ExprPtr getExpr1() {
        return e1;
    }
    ExprPtr getExpr2() {
        return e2;
    }
};

class TrinaryExpression : public Expression {
protected:
    ExprPtr e1;
    ExprPtr e2;
    ExprPtr e3;
    TrinaryExpression(ExprPtr _e1, ExprPtr _e2, ExprPtr _e3 )
    : e1(_e1),e2(_e2),e3(_e3) { }
public:
    ExprPtr getExpr1() {
        return e1;
    }
    ExprPtr getExpr2() {
        return e2;
    }
    ExprPtr getExpr3() {
        return e3;
    }    
};

class UnaryExpression : public Expression {
protected:
    std::vector<ExprPtr> exprs;
    UnaryExpression(std::vector<ExprPtr> es) 
    : exprs(es) { }
    UnaryExpression(ExprPtr _e1, ExprPtr _e2) {
        exprs.push_back(_e1);
        exprs.push_back(_e2);
    }
    virtual ~UnaryExpression() {
        exprs.clear();
    }
public:
    std::vector<ExprPtr> getExprs() {
        return exprs;
    }
};

class TrueExpression : public Expression {
public:
    TrueExpression() { }
    virtual unsigned getOpCode() {
        return Expression::True;
    }
    virtual void dump() {
        std::cout << "true";
    }
};

class FalseExpression : public Expression {
public:
    FalseExpression() { }
    virtual unsigned getOpCode() {
        return Expression::False;
    }
    virtual void dump() {
        std::cout << "false";
    }
};

class UInt32NumExpression : public Expression {
protected:
    unsigned num;
public:
    UInt32NumExpression(unsigned n) : num(n) { }
    unsigned getValue() {
        return num;
    }
    virtual unsigned getOpCode() {
        return Expression::UInt32Num;
    }
    virtual void dump() {
        std::cout << num;
    }
};

class SInt32NumExpression : public Expression {
protected:
    int num;
public:
    SInt32NumExpression(int n) : num(n) { }
    int getValue() {
        return num;
    }
    virtual unsigned getOpCode() {
        return Expression::SInt32Num;
    }
    virtual void dump() {
        std::cout << num;
    }
};

class BoolVarExpression : public SingleExpression {
public:
    BoolVarExpression(std::string _name) 
    : SingleExpression(_name) { 
         NbBoolVariables++;
    }
    virtual unsigned getOpCode() {
        return Expression::BoolVar;
    }
    virtual void dump() {
        std::cout << name;
    }
};

class IntVarExpression : public SingleExpression {
public:
    IntVarExpression(std::string _name) 
    : SingleExpression(_name) { 
        NbIntVariables++;
    }
    virtual unsigned getOpCode() {
        return Expression::IntVar;
    }
    virtual void dump() {
        std::cout << name;
    }
};

class IntToIntVarExpression : public SingleExpression {
public:
    IntToIntVarExpression(std::string _name) 
    : SingleExpression(_name) { }
    virtual unsigned getOpCode() {
        return Expression::IntToIntVar;
    }
    virtual void dump() {
        std::cout << name;
    }
};

class ToParseExpression : public Expression {
private:
    std::string str;
public:
    ToParseExpression(std::string s);
    std::string getString() {
        return str;
    }
    virtual unsigned getOpCode() {
        return Expression::ToParse;
    }
    virtual void dump() {
        std::cout << str;
    }
};

class GeExpression : public BinaryExpression {
public:
    GeExpression(ExprPtr _e1, ExprPtr _e2) 
    : BinaryExpression(_e1, _e2) { }
    virtual unsigned getOpCode() {
        return Expression::Ge;
    }
    virtual void dump() {
        std::cout << "(>= ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class GtExpression : public BinaryExpression {
public:
    GtExpression(ExprPtr _e1, ExprPtr _e2) 
    : BinaryExpression(_e1, _e2) { }
    virtual unsigned getOpCode() {
        return Expression::Gt;
    }
    virtual void dump() {
        std::cout << "(> ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class LeExpression : public BinaryExpression {
public:
    LeExpression(ExprPtr _e1, ExprPtr _e2) 
    : BinaryExpression(_e1, _e2) { }
    virtual unsigned getOpCode() {
        return Expression::Le;
    }
    virtual void dump() {
        std::cout << "(<= ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class LtExpression : public BinaryExpression {
public:
    LtExpression(ExprPtr _e1, ExprPtr _e2) 
    : BinaryExpression(_e1, _e2) { }
    virtual unsigned getOpCode() {
        return Expression::Lt;
    }
    virtual void dump() {
        std::cout << "(< ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class DiseqExpression : public BinaryExpression {
public:
    DiseqExpression(ExprPtr _e1, ExprPtr _e2) 
    : BinaryExpression(_e1, _e2) { }
    virtual unsigned getOpCode() {
        return Expression::Diseq;
    }
    virtual void dump() {
        std::cout << "(!= ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class EqExpression : public BinaryExpression {
public:
    EqExpression(ExprPtr _e1, ExprPtr _e2) 
    : BinaryExpression(_e1, _e2) { }
    virtual unsigned getOpCode() {
        return Expression::Eq;
    }
    virtual void dump() {
        std::cout << "(= ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class NotExpression : public Expression {
protected:
    ExprPtr e;
public:
    NotExpression(ExprPtr _e) : e(_e) { }
    ExprPtr get() { 
        return e;
    }
    virtual unsigned getOpCode() {
        return Expression::Not;
    }
    virtual void dump() {
        std::cout << "(not ";
        e->dump();
        std::cout << ")";
    }
};

class AndExpression : public UnaryExpression {
public:
    AndExpression(std::vector<ExprPtr> es) 
    : UnaryExpression(es) { }
    AndExpression(ExprPtr _e1, ExprPtr _e2) 
    : UnaryExpression(_e1, _e2) {}
    virtual unsigned getOpCode() {
        return Expression::And;
    }
    virtual void dump() {
        if (exprs.size()==1) {
            ExprPtr e = exprs.back();
            e->dump();
        } else {
            std::cout << "(and ";
            std::vector<ExprPtr>::iterator it;
            for(it = exprs.begin(); it != exprs.end(); ++it) {
                ExprPtr e = *it;
                e->dump();
                if (it+1!=exprs.end()) {
                    std::cout << " ";
                }
            }
            std::cout << ")";
        }
    }
};

class OrExpression : public UnaryExpression {
public:
    OrExpression(std::vector<ExprPtr> es) 
    : UnaryExpression(es) { }
    OrExpression(ExprPtr _e1, ExprPtr _e2) 
    : UnaryExpression(_e1, _e2) {}
    virtual unsigned getOpCode() {
        return Expression::Or;
    }
    virtual void dump() {
        if (exprs.size()==1) {
            ExprPtr e = exprs.back();
            e->dump();
        } else {
            std::cout << "(or ";
            std::vector<ExprPtr>::iterator it;
            for(it = exprs.begin(); it != exprs.end(); ++it) {
                ExprPtr e = *it;
                e->dump();
                if (it+1!=exprs.end()) {
                    std::cout << " ";
                }
            }
            std::cout << ")";
        }
    }
};

class XorExpression : public UnaryExpression {
public:
    XorExpression(std::vector<ExprPtr> es) 
    : UnaryExpression(es) { }
    XorExpression(ExprPtr _e1, ExprPtr _e2) 
    : UnaryExpression(_e1, _e2) {}
    virtual unsigned getOpCode() {
        return Expression::Xor;
    }
    virtual void dump() {
        if (exprs.size()==1) {
            ExprPtr e = exprs.back();
            e->dump();
        } else {
            std::cout << "(xor ";
            std::vector<ExprPtr>::iterator it;
            for(it = exprs.begin(); it != exprs.end(); ++it) {
                ExprPtr e = *it;
                e->dump();
                if (it+1!=exprs.end()) {
                    std::cout << " ";
                }
            }
            std::cout << ")";
        }
    }
};

class SumExpression : public UnaryExpression {
public:
    SumExpression(std::vector<ExprPtr> es) 
    : UnaryExpression(es) { }
    SumExpression(ExprPtr _e1, ExprPtr _e2) 
    : UnaryExpression(_e1, _e2) {}
    virtual unsigned getOpCode() {
        return Expression::Sum;
    }
    virtual void dump() {
        if (exprs.size()==1) {
            ExprPtr e = exprs.back();
            e->dump();
        } else {
            std::cout << "(+ ";
            std::vector<ExprPtr>::iterator it;
            for(it = exprs.begin(); it != exprs.end(); ++it) {
                ExprPtr e = *it;
                e->dump();
                if (it+1!=exprs.end()) {
                    std::cout << " ";
                }
            }
            std::cout << ")";
        }
    }
};

class SubExpression : public UnaryExpression {
public:
    SubExpression(std::vector<ExprPtr> es) 
    : UnaryExpression(es) { }
    SubExpression(ExprPtr _e1, ExprPtr _e2) 
    : UnaryExpression(_e1, _e2) {}
    virtual unsigned getOpCode() {
        return Expression::Sub;
    }
    virtual void dump() {
        if (exprs.size()==1) {
            ExprPtr e = exprs.back();
            e->dump();
        } else {
            std::cout << "(- ";
            std::vector<ExprPtr>::iterator it;
            for(it = exprs.begin(); it != exprs.end(); ++it) {
                ExprPtr e = *it;
                e->dump();
                if (it+1!=exprs.end()) {
                    std::cout << " ";
                }
            }
            std::cout << ")";
        }
    }
};

class MulExpression : public UnaryExpression {
public:
    MulExpression(std::vector<ExprPtr> es) 
    : UnaryExpression(es) { }
    MulExpression(ExprPtr _e1, ExprPtr _e2) 
    : UnaryExpression(_e1, _e2) {}
    virtual unsigned getOpCode() {
        return Expression::Mul;
    }
    virtual void dump() {
        if (exprs.size()==1) {
            ExprPtr e = exprs.back();
            e->dump();
        } else {
            std::cout << "(* ";
            std::vector<ExprPtr>::iterator it;
            for(it = exprs.begin(); it != exprs.end(); ++it) {
                ExprPtr e = *it;
                e->dump();
                if (it+1!=exprs.end()) {
                    std::cout << " ";
                }
            }
            std::cout << ")";
        }
    }
};

class DivExpression : public BinaryExpression {
public:
    DivExpression(ExprPtr e1, ExprPtr e2) 
    :  BinaryExpression(e1, e2) {}
    virtual unsigned getOpCode() {
        return Expression::Div;
    }
    virtual void dump() {
        std::cout << "(div ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class ModExpression : public BinaryExpression {
public:
    ModExpression(ExprPtr e1, ExprPtr e2) 
    :  BinaryExpression(e1, e2) {}
    virtual unsigned getOpCode() {
        return Expression::Mod;
    }
    virtual void dump() {
        std::cout << "(mod ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class IteExpression : public TrinaryExpression {
public:
    IteExpression(ExprPtr cond, ExprPtr then, ExprPtr eelse)
    : TrinaryExpression(cond, then, eelse) {}
    virtual unsigned getOpCode() {
        return Expression::Ite;
    }
    virtual void dump() {
        std::cout << "(ite ";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << " ";
        e3->dump();
        std::cout << ")";
    }
};

class AppExpression : public BinaryExpression {
public:
    AppExpression(ExprPtr f, ExprPtr arg) 
    :  BinaryExpression(f, arg) {}
    virtual unsigned getOpCode() {
        return Expression::App;
    }
    virtual void dump() {
        std::cout << "(";
        e1->dump();
        std::cout << " ";
        e2->dump();
        std::cout << ")";
    }
};

class UpdateExpression : public TrinaryExpression {
public:
    UpdateExpression(ExprPtr f, ExprPtr arg, ExprPtr v) 
    :  TrinaryExpression(f, arg, v) {}
    virtual unsigned getOpCode() {
        return Expression::Update;
    }
    virtual void dump() {
        std::cout << "(update ";
        e1->dump();
        std::cout << " (";
        e2->dump();
        std::cout << ") ";
        e3->dump();
        std::cout << ")";
    }
};



#endif
