/**
 * YicesSolverTest.cpp
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/11/02
 * @copyright : Hosei University 2015
 */


#include <stdio.h>

#include "Logic/YicesSolver.h"
#include "gtest/gtest.h"

TEST(YicesSolverTest, YicesSolverCheck) {
    
    // Create a solver
    YicesSolver *solver = new YicesSolver();
    solver->init();
    
    // Check (a and b) -> SAT
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    solver->addToContext(e1);
    solver->addToContext(e2);
    const int res1 = solver->check();
    EXPECT_EQ(res1, l_true);
    
    // Get values
    const int val1 = solver->getValue("a");
    bool error1;
    const int val2 = solver->getValue("a", error1);
    // getBoolValue
    // getValueOrZero

    // Check (a and b and (not a)) -> UNSAT
    ExprPtr e3 = Expression::mkNot(e1);
    solver->addToContext(e3);
    const int res2 = solver->check();
    EXPECT_EQ(res2, l_false);
    
    solver->clean();
}

TEST(YicesSolverTest, YicesSolverMaxsat) {
    
    // Create a solver
    YicesSolver *solver = new YicesSolver();
    solver->init();
    
    // MaxSAT (a and b) -> SAT
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    e1->setSoft();
    e2->setSoft();
    solver->addToContext(e1);
    solver->addToContext(e2);
    const int res1 = solver->maxSat();
    EXPECT_EQ(res1, l_true);
    
    // MaxSAT (a and b and (not a)) -> SAT with a cost of 1
    ExprPtr e3 = Expression::mkNot(e1);
    e3->setSoft();
    solver->addToContext(e3);
    const int res2 = solver->maxSat();
    EXPECT_EQ(res2, l_true);
    const double cost1 = solver->getCostAsDouble();
    EXPECT_EQ(cost1, 1.0);
    solver->clean();
    
    // MaxSAT (a and b and (not a)) with only b as soft -> UNSAT
    solver->init();
    e1->setHard();
    e2->setSoft();
    e3->setHard();
    solver->addToContext(e1);
    solver->addToContext(e2);
    solver->addToContext(e3);
    const int res3 = solver->maxSat();
    EXPECT_EQ(res3, l_false);
    solver->clean();
    
    
}


GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
