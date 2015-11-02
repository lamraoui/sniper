/**
 * ExpressionTest.cpp
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/10/28
 * @copyright : Hosei University 2015
 */


#include <stdio.h>

#include "Logic/Expression.h"
#include "gtest/gtest.h"

TEST(ExpressionTest, BaseExpression) {
    BoolVarExprPtr e1 = Expression::mkBoolVar("name");
    EXPECT_EQ(e1->getName(), "name");
    e1->setHard();
    EXPECT_TRUE(e1->isHard());
    e1->setSoft();
    EXPECT_TRUE(e1->isSoft());
    e1->setLine(42);
    EXPECT_EQ(e1->getLine(), 42);
    llvm::LLVMContext &C(llvm::getGlobalContext());
    llvm::BasicBlock *bb0 = llvm::BasicBlock::Create(C);
    llvm::ReturnInst *br0 = llvm::ReturnInst::Create(C, bb0);
    e1->setInstruction(br0);
    EXPECT_EQ(e1->getInstruction(), br0);
    EXPECT_EQ(e1->getBB(), bb0);
    e1->setInstruction(NULL);
    llvm::BasicBlock *bbNull = NULL;
    EXPECT_EQ(e1->getBB(), bbNull);
}

TEST(ExpressionTest, TrueExpression) {
    TrueExpression *e = new TrueExpression();
    const int code = Expression::True;
    EXPECT_EQ(e->getOpCode(), code);
    delete e;
}

TEST(ExpressionTest, FalseExpression) {
    FalseExpression *e = new FalseExpression();
    const int code = Expression::False;
    EXPECT_EQ(e->getOpCode(), code);
    delete e;
}

TEST(ExpressionTest, UInt32NumExpression) {
    UInt32NumExpression *e = new UInt32NumExpression(42);
    EXPECT_EQ(e->getValue(), 42);
    const int code = Expression::UInt32Num;
    EXPECT_EQ(e->getOpCode(), code);
    delete e;
}

TEST(ExpressionTest, SInt32NumExpression) {
    SInt32NumExpression *e = new SInt32NumExpression(-42);
    EXPECT_EQ(e->getValue(), -42);
    const int code = Expression::SInt32Num;
    EXPECT_EQ(e->getOpCode(), code);
    delete e;
}

TEST(ExpressionTest, BoolVarExpression) {
    BoolVarExpression *e1 = new BoolVarExpression("a");
    EXPECT_EQ(e1->getName(), "a");
    const int code = Expression::BoolVar;
    EXPECT_EQ(e1->getOpCode(), code);
    EXPECT_EQ(Expression::getNbBoolVariables(), 1);
    BoolVarExpression *e2 = new BoolVarExpression("b");
    EXPECT_EQ(Expression::getNbBoolVariables(), 2);
    delete e1;
    delete e2;
    EXPECT_EQ(Expression::getNbBoolVariables(), 0);
}

TEST(ExpressionTest, IntVarExpression) {
    IntVarExpression *e1 = new IntVarExpression("x");
    EXPECT_EQ(e1->getName(), "x");
    const int code = Expression::IntVar;
    EXPECT_EQ(e1->getOpCode(), code);
    EXPECT_EQ(Expression::getNbIntVariables(), 1);
    IntVarExpression *e2 = new IntVarExpression("y");
    EXPECT_EQ(Expression::getNbIntVariables(), 2);
    delete e1;
    delete e2;
    EXPECT_EQ(Expression::getNbIntVariables(), 0);
}

TEST(ExpressionTest, IntToIntVarExpression) {
    IntToIntVarExpression *e = new IntToIntVarExpression("array");
    const int code = Expression::IntToIntVar;
    EXPECT_EQ(e->getOpCode(), code);
    delete e;
}

TEST(ExpressionTest, ToParseExpression) {
    ToParseExpression *e = new ToParseExpression("(a or b)");
    const int code = Expression::ToParse;
    EXPECT_EQ(e->getOpCode(), code);
    EXPECT_EQ(e->getString(), "(a or b)");
    delete e;
}

TEST(ExpressionTest, GeExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    GeExpression *e3 = new GeExpression(e1, e2);
    const int code = Expression::Ge;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
    delete e3;
}

TEST(ExpressionTest, GtExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    GtExpression *e3 = new GtExpression(e1, e2);
    const int code = Expression::Gt;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
    delete e3;
}

TEST(ExpressionTest, LeExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    LeExpression *e3 = new LeExpression(e1, e2);
    const int code = Expression::Le;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
    delete e3;
}

TEST(ExpressionTest, LtExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    LtExpression *e3 = new LtExpression(e1, e2);
    const int code = Expression::Lt;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
    delete e3;
}

TEST(ExpressionTest, DiseqExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    DiseqExpression *e3 = new DiseqExpression(e1, e2);
    const int code = Expression::Diseq;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
    delete e3;
}

TEST(ExpressionTest, EqExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    EqExpression *e3 = new EqExpression(e1, e2);
    const int code = Expression::Eq;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
    delete e3;
}

TEST(ExpressionTest, NotExpression) {
    ExprPtr e1 = Expression::mkBoolVar("a");
    NotExpression *e2 = new NotExpression(e1);
    const int code = Expression::Not;
    EXPECT_EQ(e2->getOpCode(), code);
    EXPECT_EQ(e2->get(), e1);
}

TEST(ExpressionTest, AndExpression) {
    // Binary
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    AndExpression *e3 = new AndExpression(e1, e2);
    const int code = Expression::And;
    EXPECT_EQ(e3->getOpCode(), code);
    std::vector<ExprPtr> v1 = e3->getExprs();
    ASSERT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], e1);
    EXPECT_EQ(v1[1], e2);
    // Unary
    ExprPtr e4 = Expression::mkBoolVar("c");
    ExprPtr e5 = Expression::mkBoolVar("d");
    ExprPtr e6 = Expression::mkBoolVar("e");
    std::vector<ExprPtr> v2;
    v2.push_back(e4);
    v2.push_back(e5);
    v2.push_back(e6);
    AndExpression *e7 = new AndExpression(v2);
    std::vector<ExprPtr> v3 = e7->getExprs();
    ASSERT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], e4);
    EXPECT_EQ(v3[1], e5);
    EXPECT_EQ(v3[2], e6);
}

TEST(ExpressionTest, OrExpression) {
    // Binary
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    OrExpression *e3 = new OrExpression(e1, e2);
    const int code = Expression::Or;
    EXPECT_EQ(e3->getOpCode(), code);
    std::vector<ExprPtr> v1 = e3->getExprs();
    ASSERT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], e1);
    EXPECT_EQ(v1[1], e2);
    // Unary
    ExprPtr e4 = Expression::mkBoolVar("c");
    ExprPtr e5 = Expression::mkBoolVar("d");
    ExprPtr e6 = Expression::mkBoolVar("e");
    std::vector<ExprPtr> v2;
    v2.push_back(e4);
    v2.push_back(e5);
    v2.push_back(e6);
    OrExpression *e7 = new OrExpression(v2);
    std::vector<ExprPtr> v3 = e7->getExprs();
    ASSERT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], e4);
    EXPECT_EQ(v3[1], e5);
    EXPECT_EQ(v3[2], e6);
}

TEST(ExpressionTest, XorExpression) {
    // Binary
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    XorExpression *e3 = new XorExpression(e1, e2);
    const int code = Expression::Xor;
    EXPECT_EQ(e3->getOpCode(), code);
    std::vector<ExprPtr> v1 = e3->getExprs();
    ASSERT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], e1);
    EXPECT_EQ(v1[1], e2);
    // Unary
    ExprPtr e4 = Expression::mkBoolVar("c");
    ExprPtr e5 = Expression::mkBoolVar("d");
    ExprPtr e6 = Expression::mkBoolVar("e");
    std::vector<ExprPtr> v2;
    v2.push_back(e4);
    v2.push_back(e5);
    v2.push_back(e6);
    XorExpression *e7 = new XorExpression(v2);
    std::vector<ExprPtr> v3 = e7->getExprs();
    ASSERT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], e4);
    EXPECT_EQ(v3[1], e5);
    EXPECT_EQ(v3[2], e6);
}

TEST(ExpressionTest, SumExpression) {
    // Binary
    ExprPtr e1 = Expression::mkIntVar("v");
    ExprPtr e2 = Expression::mkIntVar("w");
    SumExpression *e3 = new SumExpression(e1, e2);
    const int code = Expression::Sum;
    EXPECT_EQ(e3->getOpCode(), code);
    std::vector<ExprPtr> v1 = e3->getExprs();
    ASSERT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], e1);
    EXPECT_EQ(v1[1], e2);
    // Unary
    ExprPtr e4 = Expression::mkIntVar("x");
    ExprPtr e5 = Expression::mkIntVar("y");
    ExprPtr e6 = Expression::mkIntVar("z");
    std::vector<ExprPtr> v2;
    v2.push_back(e4);
    v2.push_back(e5);
    v2.push_back(e6);
    SumExpression *e7 = new SumExpression(v2);
    std::vector<ExprPtr> v3 = e7->getExprs();
    ASSERT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], e4);
    EXPECT_EQ(v3[1], e5);
    EXPECT_EQ(v3[2], e6);
}

TEST(ExpressionTest, SubExpression) {
    // Binary
    ExprPtr e1 = Expression::mkIntVar("v");
    ExprPtr e2 = Expression::mkIntVar("w");
    SubExpression *e3 = new SubExpression(e1, e2);
    const int code = Expression::Sub;
    EXPECT_EQ(e3->getOpCode(), code);
    std::vector<ExprPtr> v1 = e3->getExprs();
    ASSERT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], e1);
    EXPECT_EQ(v1[1], e2);
    // Unary
    ExprPtr e4 = Expression::mkIntVar("x");
    ExprPtr e5 = Expression::mkIntVar("y");
    ExprPtr e6 = Expression::mkIntVar("z");
    std::vector<ExprPtr> v2;
    v2.push_back(e4);
    v2.push_back(e5);
    v2.push_back(e6);
    SubExpression *e7 = new SubExpression(v2);
    std::vector<ExprPtr> v3 = e7->getExprs();
    ASSERT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], e4);
    EXPECT_EQ(v3[1], e5);
    EXPECT_EQ(v3[2], e6);
}

TEST(ExpressionTest, MulExpression) {
    // Binary
    ExprPtr e1 = Expression::mkIntVar("v");
    ExprPtr e2 = Expression::mkIntVar("w");
    MulExpression *e3 = new MulExpression(e1, e2);
    const int code = Expression::Mul;
    EXPECT_EQ(e3->getOpCode(), code);
    std::vector<ExprPtr> v1 = e3->getExprs();
    ASSERT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], e1);
    EXPECT_EQ(v1[1], e2);
    // Unary
    ExprPtr e4 = Expression::mkIntVar("x");
    ExprPtr e5 = Expression::mkIntVar("y");
    ExprPtr e6 = Expression::mkIntVar("z");
    std::vector<ExprPtr> v2;
    v2.push_back(e4);
    v2.push_back(e5);
    v2.push_back(e6);
    MulExpression *e7 = new MulExpression(v2);
    std::vector<ExprPtr> v3 = e7->getExprs();
    ASSERT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], e4);
    EXPECT_EQ(v3[1], e5);
    EXPECT_EQ(v3[2], e6);
}

TEST(ExpressionTest, DivExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    DivExpression *e3 = new DivExpression(e1, e2);
    const int code = Expression::Div;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
}

TEST(ExpressionTest, ModExpression) {
    ExprPtr e1 = Expression::mkIntVar("x");
    ExprPtr e2 = Expression::mkIntVar("y");
    ModExpression *e3 = new ModExpression(e1, e2);
    const int code = Expression::Mod;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), e1);
    EXPECT_EQ(e3->getExpr2(), e2);
}

TEST(ExpressionTest, IteExpression) {
    ExprPtr cond  = Expression::mkBoolVar("a");
    ExprPtr then  = Expression::mkIntVar("x");
    ExprPtr eelse = Expression::mkIntVar("y");
    IteExpression *e4 = new IteExpression(cond, then, eelse);
    const int code = Expression::Ite;
    EXPECT_EQ(e4->getOpCode(), code);
    EXPECT_EQ(e4->getExpr1(), cond);
    EXPECT_EQ(e4->getExpr2(), then);
    EXPECT_EQ(e4->getExpr3(), eelse);
}

TEST(ExpressionTest, AppExpression) {
    ExprPtr f   = Expression::mkIntToIntVar("fun");
    ExprPtr arg = Expression::mkIntVar("x");
    AppExpression *e3 = new AppExpression(f, arg);
    const int code = Expression::App;
    EXPECT_EQ(e3->getOpCode(), code);
    EXPECT_EQ(e3->getExpr1(), f);
    EXPECT_EQ(e3->getExpr2(), arg);
}

TEST(ExpressionTest, UpdateExpression) {
    ExprPtr f    = Expression::mkBoolVar("fun");
    ExprPtr arg  = Expression::mkIntVar("x");
    ExprPtr v    = Expression::mkUInt32Num(42);
    UpdateExpression *e4 = new UpdateExpression(f, arg, v);
    const int code = Expression::Update;
    EXPECT_EQ(e4->getOpCode(), code);
    EXPECT_EQ(e4->getExpr1(), f);
    EXPECT_EQ(e4->getExpr2(), arg);
    EXPECT_EQ(e4->getExpr3(), v);
}

TEST(ExpressionTest, PointerEquality) {
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = e1;
  EXPECT_EQ(e1, e2);
}

TEST(ExpressionTest, Equality) {
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkNot(e1);
    ExprPtr e3 = Expression::mkBoolVar("b");
    ExprPtr e4 = Expression::mkAnd(e2, e3);
    ExprPtr e5 = Expression::mkBoolVar("a");
    ExprPtr e6 = Expression::mkNot(e5);
    ExprPtr e7 = Expression::mkBoolVar("b");
    ExprPtr e8 = Expression::mkAnd(e6, e7);
    EXPECT_EQ(e4, e8);
}

TEST(ExpressionTest, Disequality) {
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkNot(e1);
    ExprPtr e3 = Expression::mkBoolVar("b");
    ExprPtr e4 = Expression::mkAnd(e2, e3);
    ExprPtr e5 = Expression::mkBoolVar("a");
    ExprPtr e6 = Expression::mkBoolVar("b");
    ExprPtr e7 = Expression::mkAnd(e5, e6);
    EXPECT_NE(e4, e7);
}

GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
