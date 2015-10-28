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

//#include "Logic/Expression.h"
#include "gtest/gtest.h"

TEST(Expression, ExpressionEquality) {
  EXPECT_EQ("1", "1");
}


GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
