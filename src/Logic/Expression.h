/**
 * \file Expression.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   26 January 2015
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
#include "llvm/DebugInfo.h"

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

/**
 * Pretty printer for expressions.
 */
std::ostream& operator<<(std::ostream& os, const ExprPtr e);

/**
 * \class Expression
 *
 * \brief Parent class for expressions.
 * 
 * An expression, also called clause or constraint, is 
 * representated by an abstract syntax tree.
 */
class Expression {
    
public:
     /**@{
      * Op codes */
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
    /**@}*/

private:
    /**
     * Unique ID number to be assigned to each expression.
     */
    static unsigned ID;
    const unsigned currentID;
    /**
     * True if the expresssion is soft (retractable).
     */
    bool soft;
    /**
     * LLVM instruction from which the expression was encoded.
     * In some cases, this variable can be null.
     */
    llvm::Instruction *instruction;
    /**
     * Line number in the original source code of the LLVM expression 
     * from which the expression was encoded.
     */
    unsigned line;
protected:
    /**
     * Number of integer variables created.
     */
    static unsigned NbIntVariables;
    /**
     * Number of Boolean variables created.
     */
    static unsigned NbBoolVariables;
protected:
    Expression()
    : currentID(ID++), soft(true), instruction(NULL), line(0) { }
    /**
     * Destructor.
     */
    virtual ~Expression() { }
    
public:
    /**
     * Return an expression representing \a true.
     */
    static TrueExprPtr mkTrue();
    /**
     * Return an expression representing \a false.
     */
    static FalseExprPtr mkFalse();
    /**
     * Return an expression representing the given unsigned integer.
     */
    static UInt32NumExprPtr mkUInt32Num(unsigned value);
    /**
     * Return an expression representing the given signed integer.
     */
    static SInt32NumExprPtr mkSInt32Num(int value);
    /**
     * Return a new boolean variable expression (instance).
     */
    static BoolVarExprPtr mkBoolVar(std::string name);
    /**
     * Return a new integer variable expression (instance).
     */
    static IntVarExprPtr mkIntVar(std::string name);
    /**
     * Return a new function variable expression (instance).
     */
    static IntToIntVarExprPtr mkIntToIntVar(std::string name);
    /**
     * Not implemented.
     */
    static ToParseExprPtr parseExpression(std::string s);
    /**
     * Return an expression representing \a (not e1).
     */
    static NotExprPtr mkNot(ExprPtr e1);
    /**
     * Return an expression representing \a e1 = e2.
     */
    static EqExprPtr mkEq(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 != e2.
     */
    static DiseqExprPtr mkDiseq(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 < e2.
     */
    static LtExprPtr mkLt(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 <= e2.
     */
    static LeExprPtr mkLe(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 > e2.
     */
    static GtExprPtr mkGt(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 >= e2.
     */
    static GeExprPtr mkGe(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 and e2.
     */
    static AndExprPtr mkAnd(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a es_0 and es_1 ... and es_n-1.
     */
    static AndExprPtr mkAnd(std::vector<ExprPtr> es);
    /**
     * Return an expression representing \a e1 or e2.
     */
    static OrExprPtr mkOr(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a es_0 or es_1 ... or es_n-1.
     */
    static OrExprPtr mkOr(std::vector<ExprPtr> es);
    /**
     * Return an expression representing \a e1 xor e2.
     */
    static XorExprPtr mkXor(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a es_0 xor es_1 ... xor es_n-1.
     */
    static XorExprPtr mkXor(std::vector<ExprPtr> es);
    /**
     * Return an expression representing (IF econd ethen eelse).
     */
    static IteExprPtr mkIte(ExprPtr econd, ExprPtr ethen, ExprPtr eelse);
    /**
     * Return an expression representing \a e1 + e2.
     */
    static SumExprPtr mkSum(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 + e2 (signed).
     *
     * Signed addition with overlow. 
     * Bound: -2^(\a nbBit-1) <= x <= 2^(\a nbBit-1).
     */
    static ExprPtr mkSSum(ExprPtr e1, ExprPtr e2, unsigned nbBit);
    /**
     * Return an expression representing \a e1 + e2 (unsigned).
     *
     * Unsigned addition with overlow.
     * Bound: 0 <= u <= 2^(nbBit).
     */
    static ExprPtr mkUSum(ExprPtr e1, ExprPtr e2, unsigned nbBit);
    /**
     * Return an expression representing \a e1 - e2.
     */
    static SubExprPtr mkSub(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 * e2.
     */
    static MulExprPtr mkMul(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 / e2.
     */
    static DivExprPtr mkDiv(ExprPtr e1, ExprPtr e2);
    /**
     * Return an expression representing \a e1 % e2.
     */
    static ModExprPtr mkMod(ExprPtr e1, ExprPtr e2);
    /**
     * Return a function application term \a (e1 e2).
     */
    static AppExprPtr mkApp(ExprPtr e1, ExprPtr e2);
    /**
     * Build a function update term \a (update f (arg) v).
     */
    static UpdateExprPtr mkFunctionUpdate(ExprPtr f, ExprPtr arg, ExprPtr v);
    /**
     * Return an expression representing \a left op right.
     */
    static ExprPtr mkOp(ExprPtr left, ExprPtr right, llvm::CmpInst::Predicate op);
    /**
     * Return an expression representing \a left i.op right.
     */
    static ExprPtr mkOp(ExprPtr left, ExprPtr right, llvm::Instruction *i);
    
    /**
     * Return true if \a e1 is equal to \a e2, false otherwise.
     */
    friend bool operator== (ExprPtr e1, ExprPtr e2);
    /**
     * Return true if \a e1 is not equal to \a e2, false otherwise.
     */
    friend bool operator!= (ExprPtr e1, ExprPtr e2);
    
    /**
     * Return the opposite opperator of \a op.
     */
    static llvm::CmpInst::Predicate negateOp(llvm::CmpInst::Predicate op);
    /**
     * Return an expression representing the LLVM value \a v.
     */
    static ExprPtr getExprFromValue(llvm::Value *v);
    
    /**
     * Return the number of integer variables created.
     */
    static unsigned getNbIntVariables() {
        return NbIntVariables;
    }
    /**
     * Return the number of boolean variables created.
     */
    static unsigned getNbBoolVariables() {
        return NbBoolVariables;
    }
    /**
     * Return \a true if the expresseion is soft (retractable), 
     * false otherwise.
     */
    bool isSoft() {
        return soft;
    }
    /**
     * Return \a true if the expresseion is hard (not retractable), 
     * false otherwise.
     */
    bool isHard() {
        return !soft;
    }
    /**
     * Assign a line number (source code) to this expression.
     */
    void setLine(unsigned l) {
        line = l;
    }
    /**
     * Set this expression as hard (not retractable).
     */
    void setHard() {
        soft = false;
    }
    /**
     * Set this expression as soft (retractable).
     */
    void setSoft() {
        unsigned l = 0;
        llvm::Instruction *I = getInstruction();
        if (I) {
            if (llvm::MDNode *N = I->getMetadata("dbg")) {
                llvm::DILocation Loc(N);
                l = Loc.getLineNumber();
                setLine(l);
            }
        }
        setLine(l);
        soft = true;
    }
    /**
     * Return the line number in the original source code of the 
     * LLVM expression from which the expression was encoded.
     */
    unsigned getLine() {
        return line;
    }
    /**
     * Return the LLVM instruction from which the expression was encoded.
     * If no instruction was assigned to this expression,
     * the function returns null.
     */
    llvm::Instruction* getInstruction() {
        return instruction;
    }
    /**
     * Set the LLVM instruction from which the expression was encoded.
     */
    void setInstruction(llvm::Instruction *i) {
        instruction = i;
    }
    /**
     * Return the LLVM basicblock of the LLVM instruction from which 
     * the expression was encoded.
     * If no instruction was assigned to this expression, 
     * the function returns null.
     */
    llvm::BasicBlock* getBB() {
        if (instruction==NULL) {
            //std::cout << "warning: no instruction for <";
            //this->dump();
            //std::cout << ">" << std::endl;
            return NULL;
        }
        return instruction->getParent();
    }
    /**
     * Return the unique ID number of this expression.
     */
    unsigned getID() {
        return currentID;   
    }
    /**
     * Return the op code of this expression (see above).
     */
    virtual unsigned getOpCode() = 0;
    /**
     * Dump to the standard ouput a textual representation 
     * of this expression.
     */
    virtual void dump() = 0;
    
};

/**
 * \class SingleExpression
 *
 * A class representing a leaf expression such as a variable.
 */
class SingleExpression : public Expression {
protected:
    std::string name;
    SingleExpression(std::string _name) 
    : name(_name) { }
public:
    /**
     * Return the name of the variable.
     */
    std::string getName() {
        return name;
    }
};

/**
 * \class BinaryExpression
 * 
 * A class representing an expression with two arguments.
 */
class BinaryExpression : public Expression {
protected:
    ExprPtr e1;
    ExprPtr e2;
    BinaryExpression(ExprPtr _e1, ExprPtr _e2) 
    : e1(_e1),e2(_e2) { }
public:
    /**
     * Return the left-hand side expression.
     */
    ExprPtr getExpr1() {
        return e1;
    }
    /**
     * Return the right-hand side expression.
     */
    ExprPtr getExpr2() {
        return e2;
    }
};

/**
 * \class TrinaryExpression
 *
 * A class representing an expression with three arguments.
 */
class TrinaryExpression : public Expression {
protected:
    ExprPtr e1;
    ExprPtr e2;
    ExprPtr e3;
    TrinaryExpression(ExprPtr _e1, ExprPtr _e2, ExprPtr _e3 )
    : e1(_e1),e2(_e2),e3(_e3) { }
public:
    /**
     * Return the first expression.
     */
    ExprPtr getExpr1() {
        return e1;
    }
    /**
     * Return the second expression.
     */
    ExprPtr getExpr2() {
        return e2;
    }
    /**
     * Return the third expression.
     */
    ExprPtr getExpr3() {
        return e3;
    }    
};

/**
 * \class UnaryExpression
 *
 * A class representing an expression with \a n arguments.
 */
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
    /**
     * Return all the expressions.
     */
    std::vector<ExprPtr> getExprs() {
        return exprs;
    }
};

/**
 * \class TrueExpression
 *
 * An expression representing \a true.
 */
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

/**
 * \class FalseExpression
 *
 * An expression representing \a false.
 */
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

/**
 * \class UInt32NumExpression
 *
 * An expression representing an unsigned integer value.
 */
class UInt32NumExpression : public Expression {
protected:
    unsigned num;
public:
    UInt32NumExpression(unsigned n) : num(n) { }
    /**
     * Return the value.
     */
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

/**
 * \class SInt32NumExpression
 *
 * An expression representing a signed integer value.
 */
class SInt32NumExpression : public Expression {
protected:
    int num;
public:
    SInt32NumExpression(int n) : num(n) { }
    /**
     * Return the value.
     */
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

/**
 * \class BoolVarExpression
 *
 * An expression representing a Boolean variable.
 */
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
    virtual ~BoolVarExpression() {
        NbBoolVariables--;
    }
};

/**
 * \class IntVarExpression
 *
 * An expression representing an integer variable.
 */
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
    virtual ~IntVarExpression() {
        NbIntVariables--;
    }
};

/**
 * \class IntToIntVarExpression
 *
 * An expression representing a function (int->int) variable.
 */
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

/**
 * \class ToParseExpression
 * 
 * An expression representing a parsed expression.
 */
class ToParseExpression : public Expression {
private:
    std::string str;
public:
    ToParseExpression(std::string s) : str(s) { }
    /**
     * Return the parsed string.
     */
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

/**
 * \class GeExpression
 * 
 * An expression representing \a e1 >= e2.
 */
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

/**
 * \class GtExpression
 * 
 * An expression representing \a e1 > e2.
 */
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

/**
 * \class LeExpression
 *
 * An expression representing \a e1 <= e2.
 */
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

/**
 * \class LtExpression 
 * 
 * An expression representing \a e1 < e2.
 */
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

/**
 * \class DiseqExpression
 *
 * An expression representing \a e1 != e2.
 */
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

/**
 * \class EqExpression
 *
 * An expression representing \a e1 = e2.
 */
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

/**
 * \class NotExpression
 *
 * An expression representing \a (not e).
 */
class NotExpression : public Expression {
protected:
    ExprPtr e;
public:
    NotExpression(ExprPtr _e) : e(_e) { }
    /**
     * Return the expression not negated.
     */
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

/**
 * \class AndExpression
 *
 * An expression representing \a e1 and e2.
 */
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

/**
 * \class OrExpression
 * 
 * An expression representing \a e1 or e2.
 */
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

/**
 * \class XorExpression
 *
 * An expression representing \a e1 xor e2.
 */
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

/**
 * \class SumExpression
 *
 * An expression representing \a e1 + e2.
 */
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

/**
 * \class SubExpression
 *
 * An expression representing \a e1 - e2.
 */
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

/**
 * \class MulExpression
 *
 * An expression representing \a e1 * e2.
 */
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

/**
 * \class DivExpression
 *
 * An expression representing \a e1 / e2.
 */
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

/**
 * \class ModExpression
 *
 * An expression representing \a e1 % e2.
 */
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

/**
 * \class IteExpression
 *
 * An expression representing \a (IF cond then eelse).
 */
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

/**
 * \class AppExpression
 *
 * An expression representing the function 
 * application term \a (e1 e2).
 */
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

/**
 * \class UpdateExpression
 *
 * An expression representing a function 
 * update term \a (update f (arg) v).
 */
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

#endif // _EXPRESSION_H
