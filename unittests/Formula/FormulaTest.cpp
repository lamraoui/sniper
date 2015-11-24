/**
 * FormulaTest.cpp
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/11/02
 * @copyright : Hosei University 2015
 */


#include <stdio.h>

#include "Logic/Formula.h"
#include "gtest/gtest.h"

TEST(FormulaTest, FormulaBase) {
    
    // Create a basic block containing three instructions
    llvm::LLVMContext &C(llvm::getGlobalContext());
    llvm::BasicBlock *bb0     = llvm::BasicBlock::Create(C);
    llvm::Type       *int32Ty = llvm::Type::getInt32Ty(C);
    llvm::Value      *val0    = llvm::ConstantInt::get(int32Ty, 42);
    llvm::ZExtInst   *ze0     = new llvm::ZExtInst(val0, int32Ty, "", bb0);
    llvm::SExtInst   *se0     = new llvm::SExtInst(ze0,  int32Ty, "", bb0);
    llvm::ReturnInst *br0     = llvm::ReturnInst::Create(C, bb0);
    
    // Create some expressions and assigned them the previous instructions
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    ExprPtr e3 = Expression::mkBoolVar("c");
    
    // Create an empty formula
    Formula *f = new Formula();
    EXPECT_TRUE(f->empty());
    EXPECT_EQ(f->size(), 0);
    
    // Add a soft expression
    e1->setSoft();
    f->add(e1);
    EXPECT_TRUE(!f->empty());
    EXPECT_EQ(f->size(), 1);
    EXPECT_EQ(f->getNbSoftExpr(), 1);
    EXPECT_EQ(f->getNbHardExpr(), 0);
    
    // Remove the single expression
    f->remove(e1);
    EXPECT_TRUE(f->empty());
    EXPECT_EQ(f->getNbSoftExpr(), 0);
    EXPECT_EQ(f->getNbHardExpr(), 0);
    
    // Put back the previous expression, as hard this time
    e1->setHard();
    f->add(e1);
    EXPECT_EQ(f->size(), 1);
    EXPECT_EQ(f->getNbSoftExpr(), 0);
    EXPECT_EQ(f->getNbHardExpr(), 1);
    
    // Add a second expression, a soft one
    e2->setSoft();
    f->add(e2);
    EXPECT_EQ(f->size(), 2);
    EXPECT_EQ(f->getNbSoftExpr(), 1);
    EXPECT_EQ(f->getNbHardExpr(), 1);
    
    // Change the second expression to be hard
    e2->setHard();
    EXPECT_EQ(f->getNbSoftExpr(), 0);
    EXPECT_EQ(f->getNbHardExpr(), 2);
    
    // Remove an expression that is not in the formula
    f->remove(e3);
    EXPECT_EQ(f->size(), 2);
    
    // All expressions are hard
    e3->setHard();
    f->add(e3);
    size_t s1 = 0;
    for (ExprPtr e : f->getExprs()) {
        EXPECT_TRUE(e->isHard());
        s1++;
    }
    EXPECT_EQ(f->size(), s1);
    EXPECT_EQ(f->getNbHardExpr(), s1);
    EXPECT_EQ(f->getNbSoftExpr(), 0);
    
    // Get all line numbers
    std::vector<unsigned> lines = f->getLineNumbers();
    EXPECT_EQ(lines.size(), 3);
    for (unsigned l : lines) {
        EXPECT_EQ(l, 0);
    }
    
    // Set instructions to expressions
    e1->setInstruction(ze0);
    EXPECT_TRUE(e1->getInstruction());
    e2->setInstruction(se0);
    EXPECT_TRUE(e2->getInstruction());
    e3->setInstruction(br0);
    EXPECT_TRUE(e3->getInstruction());
    
    // TODO: add metadata to instructions and check line numbers
    //std::vector<unsigned> lines2 = f->getLineNumbers();
    
    // Get expressions for a BasicBlock
    std::vector<ExprPtr> ebb1 = f->getExprs(bb0);
    ASSERT_EQ(ebb1.size(), 3);
    const bool e1IsIn = (ebb1[0]==e1 || ebb1[1]==e1 || ebb1[2]==e1);
    const bool e2IsIn = (ebb1[0]==e2 || ebb1[1]==e2 || ebb1[2]==e2);
    const bool e3IsIn = (ebb1[0]==e3 || ebb1[1]==e3 || ebb1[2]==e3);
    EXPECT_TRUE((e1IsIn && e2IsIn && e3IsIn));
    
    // Get soft expressions for a BasicBlock
    std::vector<ExprPtr> ebb2 = f->getSoftExprs(bb0);
    EXPECT_TRUE(ebb2.empty());
    e2->setSoft();
    std::vector<ExprPtr> ebb3 = f->getSoftExprs(bb0);
    EXPECT_EQ(ebb3.size(), 1);
    
    // Remove all expressions
    std::vector<ExprPtr> eall = f->getExprs();
    size_t s2 = 0;
    while (s2 < eall.size()) {
        ASSERT_LT(s2, eall.size());
        ExprPtr x = *std::next(eall.begin(), s2); // eall.getAt(s2)
        f->remove(x);
        s2++;
    }
    EXPECT_TRUE(f->empty());
   
    // Clone an empty formula
    Formula *f2 = f->clone();
    std::vector<ExprPtr> E1 = f->getExprs();
    std::vector<ExprPtr> E2 = f2->getExprs();
    const bool equal1 = (E1==E2);
    EXPECT_TRUE(equal1);
    
    // Add an expression to f2
    f2->add(e1);
    E2 = f2->getExprs();
    const bool equal2 = (E1==E2);
    EXPECT_FALSE(equal2);
    
    // Add the same expression to f
    f->add(e1);
    E1 = f->getExprs();
    const bool equal3 = (E1==E2);
    EXPECT_TRUE(equal3);
    
    // Copy f to f3
    Formula f3 = *f;
    std::vector<ExprPtr> E3 = f3.getExprs();
    const bool equal4 = (E1==E3);
    EXPECT_TRUE(equal4);
    
    // Equal operator
    Formula *f4 = new Formula();
    Formula *f5 = new Formula();
    const bool equal5 = (*f4==*f5);
    EXPECT_TRUE(equal5);
    
    f5->add(e2);
    const bool equal6 = (*f4==*f5);
    EXPECT_FALSE(equal6);
    
    // Add duplicate expressions (same reference only)
    Formula *f6 = new Formula();
    f6->add(e1);
    f6->add(e2);
    f6->add(e3);
    EXPECT_EQ(f6->size(), 3);
    f6->add(e3); // duplicate
    f6->add(e3); // duplicate
    EXPECT_EQ(f6->size(), 3);
    f6->add(e1);
    EXPECT_EQ(f6->size(), 3);
    
    // Add duplicate expressions (same content)
    ExprPtr e4 = Expression::mkBoolVar("b"); // same as e2
    e4->setSoft();
    f6->add(e4);
    EXPECT_EQ(f6->size(), 3);
    
    // Add a set of expressions
    std::vector<ExprPtr> E4;
    E4.push_back(e1);
    E4.push_back(e2);
    E4.push_back(e3);
    E4.push_back(e4);
    f6->add(E4);
    EXPECT_EQ(f6->size(), 3);
    
    delete f;
    delete f2;
    delete f4;
    delete f5;
    delete f6;
    
    // TODO: Free parents first
    //delete ze0;
    //delete se0;
}


TEST(FormulaTest, SetOfFormulasBase) {
    
    // Create a basic block containing three instructions
    llvm::LLVMContext &C(llvm::getGlobalContext());
    llvm::BasicBlock *bb0     = llvm::BasicBlock::Create(C);
    llvm::Type       *int32Ty = llvm::Type::getInt32Ty(C);
    llvm::Value      *val0    = llvm::ConstantInt::get(int32Ty, 42);
    llvm::ZExtInst   *ze0     = new llvm::ZExtInst(val0, int32Ty, "", bb0);
    llvm::SExtInst   *se0     = new llvm::SExtInst(ze0,  int32Ty, "", bb0);
    llvm::ReturnInst *br0     = llvm::ReturnInst::Create(C, bb0);
    
    // Create some expressions and assigned them the previous instructions
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    ExprPtr e3 = Expression::mkBoolVar("c");
    
    // Create an empty set of formulas
    SetOfFormulas *sf = new SetOfFormulas();
    EXPECT_TRUE(sf->empty());
    EXPECT_EQ(sf->size(), 0);
    
    // Create and add some formulas
    const unsigned n = 10;
    std::vector<FormulaPtr> F1;
    for (int i=0;i<n;i++) {
        FormulaPtr f = Formula::make();
        std::stringstream sstm;
        sstm << "n" << i;
        std::string result = sstm.str();
        ExprPtr e = Expression::mkBoolVar(result);
        f->add(e);
        F1.push_back(f);
        sf->add(f);
    }
    EXPECT_TRUE(!sf->empty());
    EXPECT_EQ(sf->size(), n);
    
    // Get formulas of sf
    std::vector<FormulaPtr> F2 = sf->getFormulas();
    const bool equal1 = (F1 == F2);
    EXPECT_TRUE(equal1);
    
    // Get formual at a particulat position in sf
    ASSERT_EQ(sf->size(), n);
    ASSERT_EQ(F1.size(), n);
    for (int i=0;i<n;i++) {
        FormulaPtr f1 = sf->getAt(i);
        FormulaPtr f2 = F1[i];
        EXPECT_EQ(f1, f2);
    }
    
    // Add a vector of formula to sf
    const unsigned m = 42;
    std::vector<FormulaPtr> F3;
    for (int i=0;i<m;i++) {
        FormulaPtr f = Formula::make();
        std::stringstream sstm;
        sstm << "m" << i;
        std::string result = sstm.str();
        ExprPtr e = Expression::mkBoolVar(result);
        f->add(e);
        F3.push_back(f);
    }
    sf->add(F3);
    EXPECT_EQ(sf->size(), n+m);
    
    // Add a set of formula to sf
    SetOfFormulasPtr sf2 = SetOfFormulas::make();
    const unsigned k = 22;
    for (int i=0;i<k;i++) {
        FormulaPtr f = Formula::make();
        std::stringstream sstm;
        sstm << "k" << i;
        std::string result = sstm.str();
        ExprPtr e = Expression::mkBoolVar(result);
        f->add(e);
        sf2->add(f);
    }
    sf->add(sf2->getFormulas());
    EXPECT_EQ(sf->size(), n+m+k);
    
    // TODO: Get code size reduction of sf
    //double csr1 = sf->getCodeSizeReduction(42);
    //EXPECT_EQ(csr1, 0);
    
    // TODO: CSR non null
    //double csr2 = sf->getCodeSizeReduction(42);
    //EXPECT_EQ(csr1, 14.2);
    
    // Add a formula which is a subset of another one
    // -> f3 = {var2} is a subset of f2 = {var1, var2, var3}
    SetOfFormulasPtr sf3 = SetOfFormulas::make();
    FormulaPtr f2 = Formula::make();
    FormulaPtr f3 = Formula::make();
    ExprPtr var1 = Expression::mkBoolVar("var1");
    ExprPtr var2 = Expression::mkBoolVar("var2");
    ExprPtr var3 = Expression::mkBoolVar("var3");
    f2->add(var1);
    f2->add(var2);
    f2->add(var3);
    f3->add(var2);
    sf3->add(f2);
    sf3->add(f3);
    EXPECT_EQ(sf3->size(), 1);
    
    // Add a formula which is a overset of a formula already in the set
    // -> f4 = {var1, var2, var3, var4} is a overset of f2 = {var1, var2, var3}
    FormulaPtr f4 = Formula::make();
    ExprPtr var1b1 = Expression::mkBoolVar("var1");
    ExprPtr var2b1 = Expression::mkBoolVar("var2");
    ExprPtr var3b1 = Expression::mkBoolVar("var3");
    ExprPtr var4b1 = Expression::mkBoolVar("var4");
    f4->add(var1b1);
    f4->add(var2b1);
    f4->add(var3b1);
    f4->add(var4b1);
    sf3->add(f4);
    EXPECT_EQ(sf3->size(), 1);
    EXPECT_EQ(sf3->getFormulas()[0]->size(), 4);
    
    // Add a formula which is equal to one in the set
    FormulaPtr f5 = Formula::make();
    ExprPtr var1bis = Expression::mkBoolVar("var1");
    ExprPtr var2bis = Expression::mkBoolVar("var2");
    ExprPtr var3bis = Expression::mkBoolVar("var3");
    f5->add(var1bis);
    f5->add(var2bis);
    f5->add(var3bis);
    sf3->add(f5);
    EXPECT_EQ(sf3->size(), 1);
    
    // TODO: Free parents first
    //delete ze0;
    //delete se0;
}


GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
