/**
 * \file CombineTest.cpp
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

#include "Logic/Combine.h"
#include "gtest/gtest.h"


TEST(CombineTest, CombineTestPWU) {
 
    // Empty set of formulas
    std::vector<SetOfFormulasPtr> D1;
    SetOfFormulasPtr C1 = Combine::combineByPWU(D1);
    EXPECT_TRUE(C1->empty());
    
    // <{{a,b,c}}> -> {{a,b,c}}
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    ExprPtr e3 = Expression::mkBoolVar("c");
    FormulaPtr f1 = Formula::make();
    f1->add(e1);
    f1->add(e2);
    f1->add(e3);
    SetOfFormulasPtr sf1 = SetOfFormulas::make();
    sf1->add(f1);
    D1.push_back(sf1);
    SetOfFormulasPtr C2 = Combine::combineByPWU(D1);
    EXPECT_TRUE(!C2->empty());
    EXPECT_EQ(C2->size(), 1);
    EXPECT_EQ(C2->getAt(0)->size(), 3);

    // <{{a}}, {{b}}> -> {{a,b}}
    std::vector<SetOfFormulasPtr> D2;
    FormulaPtr f2 = Formula::make();
    f2->add(e1);
    SetOfFormulasPtr sf2 = SetOfFormulas::make();
    sf2->add(f2);
    D2.push_back(sf2);
    FormulaPtr f3 = Formula::make();
    f3->add(e2);
    SetOfFormulasPtr sf3 = SetOfFormulas::make();
    sf3->add(f3);
    D2.push_back(sf3);
    SetOfFormulasPtr C3 = Combine::combineByPWU(D2);
    EXPECT_TRUE(!C3->empty());
    EXPECT_EQ(C3->size(), 1);
    EXPECT_EQ(C3->getAt(0)->size(), 2);
    ExprPtr boolvar1 = C3->getAt(0)->getExprs()[0];
    ExprPtr boolvar2 = C3->getAt(0)->getExprs()[1];
    ExprPtr o1 = Expression::mkBoolVar("a");
    ExprPtr o2 = Expression::mkBoolVar("b");
    const bool tmp1 = (boolvar1==o1 || boolvar1==o2);
    const bool tmp2 = (boolvar2==o2 || boolvar2==o1);
    EXPECT_TRUE(tmp1);
    EXPECT_TRUE(tmp2);

}

TEST(CombineTest, CombineTestMHS) {
    
    // Empty set of formulas
    std::vector<SetOfFormulasPtr> D1;
    SetOfFormulasPtr C1 = Combine::combineByMHS(D1);
    EXPECT_TRUE(C1->empty());
    
    // <{{a,b,c}}> -> {{a,b,c}}
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    ExprPtr e3 = Expression::mkBoolVar("c");
    FormulaPtr f1 = Formula::make();
    f1->add(e1);
    f1->add(e2);
    f1->add(e3);
    SetOfFormulasPtr sf1 = SetOfFormulas::make();
    sf1->add(f1);
    D1.push_back(sf1);
    SetOfFormulasPtr C2 = Combine::combineByMHS(D1);
    EXPECT_TRUE(!C2->empty());
    EXPECT_EQ(C2->size(), 1);
    EXPECT_EQ(C2->getAt(0)->size(), 3);
    
    // <{{a}}, {{b}}> -> {{a,b}}
    std::vector<SetOfFormulasPtr> D2;
    FormulaPtr f2 = Formula::make();
    f2->add(e1);
    SetOfFormulasPtr sf2 = SetOfFormulas::make();
    sf2->add(f2);
    D2.push_back(sf2);
    FormulaPtr f3 = Formula::make();
    f3->add(e2);
    SetOfFormulasPtr sf3 = SetOfFormulas::make();
    sf3->add(f3);
    D2.push_back(sf3);
    SetOfFormulasPtr C3 = Combine::combineByMHS(D2);
    EXPECT_TRUE(!C3->empty());
    EXPECT_EQ(C3->size(), 1);
    EXPECT_EQ(C3->getAt(0)->size(), 2);
    ExprPtr boolvar1 = C3->getAt(0)->getExprs()[0];
    ExprPtr boolvar2 = C3->getAt(0)->getExprs()[1];
    ExprPtr o1 = Expression::mkBoolVar("a");
    ExprPtr o2 = Expression::mkBoolVar("b");
    const bool tmp1 = (boolvar1==o1 || boolvar1==o2);
    const bool tmp2 = (boolvar2==o2 || boolvar2==o1);
    EXPECT_TRUE(tmp1);
    EXPECT_TRUE(tmp2);

}

TEST(CombineTest, CombineTestFlatten) {
    
    // Empty set of formulas
    std::vector<SetOfFormulasPtr> D1;
    SetOfFormulasPtr C1 = Combine::combineByFlatten(D1);
    EXPECT_TRUE(C1->empty());
 
    // <{{a,b,c}}> -> {{a,b,c}}
    ExprPtr e1 = Expression::mkBoolVar("a");
    ExprPtr e2 = Expression::mkBoolVar("b");
    ExprPtr e3 = Expression::mkBoolVar("c");
    FormulaPtr f1 = Formula::make();
    f1->add(e1);
    f1->add(e2);
    f1->add(e3);
    SetOfFormulasPtr sf1 = SetOfFormulas::make();
    sf1->add(f1);
    D1.push_back(sf1);
    SetOfFormulasPtr C2 = Combine::combineByFlatten(D1);
    EXPECT_TRUE(!C2->empty());
    EXPECT_EQ(C2->size(), 1);
    EXPECT_EQ(C2->getAt(0)->size(), 3);
    
    // <{{a}}, {{b}}> -> {{a,b}}
    std::vector<SetOfFormulasPtr> D2;
    FormulaPtr f2 = Formula::make();
    f2->add(e1);
    SetOfFormulasPtr sf2 = SetOfFormulas::make();
    sf2->add(f2);
    D2.push_back(sf2);
    FormulaPtr f3 = Formula::make();
    f3->add(e2);
    SetOfFormulasPtr sf3 = SetOfFormulas::make();
    sf3->add(f3);
    D2.push_back(sf3);
    SetOfFormulasPtr C3 = Combine::combineByFlatten(D2);
    EXPECT_TRUE(!C3->empty());
    EXPECT_EQ(C3->size(), 2);
    EXPECT_EQ(C3->getAt(0)->size(), 1);
    EXPECT_EQ(C3->getAt(1)->size(), 1);
    ExprPtr boolvar1 = C3->getAt(0)->getExprs()[0];
    ExprPtr boolvar2 = C3->getAt(1)->getExprs()[0];
    ExprPtr o1 = Expression::mkBoolVar("a");
    ExprPtr o2 = Expression::mkBoolVar("b");
    const bool tmp1 = (boolvar1==o1 || boolvar1==o2);
    const bool tmp2 = (boolvar2==o2 || boolvar2==o1);
    EXPECT_TRUE(tmp1);
    EXPECT_TRUE(tmp2);

}

GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
