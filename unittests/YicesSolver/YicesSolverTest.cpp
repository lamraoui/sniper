/**
 * \file YicesSolverTest.cpp
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
 * \date   30 March 2015
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "yices_c.h"

#include "Logic/YicesSolver.h"
#include "Logic/Expression.h"
#include "gtest/gtest.h"

std::string getYicesExprAsString(yices_expr e) {
    if (e==NULL) {
        return "";
    }
    // Dump the expression into a string
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    yices_pp_expr(e); // printed into buffer
    std::string e_str = buffer.str();
    std::cout.rdbuf(old);
    return e_str;
}

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
    delete solver;
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
    delete solver;
}

// Testing makeYicesExpression()
/*TEST(YicesSolverTest, YicesSolverMkExpr) {
    
    // Create a solver
    YicesSolver *solver = new YicesSolver();
    solver->init();
    
    // True
    ExprPtr e1     = Expression::mkTrue();
    yices_expr ye1 = solver->makeYicesExpression(e1);
    
}*/

GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
