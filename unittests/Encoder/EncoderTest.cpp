/**
 * EncoderTest.cpp
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/12/18
 * @copyright : Hosei University 2015
 */


#include <stdio.h>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

/*#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"*/

#include "llvm/ADT/OwningPtr.h"

#include "Options.h"
#include "Utils/Utils.h"
#include "Logic/Expression.h"
#include "Encoder/Context.h"
#include "Encoder/Encoder.h"

#include "gtest/gtest.h"

using namespace llvm;

class EncoderTest : public testing::Test {
protected:
    // Code here will execute just before the test ensues
    virtual void SetUp() {
        // Create a module containing one function
        LLVMContext &Context = getGlobalContext();
        Mod.reset(new Module("test", Context));
        IntegerType *int32Ty = Type::getInt32Ty(Context);
        Constant *c = Mod->getOrInsertFunction("foo", int32Ty,
                                               int32Ty, int32Ty, NULL);
        Fun = cast<Function>(c);
        Fun->setCallingConv(CallingConv::C);
        Function::arg_iterator args = Fun->arg_begin();
        Value* x = args++;
        x->setName("x");
        Value* y = args++;
        y->setName("y");
        BB = BasicBlock::Create(Context, "entry", Fun);
    }
    // code here will be called just after the test completes
    virtual void TearDown() {
        BB = 0;
        Mod.reset();
    }
     // Custom data members
    OwningPtr<Module> Mod;
    Function *Fun;
    BasicBlock *BB;
};

TEST_F(EncoderTest, EncodeBinaryOperator) {
    // Add two instructions to Fun
    IRBuilder<> builder(BB);
    Function::arg_iterator args = Fun->arg_begin();
    Value* x = args++;
    Value* y = args++;
    Value *tmp1 = builder.CreateBinOp(Instruction::Add, x, y, "tmp1");
    BinaryOperator *inst = dyn_cast<BinaryOperator>(tmp1);
    builder.CreateRet(inst);
    verifyModule(*Mod, PrintMessageAction);
    
    // Initialize the SNIPER encoder
    LocalVariables *LV = new LocalVariables();
    LV->processLoadStore(Fun);
    ::Context *ctx = new ::Context(LV);
    Encoder *encoder = new Encoder(ctx);
    EXPECT_TRUE(encoder!=NULL);
    
    // Encode the ADD instruction into a constraint
    // (= tmp1 (+ x y))
    ExprPtr e = encoder->encode(inst);
    EXPECT_TRUE(e!=NULL);
    EXPECT_TRUE(e->isSoft());
    EXPECT_EQ(e->getInstruction(), inst);
    EXPECT_EQ(e->getBB(), BB);
    const bool isEq = (e->getOpCode()==Expression::Eq);
    EXPECT_TRUE(isEq);
    EqExprPtr ee = std::static_pointer_cast<EqExpression>(e);
    EXPECT_TRUE(ee!=NULL);
    ExprPtr e1 = ee->getExpr1();
    ExprPtr e2 = ee->getExpr2();
    const bool isE1IntVar = (e1->getOpCode()==Expression::IntVar);
    EXPECT_TRUE(isE1IntVar);
    const bool isE2Sum = (e2->getOpCode()==Expression::Sum);
    EXPECT_TRUE(isE2Sum);
    SumExprPtr se = std::static_pointer_cast<SumExpression>(e2);
    EXPECT_TRUE(se!=NULL);
    std::vector<ExprPtr> exprs = se->getExprs();
    EXPECT_EQ(exprs.size(), 2);
    const bool isSE1IntVar = (exprs[0]->getOpCode()==Expression::IntVar);
    const bool isSE2IntVar = (exprs[1]->getOpCode()==Expression::IntVar);
    EXPECT_TRUE(isSE1IntVar && isSE2IntVar);
}

TEST_F(EncoderTest, EncodeSelectInst) {
    // Add two instructions to Fun
    IRBuilder<> builder(BB);
    Function::arg_iterator args = Fun->arg_begin();
    Value* x = args++;
    Value* y = args++;
    Value *c1 = builder.CreateICmpSGE(x, y, "tmp1");
    Value *tmp1 = builder.CreateSelect(c1, x, y, "tmp2");
    SelectInst *inst = dyn_cast<SelectInst>(tmp1);
    builder.CreateRet(inst);
    verifyModule(*Mod, PrintMessageAction);
    
    // Initialize the SNIPER encoder
    LocalVariables *LV = new LocalVariables();
    LV->processLoadStore(Fun);
    ::Context *ctx = new ::Context(LV);
    Encoder *encoder = new Encoder(ctx);
    EXPECT_TRUE(encoder!=NULL);

    // Encode the SELECT instruction into a constraint
    // (= tmp1 (>= x y))
    // (= tmp2 (tmp1? x : y))
    ExprPtr e = encoder->encode(inst);
    EXPECT_TRUE(e!=NULL);
    EXPECT_TRUE(e->isSoft());
    EXPECT_EQ(e->getInstruction(), inst);
    EXPECT_EQ(e->getBB(), BB);
    const bool isEq = (e->getOpCode()==Expression::Eq);
    EXPECT_TRUE(isEq);
    EqExprPtr ee = std::static_pointer_cast<EqExpression>(e);
    EXPECT_TRUE(ee!=NULL);
    ExprPtr e1 = ee->getExpr1();
    ExprPtr e2 = ee->getExpr2();
    const bool isE1IntVar = (e1->getOpCode()==Expression::IntVar);
    const bool isE2Ite = (e2->getOpCode()==Expression::Ite);
    EXPECT_TRUE(isE1IntVar && isE2Ite);
    IteExprPtr se = std::static_pointer_cast<IteExpression>(e2);
    se->dump();
    EXPECT_TRUE(se!=NULL);
    ExprPtr se1 = se->getExpr1();
    ExprPtr se2 = se->getExpr2();
    ExprPtr se3 = se->getExpr3();
    const bool isBool    = (se1->getOpCode()==Expression::BoolVar);
    const bool isIntVar1 = (se2->getOpCode()==Expression::IntVar);
    const bool isIntVar2 = (se3->getOpCode()==Expression::IntVar);
    EXPECT_TRUE(isBool && isIntVar1 && isIntVar2);
}


TEST_F(EncoderTest, EncodePHINode) {
    
}

TEST_F(EncoderTest, EncodeBranchInst) {
    
}

TEST_F(EncoderTest, EncodeSwitchInst) {
    
}

TEST_F(EncoderTest, EncodeICmpInst) {
    
}

TEST_F(EncoderTest, EncodeAllocaInst) {
    
}

TEST_F(EncoderTest, EncodeStoreInst) {
    
}

TEST_F(EncoderTest, EncodeLoadInst) {
    
}

TEST_F(EncoderTest, EncodeGEPInst) {
    
}

TEST_F(EncoderTest, EncodeSExtInst) {
    
}

TEST_F(EncoderTest, EncodeZExtInst) {
    
}

TEST_F(EncoderTest, EncodeCallInst) {
    
}

TEST_F(EncoderTest, EncodeReturnInst) {
    
}

GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
