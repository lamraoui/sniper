/**
 * Encoder.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/04/11
 * @copyright : NII 2014
 */


#include "Encoder.h"


const StringRef Encoder::SNIPER_FUN_PREFIX      = "sniper_";
const StringRef Encoder::SNIPER_ASSERT_RETINT_FUN_NAME = "sniper_i_assert";
const StringRef Encoder::SNIPER_ASSERT_RETVOID_FUN_NAME = "sniper_v_assert";
const StringRef Encoder::SNIPER_ASSUME_RETINT_FUN_NAME = "sniper_i_assume";
const StringRef Encoder::SNIPER_ASSUME_RETVOID_FUN_NAME = "sniper_v_assume";
const StringRef Encoder::ASSERT_FUN_NAME        = "assert";
const StringRef Encoder::ASSUME_FUN_NAME        = "assume";
const StringRef Encoder::LOOP_ASSERT_FUN_NAME   = "assert_loop";


// =============================================================================
// encode - BinaryOperator
// 
// Arithmetic: Add, Sub, Mul, Rem 
// Boolean: And, Or, Xor, Shl, LShr, AShr
// =============================================================================
ExprPtr Encoder::encode(BinaryOperator *bo) {
    int opCode = bo->getOpcode();
    Value *r = dyn_cast<Value>(bo);
    Value *arg1 = bo->getOperand(0);
    Value *arg2 = bo->getOperand(1);
    // Non-linear arithmetic checking
    if((opCode==Instruction::Mul || opCode==Instruction::FMul ||
        opCode==Instruction::UDiv || opCode==Instruction::SDiv ||
        opCode==Instruction::FDiv) 
        && !isa<ConstantInt>(arg1) && !isa<ConstantInt>(arg2)) {

        // Declare the virtual variables
        ExprPtr rVar = ctx->newVariable(r);
        ExprPtr arg1Var = ctx->getLocalVariable(bo, arg1, 0);
        if(arg1Var==NULL)
            arg1Var = ctx->newVariable(arg1);
        ExprPtr arg2Var = ctx->getLocalVariable(bo, arg2, 1);
        if(arg2Var==NULL)
            arg2Var = ctx->newVariable(arg2);
        
        ExprPtr expr = NULL; 
        NLOperationSummary *NOS = NULL;//profile->getNLOpSummary(bo);
        if (NOS) {
            // (or (x1=y1*z1) (x2=y2*z2) (xn=yn*zn))
            
            // (or (and (x1=v1_1) (y1=v1_2) (z1=v1_3)) 
            //     (and (x2=v2_1) (y1=v2_2) (z1=v2_3))
            // ...)
            // with n the n'ieme summary
            std::vector<ExprPtr> orExprs;
            std::vector<std::pair<Value*,Value*> > values = NOS->getValues();
            std::vector<std::pair<Value*,Value*> >::const_iterator it;
            for (it=values.begin(); it!=values.end(); ++it) {
                std::pair<Value*,Value*> v = *it;
                ConstantInt *CI1 = dyn_cast<ConstantInt>(v.first);
                int x = CI1->getSExtValue();
                ConstantInt *CI2 = dyn_cast<ConstantInt>(v.second);
                int y = CI2->getSExtValue();
                unsigned c;
                // Mult
                if (opCode==Instruction::Mul) {
                    c = x * y;
                } 
                // Div
                else if (opCode==Instruction::UDiv || opCode==Instruction::SDiv){
                    c = x / y;
                } else {
                    std::cout << "error: encode binary operator\n";
                    exit(1);
                }
                ExprPtr v1 = Expression::mkSInt32Num(c);
                ExprPtr v2 = Expression::mkSInt32Num(x);
                ExprPtr v3 = Expression::mkSInt32Num(y);
                std::vector<ExprPtr> exprs;
                exprs.push_back(Expression::mkEq(rVar, v1));
                exprs.push_back(Expression::mkEq(arg1Var, v2));
                exprs.push_back(Expression::mkEq(arg2Var, v3));
                ExprPtr andExpr = Expression::mkAnd(exprs);
                orExprs.push_back(andExpr);
            }
            expr = Expression::mkOr(orExprs);
        }
        return expr;
    }
    // Declare the virtual variables
    ExprPtr rVar = ctx->newVariable(r);
    ExprPtr arg1Var = ctx->getLocalVariable(bo, arg1, 0);
    if(arg1Var==NULL)
        arg1Var = ctx->newVariable(arg1);
    ExprPtr arg2Var = ctx->getLocalVariable(bo, arg2, 1);
    if(arg2Var==NULL)
        arg2Var = ctx->newVariable(arg2);
    // Make the whole expression and put it in an assert
    ExprPtr opExpr = NULL;
    switch (opCode) {
        case Instruction::Add:
            // Dummy add instruction (= x y _)
            if(bo->getOpcode()==Instruction::Add 
               && bo->getName().substr(0, 1) == "_") {
                opExpr = arg1Var;
            } else {
                //unsigned n = bo->getType()->getIntegerBitWidth();
                //opExpr = Expression::mkUSum(arg1Var, arg2Var, n);   
                opExpr = Expression::mkSum(arg1Var, arg2Var);   
            }
            break;
        case Instruction::Sub:
            opExpr = Expression::mkSub(arg1Var, arg2Var);
            break;
        case Instruction::Mul:
            opExpr = Expression::mkMul(arg1Var, arg2Var);
            break;
        case Instruction::UDiv:
        case Instruction::SDiv:
            opExpr = Expression::mkDiv(arg1Var, arg2Var);
            break;
        case Instruction::Xor:{
            // A and notB or notA and B
            ExprPtr notA = Expression::mkNot(arg1Var);
            ExprPtr notB = Expression::mkNot(arg2Var);
            ExprPtr AandnotB = Expression::mkAnd(arg1Var, notB);
            ExprPtr notAandB = Expression::mkAnd(notA, arg2Var);
            opExpr = Expression::mkOr(AandnotB, notAandB);
        } break;
        case Instruction::And:
            opExpr = Expression::mkAnd(arg1Var, arg2Var);
            break;
        case Instruction::Or:
            opExpr = Expression::mkOr(arg1Var, arg2Var);
            break;           
        case Instruction::FRem:
        case Instruction::FAdd:
        case Instruction::FMul:
        case Instruction::FSub:
        case Instruction::FDiv:
            error("floating point operations not supported.");
            break;
        case Instruction::URem:
        case Instruction::SRem:
        case Instruction::Shl:
        case Instruction::LShr:
        case Instruction::AShr: 
            error("instruction not supported.");
            break;
        default:
            llvm_unreachable("Illegal opCode");
    }
    // (= r (op arg1 arg2))
    ExprPtr eqExpr = Expression::mkEq(rVar, opExpr);
    return eqExpr; // hard or soft
}


// =============================================================================
// encode - SelectInst 
// =============================================================================
ExprPtr Encoder::encode(SelectInst *select) {
    // Get the operands
    Value *result   = dyn_cast<Value>(select);
    Value *cond     = select->getCondition();
    Value *trueVal  = select->getTrueValue(); 
    Value *falseVal = select->getFalseValue(); 
    // Declare the virtual variables
    ExprPtr resultVar = ctx->newVariable(result);
    ExprPtr condVar = ctx->getLocalVariable(select, cond, 0);
    if(condVar==NULL)
        condVar = ctx->newVariable(cond);
    ExprPtr trueValVar = ctx->getLocalVariable(select, trueVal, 1);
    if(trueValVar==NULL)
        trueValVar = ctx->newVariable(trueVal);
    ExprPtr falseValVar = ctx->getLocalVariable(select, falseVal, 2);
    if(falseValVar==NULL)
        falseValVar = ctx->newVariable(falseVal);
    // result = (cond ? trueVal : falseVal)
    // result = (ITE cond THEN trueVal ELSE falseVal)
    ExprPtr selectExpr = Expression::mkIte(condVar, trueValVar, falseValVar);
    ExprPtr eqExpr = Expression::mkEq(resultVar, selectExpr);
    return eqExpr; // hard or soft
}  


// =============================================================================
// encode - PHINode 
// =============================================================================
ExprPtr Encoder::encode(PHINode *phi, BasicBlock *forBlock) {
    // (= x val)
    // forBlock!=NULL
    if (forBlock) {
        Value *vTaken = phi->getIncomingValueForBlock(forBlock);
        if (!vTaken) {
            error("Encoder - Phi");
        }
        // TODO
        //ExprPtr valExpr = ctx->getLocalVariable(phi, vTaken, phi->getBasicBlockIndex(forBlock));
        ExprPtr valExpr = ctx->newVariable(vTaken);
        ExprPtr phiExpr = ctx->newVariable(dyn_cast<Value>(phi));
        ExprPtr eqExpr = Expression::mkEq(phiExpr, valExpr);
        return eqExpr;
    }
    // TCAS
    if (phi->getParent()->getName()=="alt_sep_test.exit") {
        ctx->setTCASAssertVarName(dyn_cast<Value>(phi)->getName().str());
    }
    // # <bb>
    // # x2 = phi [x0, <bb0>] [x1, <bb1>] 
    // (= x2 (ite bb0 x0 x1))
    Value *last_val = phi->getIncomingValue(phi->getNumIncomingValues()-1);
    ExprPtr last_valExpr = ctx->getLocalVariable(phi, last_val, phi->getNumOperands()-1);
    if(last_valExpr==NULL) {
        last_valExpr = ctx->newVariable(last_val);
    }
    // One incoming edge
    // (= phivar x)
    if(phi->getNumIncomingValues()==1) {
        ExprPtr phiExpr = ctx->newVariable(dyn_cast<Value>(phi));
        ExprPtr eqExpr = Expression::mkEq(phiExpr, last_valExpr);
        return eqExpr; // hard
    }
    // More than one incoming edge
    BasicBlock *bb = phi->getParent();
    ExprPtr iteExpr = NULL;
    for (int i = phi->getNumIncomingValues()-2; i >=0; i-=1) {
        Value *val = phi->getIncomingValue(i);
        BasicBlock *predbb = phi->getIncomingBlock(i);
        //ExprPtr predbbExpr = ctx->getCondVariable(predbb);
        ExprPtr valExpr = ctx->getLocalVariable(phi, val, i);
        if(valExpr==NULL) {
            valExpr = ctx->newVariable(val);
        }
        ExprPtr predbb_bbExpr = ctx->getTransition(predbb, bb); // t(predbb,b)
        //ExprPtr andExpr = Expression::mkAnd(predbbExpr ,predbb_bbExpr);
        iteExpr = Expression::mkIte(predbb_bbExpr, valExpr, last_valExpr);
        last_valExpr = iteExpr;
    }
    // (= x (ITE ... ))
    ExprPtr phiExpr = ctx->newVariable(dyn_cast<Value>(phi));
    ExprPtr eqExpr = Expression::mkEq(phiExpr, iteExpr);
    return eqExpr; //har
}


// =============================================================================
// encode - BranchInst 
// =============================================================================
ExprPtr Encoder::encode(BranchInst *br, LoopInfoPass *loops) {

    BasicBlock *bb = br->getParent(); 
    ExprPtr bbExpr = ctx->getCondVariable(bb);
    ExprPtr notbbExpr = Expression::mkNot(bbExpr);
    // Unconditional branching
    if(br->isUnconditional()) {
        // # <bb>
        // # br label <nextbb>
        // (= bb bb_nextbb)
        BasicBlock *nextbb = br->getSuccessor(0);
        ExprPtr bb_nextbbExpr = ctx->getTransition(bb, nextbb);        
        ExprPtr eqExpr = Expression::mkEq(bbExpr, bb_nextbbExpr);
        return eqExpr; //hard
    }
    // Conditional branching
    Value *cond = br->getCondition();
    if(!isa<ICmpInst>(cond) && !cond->getType()->isIntegerTy(1)) {
        error("br instruction with no icmp instruction.");
    }
    BasicBlock *nextBB1 = br->getSuccessor(0);
    BasicBlock *nextBB2 = br->getSuccessor(1);
    
    // Loops: Check if we branch to a loop latch basicblock
    bool nextBB1isLoopHeader = false;
    bool nextBB2isLoopHeader = false;
    BasicBlock *headerBB = loops->getLoopLatch(br);
    if (headerBB && headerBB==nextBB1) {
        // CASE1: If cond is true, got back to the loop latch block
        
        // # <bb>
        // # br i1 <cond>, label <loopLatchBB>, label <nextbb2>  
        
        // (or (and cond       -          (not bb_nextbb2))  
        //     (and (not cond) bb_nextbb2 (not bb_nextbb1))
        //     (and (not bb) (not bb_nextbb1) (not bb_nextbb2))
        // )  
        nextBB1isLoopHeader = true;
    } else if(headerBB && headerBB==nextBB2) {
        // CASE2: If cond is false, got back to the loop latch block
        
        // # <bb>
        // # br i1 <cond>, label <nextbb1>, label <loopLatchBB>  
        
        // (or (and cond       bb_nextbb1 (not bb_nextbb2))  
        //     (and (not cond) -          (not bb_nextbb1))
        //     (and (not bb) (not bb_nextbb1) (not bb_nextbb2))
        // )  
        nextBB2isLoopHeader = true;
    }
    
    
    // # <bb>
    // # br i1 <cond>, label <nextbb1>, label <nextbb2>  
    // (or (and bb       cond             bb_nextbb1 nextbb1 (not bb_nextbb2))  
    //     (and bb       (not cond)       bb_nextbb2 nextbb2 (not bb_nextbb1))
    //     (and (not bb) (not bb_nextbb1) (not bb_nextbb2))
    // ) 
    ExprPtr condExpr = ctx->getLocalVariable(br, cond, 0);
    if(condExpr==NULL) {
        condExpr = ctx->newVariable(cond);
    }
    ExprPtr notCondExpr = Expression::mkNot(condExpr);
    ExprPtr bb_nextbb1Expr = ctx->getTransition(bb, nextBB1);
    ExprPtr bb_nextbb2Expr = ctx->getTransition(bb, nextBB2);
    ExprPtr notbb_nextbb1Expr = Expression::mkNot(bb_nextbb1Expr);
    ExprPtr notbb_nextbb2Expr = Expression::mkNot(bb_nextbb2Expr);
        
      
    /*
    // Loops: Check if we branch to a loop latch basicblock
    BasicBlock *headerBB = loops->getLoopLatch(br);
    if (headerBB && (headerBB==nextBB1 || headerBB==nextBB2)) {
        ExprPtr andExpr;
        
        // CASE1: If cond is true, got back to the loop latch block
        // # <bb>
        // # br i1 <cond>, label <loopLatchBB>, label <nextbb2>  
        // (or (and bb       cond             -          -       (not bb_nextbb2))  
        //     (and bb       (not cond)       bb_nextbb2 nextbb2 (not bb_nextbb1))
        //     (and (not bb) -                (not bb_nextbb2))
        // )   
        
        // (or (and cond       -          (not bb_nextbb2))  
        //     (and (not cond) bb_nextbb2 (not bb_nextbb1))
        //     (and (not bb) (not bb_nextbb1) (not bb_nextbb2))
        // ) 
        if (headerBB==nextBB1) {
            std::vector<ExprPtr> andVec1;
            andVec1.push_back(bbExpr);
            andVec1.push_back(condExpr);
            andVec1.push_back(notbb_nextbb2Expr);
            andExpr1 = Expression::mkAnd(andVec1);
            
            std::vector<ExprPtr> andVec2;
            andVec2.push_back(bbExpr);
            andVec2.push_back(notCondExpr);
            andVec2.push_back(bb_nextbb2Expr);
            andVec2.push_back(nextbb2Expr);
            andExpr2 = Expression::mkAnd(andVec1);
            
        } 
        // CASE2: If cond is false, got back to the loop latch block
        // # <bb>
        // # br i1 <cond>, label <nextbb1>, label <loopLatchBB>  
        // (or (and bb       cond             bb_nextbb1 nextbb1 (not bb_nextbb2))  
        //     (and bb       (not cond)       -          -       (not bb_nextbb1))
        //     (and (not bb) (not bb_nextbb1) -)
        // ) 
        else if(headerBB==nextBB2) {
            loopAssertExpr = condExpr; 
            // (and loopAssrtExpr
            // (or (and cond       bb_nextbb1 (not bb_nextbb2))  
            //     ((not bb_nextbb1) (not bb_nextbb2))
            // ))
            std::vector<ExprPtr> and1;
            and1.push_back(condExpr);
            and1.push_back(bb_nextbb1Expr);
            and1.push_back(notbb_nextbb2Expr);
            andExpr = Expression::mkAnd(and1); 
        }
        std::vector<ExprPtr> and3;
        and3.push_back(notbbExpr);
        and3.push_back(notbb_nextbb1Expr);
        and3.push_back(notbb_nextbb2Expr);
        ExprPtr and3Expr = Expression::mkAnd(and3);
        std::vector<ExprPtr> or1;
        or1.push_back(andExpr);
        or1.push_back(and3Expr);
        ExprPtr or1Expr = Expression::mkOr(or1);
        ExprPtr expr = Expression::mkAnd(loopAssertExpr, or1Expr);
        formula->addLoopAssert(loopAssertExpr);
        return expr; // hard
    }*/
    
    
    
    // (or (and cond       bb_nextbb1 (not bb_nextbb2))  
    //     (and (not cond) bb_nextbb2 (not bb_nextbb1))
    //     (and (not bb) (not bb_nextbb1) (not bb_nextbb2))
    // )  
    std::vector<ExprPtr> and1;
    and1.push_back(condExpr);
    if (!nextBB1isLoopHeader) {
        and1.push_back(bb_nextbb1Expr);
    }
    and1.push_back(notbb_nextbb2Expr);
    ExprPtr and1Expr = Expression::mkAnd(and1);
    std::vector<ExprPtr> and2;
    and2.push_back(notCondExpr);
    if (!nextBB2isLoopHeader) {
        and2.push_back(bb_nextbb2Expr);
    }
    and2.push_back(notbb_nextbb1Expr);
    ExprPtr and2Expr = Expression::mkAnd(and2);
    std::vector<ExprPtr> and3;
    and3.push_back(notbbExpr);
    and3.push_back(notbb_nextbb1Expr);
    and3.push_back(notbb_nextbb2Expr);
    ExprPtr and3Expr = Expression::mkAnd(and3);
    std::vector<ExprPtr> or1;
    or1.push_back(and1Expr);
    or1.push_back(and2Expr);
    or1.push_back(and3Expr);
    ExprPtr or1Expr = Expression::mkOr(or1);   
    return or1Expr; // hard
}


// =============================================================================
// encode - SwitchInst 
// =============================================================================
ExprPtr Encoder::encode(SwitchInst *s) {
    // <bb>
    // switch i32 %x, label %defaultbb [
    //      i32 1, label %bb0
    //      i32 2, label %bb1
    //      i32 3, label %bb2
    // ]
    //
    // (ITE (= x 1) THEN (and (= bb_bb0 true) ... (_ false))
    //  ELSE (ITE (= x 2) THEN (and (= bb_bb1 true) ... (_ false))
    //        ELSE (ITE (= x 3) THEN (and (= bb_bb2 true) ... (_ false))
    //              ELSE (and (= bb_defaultbb true) ... (_ false))
    // )))
    
    Value *cond             = s->getCondition();
    BasicBlock *defaultbb   = s->getDefaultDest();
    BasicBlock *bb          = s->getParent();
    int numCase             = s->getNumCases();
    ExprPtr trueVar     = Expression::mkTrue();
    ExprPtr defTransVar = ctx->newTransition(bb, defaultbb);
    
    // Emulate an unconditional br instruction
    // switch i32 0, label %dest [ ]
    if (numCase==0) {
        // (bb_defaultbb true)
        return Expression::mkEq(defTransVar, trueVar);
    }
    ExprPtr falseVar = Expression::mkFalse();
    ExprPtr condVar  = ctx->getLocalVariable(s, cond, 0);
    if(condVar==NULL)
        condVar = ctx->newVariable(cond);
    
    // Create the case value variables and the transition variables
    std::vector<ExprPtr> caseVars;
    std::vector<ExprPtr> trueTransVars;
    std::vector<ExprPtr> falseTransVars;
    for (SwitchInst::CaseIt it = s->case_begin(), ite = s->case_end(); it != ite; ++it) {
        Value *caseVal = it.getCaseValue();
        ExprPtr e1 = ctx->newVariable(caseVal);
        caseVars.push_back(e1);
        BasicBlock *casebb = it.getCaseSuccessor();
        ExprPtr e2 = ctx->newTransition(bb, casebb); // t(bb,nextbb)
        ExprPtr trueTrans = Expression::mkEq(e2, trueVar);
        trueTransVars.push_back(trueTrans);
        ExprPtr falseTrans = Expression::mkEq(e2, falseVar);
        falseTransVars.push_back(falseTrans);
    }
      
    // lastValVar = (and (bb_defaultbb true) (bb_bb1 false) ... (bb_bbn false))
    std::vector<ExprPtr> tmp(falseTransVars);
    tmp.push_back(Expression::mkEq(defTransVar, trueVar));
    ExprPtr lastValVar = Expression::mkAnd(tmp);
    
    // (ITE ... )
    ExprPtr iteExpr = NULL;
    ExprPtr falseDefTransVar = Expression::mkEq(defTransVar, falseVar);
    for (std::size_t i=caseVars.size()-1; i>=0; --i) {
        
        // (and (= bb_bbi true) (= bb_bbj false))
        std::vector<ExprPtr> andArgs;
        for (std::size_t j=0; j<caseVars.size(); ++j) {
            if(i==j)
                andArgs.push_back(trueTransVars[j]);
            else 
                andArgs.push_back(falseTransVars[j]);
        }
        andArgs.push_back(falseDefTransVar);
        ExprPtr andExpr = Expression::mkAnd(andArgs);
        // (= cond val)
        ExprPtr condExpr = Expression::mkEq(condVar, caseVars[i]);
        // (ITE (= cond val) THEN (= bb_bbi true) ... ELSE ...)
        iteExpr = Expression::mkIte(condExpr, andExpr, lastValVar);
        lastValVar = iteExpr;
    }
    return iteExpr; // hard or soft
}


// =============================================================================
// encode - ICmpInst 
// =============================================================================
ExprPtr Encoder::encode(ICmpInst *icmp) {

    ExprPtr cond_var = ctx->newVariable(dyn_cast<Value>(icmp));
    // Process the left/right-hand-side of the expression
    Value *rhs = icmp->getOperand(0);
    Value *lhs = icmp->getOperand(1);
    ExprPtr rhs_var;
    rhs_var = ctx->getLocalVariable(icmp, rhs, 0);
    if(rhs_var==NULL) 
        rhs_var = ctx->newVariable(rhs);
    ExprPtr lhs_var;
    lhs_var = ctx->getLocalVariable(icmp, lhs, 1);
    if(lhs_var==NULL)
        lhs_var = ctx->newVariable(lhs);
    // Get the predicate used for the comparison and build the expression
    ExprPtr op_expr;
    switch (icmp->getPredicate()) {
        case ICmpInst::ICMP_EQ:
            op_expr = Expression::mkEq(rhs_var, lhs_var);
            break;
        case ICmpInst::ICMP_NE:
            op_expr = Expression::mkDiseq(rhs_var, lhs_var);
            break;
        case ICmpInst::ICMP_SLT:
        case ICmpInst::ICMP_ULT:
            op_expr = Expression::mkLt(rhs_var, lhs_var);
            break;
        case ICmpInst::ICMP_SLE:
        case ICmpInst::ICMP_ULE:
            op_expr = Expression::mkLe(rhs_var, lhs_var);
            break;
        case ICmpInst::ICMP_SGT:
        case ICmpInst::ICMP_UGT:
            op_expr = Expression::mkGt(rhs_var, lhs_var);
            break;
        case ICmpInst::ICMP_SGE:
        case ICmpInst::ICMP_UGE:
            op_expr = Expression::mkGe(rhs_var, lhs_var);
            break;
        default:
            llvm_unreachable("Illegal ICmp predicate");
    }
    // Make the expression for the comparison (icmp)
    // (assert+ (= cond_var (op rhs_var lhs_var)) w)
    ExprPtr eqExpr = Expression::mkEq(cond_var, op_expr);
    return eqExpr; // hard or soft
}

// =============================================================================
// encode - SExtInst 
//
// The ‘sext‘ sign extends value to the type ty2.
// =============================================================================
ExprPtr Encoder::encode(SExtInst *sext) {
    ExprPtr expr = NULL;
    // Create variables
    Value *src      = sext->getOperand(0);
    ExprPtr dst_var = ctx->newVariable(sext);
    ExprPtr src_var = ctx->newVariable(src);
    // Check the type of the argument
    Type *srcTy = sext->getSrcTy();
    Type *dstTy = sext->getDestTy();
    // Bool -> i32
    if (srcTy->isIntegerTy(1) && (dstTy->isIntegerTy(8) || dstTy->isIntegerTy(16) 
        || dstTy->isIntegerTy(32) || dstTy->isIntegerTy(64))) {
        // When sign extending from i1, the 
        // extension always results in -1 or 0.
        ExprPtr zero     = Expression::mkSInt32Num(0);
        ExprPtr minusone = Expression::mkSInt32Num(-1);
        ExprPtr iteExpr  = Expression::mkIte(src_var, minusone, zero);
        expr             = Expression::mkEq(dst_var, iteExpr);
    }
    // i32 -> i64
    else if ((srcTy->isIntegerTy(8) || srcTy->isIntegerTy(16) || srcTy->isIntegerTy(32)) 
             && (srcTy->isIntegerTy(16) || srcTy->isIntegerTy(32) || srcTy->isIntegerTy(64))) {
        // Warning: incorrect semantic !!
        // Sext src to dst : (and (= dst src) (>= src 0))
        ExprPtr zero   = Expression::mkSInt32Num(0);
        ExprPtr geExpr = Expression::mkGe(src_var, zero);
        ExprPtr eqExpr = Expression::mkEq(dst_var, src_var);
        expr           = Expression::mkAnd(eqExpr, geExpr);
    }   
    else {
        error("unsupported type for SExt instruction.");
    }
    return expr; //hard
}

// =============================================================================
// encode - ZExtInst 
//
// The ‘zext‘ instruction zero extends its operand to type ty2.
// =============================================================================
ExprPtr Encoder::encode(ZExtInst *zext) {
    ExprPtr expr = NULL;
    // Create variables
    Value *src      = zext->getOperand(0);
    ExprPtr dst_var = ctx->newVariable(zext);
    ExprPtr src_var = ctx->newVariable(src);
    // Check the type of the argument
    Type *srcTy = zext->getSrcTy();
    Type *dstTy = zext->getDestTy();
    // Bool -> i32
    if (srcTy->isIntegerTy(1) && (dstTy->isIntegerTy(8) || dstTy->isIntegerTy(16) 
       || dstTy->isIntegerTy(32) || dstTy->isIntegerTy(64))) {
        // When zero extending from i1, the result 
        // will always be either 0 or 1.
        // (= zext (if zarg 1 0))
        ExprPtr zero    = Expression::mkSInt32Num(0);
        ExprPtr one     = Expression::mkSInt32Num(1);
        ExprPtr iteExpr = Expression::mkIte(src_var, one, zero);
        expr            = Expression::mkEq(dst_var, iteExpr);
    } 
    // i32 -> i64
    else if ((srcTy->isIntegerTy(8) || srcTy->isIntegerTy(16) || srcTy->isIntegerTy(32)) 
             && (srcTy->isIntegerTy(16) || srcTy->isIntegerTy(32) || srcTy->isIntegerTy(64))) {
        if (srcTy->isIntegerTy(64)) std::cout << "warning: i64 type used\n";
        expr = Expression::mkEq(dst_var, src_var);
    }
    else {
        error("unsupported type for ZExt instruction.");
    }
    return expr; //hard
}
   
// =============================================================================
// encode - CallInst
// =============================================================================
ExprPtr Encoder::encode(CallInst *call, Formula *formula) {
    
    StringRef functionName;
    Function *F = call->getCalledFunction();
    // Direct call
    if (F) {
        functionName = F->getName();
    } 
    // Indirect call
    else {
        F = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
        if (F) {
            functionName = F->getName();
        } else {
            std::cout << "warning: unresolvable indirect function call.\n";
            exit(1);
        }
    }
    // Assumes/Asserts
    if(functionName==SNIPER_ASSERT_RETINT_FUN_NAME
       || functionName==SNIPER_ASSERT_RETVOID_FUN_NAME
       || functionName==SNIPER_ASSUME_RETINT_FUN_NAME
       || functionName==SNIPER_ASSUME_RETVOID_FUN_NAME
       || functionName==ASSERT_FUN_NAME || functionName==ASSUME_FUN_NAME) {
        Value *argOp;
        if (functionName==SNIPER_ASSERT_RETINT_FUN_NAME
            || functionName==SNIPER_ASSERT_RETVOID_FUN_NAME) {
             argOp = call->getArgOperand(1);
        } else {
             argOp = call->getArgOperand(0);
        }
        ExprPtr arg_var = NULL;
        if (ZExtInst *zext = dyn_cast<ZExtInst>(argOp)) {
            arg_var = ctx->newVariable(zext);
        } else if (isa<PHINode>(argOp)) {
            arg_var = ctx->newVariable(argOp);
        } else {
            error("cannot process assert call");
        }   
        BasicBlock *bb = call->getParent();
        ExprPtr arg_bb = ctx->getCondVariable(bb); 
        
        // PRE/POST: (and BB (= tmp 1))  notPOST: (and BB (= tmp 0))
        ExprPtr one_expr = Expression::mkSInt32Num(1);
        ExprPtr zero_expr = Expression::mkSInt32Num(0);
        ExprPtr eqOne_expr = Expression::mkEq(arg_var, one_expr);
        ExprPtr eqZero_expr = Expression::mkEq(arg_var, zero_expr);
        
        // Pre-condition
        if (functionName==SNIPER_ASSUME_RETINT_FUN_NAME
            || functionName==SNIPER_ASSUME_RETVOID_FUN_NAME
            || functionName==ASSUME_FUN_NAME) {
            //ExprPtr and_expr = Expression::mkAnd(arg_bb, eqOne_expr);
            return eqOne_expr; // hard
        }
        // Post-condition
        else if(functionName==SNIPER_ASSERT_RETINT_FUN_NAME
                || functionName==SNIPER_ASSERT_RETVOID_FUN_NAME
                || functionName==ASSERT_FUN_NAME) {
            //ExprPtr and_expr1 = Expression::mkAnd(arg_bb, eqOne_expr);
            formula->addPostCondition(eqOne_expr); //POST
            //ExprPtr and_expr2 = Expression::mkAnd(arg_bb, eqZero_expr);
            formula->addNotPostCondition(eqZero_expr); //notPOST
            return NULL;
        } else {
            error("cannot process assert call");
        }
    } 
    // Concolic profiling functions
    else if (functionName.startswith(SNIPER_FUN_PREFIX)) {
            return NULL;
    }
    // Special assert for loops
    else if(call->getNumArgOperands()==1 && functionName==LOOP_ASSERT_FUN_NAME) {
        BasicBlock *bb = call->getParent();
        ExprPtr arg_bb = ctx->getCondVariable(bb); 
        ExprPtr notbb_expr = Expression::mkNot(arg_bb);
        return notbb_expr; // hard
    } 
    // Calls to llvm function (instrinsic)
    else if(F->isIntrinsic()) {
        return NULL;
    }
    // Call to 'atoi' 
    else if(call->getNumArgOperands()==1 && functionName=="atoi") {
        Value *arg = call->getArgOperand(0);
        ExprPtr argExpr = ctx->newVariable(arg);
        ExprPtr callExpr = ctx->newVariable(call);
        ExprPtr eqExpr = Expression::mkEq(callExpr, argExpr);
        // Assert as soft
        //ExprPtr e = new Expression(eqExpr, true, call);
        return eqExpr; // hard
    } else {
        // Check if we collected info about the called function
        FunctionSummary *fs = NULL;//profile->getFunSummary(F);
        if (fs) {
            Value *arg = call->getArgOperand(0);
            ExprPtr argExpr = ctx->newVariable(arg);
            ExprPtr callExpr = ctx->newVariable(call);            
            // (or (and (= x in1) (= y out1))
            //     (and (= x in2) (= y out2))
            // )
            std::vector<ExprPtr> orExprs;
            std::vector<io_t> inOuts = fs->getInputOutputs();
            std::vector<io_t>::const_iterator it;
            for (it=inOuts.begin(); it!=inOuts.end(); ++it) {
                io_t i = *it;
                if (i.inputs.size()!=1) {
                    std::cout << "error: call with more than one arg\n";
                    exit(1);
                }
                ConstantInt *CI1 = dyn_cast<ConstantInt>(i.inputs[0]);
                int input = CI1->getSExtValue();
                ConstantInt *CI2 = dyn_cast<ConstantInt>(i.output);
                int output = CI2->getSExtValue();
                ExprPtr out = Expression::mkSInt32Num(output);
                ExprPtr eqExpr1 = Expression::mkEq(callExpr, out);
                ExprPtr in = Expression::mkSInt32Num(input);
                ExprPtr eqExpr2 = Expression::mkEq(argExpr, in);
                ExprPtr andExpr = Expression::mkAnd(eqExpr1, eqExpr2);
                orExprs.push_back(andExpr);
            }
            return Expression::mkOr(orExprs);
        } else {
            // Calls to other functions
            unsigned line = 0;
            if (MDNode *N = call->getMetadata("dbg")) { 
                DILocation Loc(N); 
                line = Loc.getLineNumber();
            }
            std::cout << "warning: the function " << functionName.str();
            std::cout << " (line " << line << ") was not inlined!\n";
            //exit(1);
            return NULL;
        }
    }
    return NULL;
}
       
// =============================================================================
// encode - AllocaInst
// =============================================================================
ExprPtr Encoder::encode(AllocaInst *alloca) {
    const Type *t = alloca->getAllocatedType();
    uint64_t size = 1;
    // Static integer array
    if(const ArrayType *aty = dyn_cast<ArrayType>(t)) {
        const Type *eltty = aty->getElementType();
        if (!eltty->isIntegerTy()) {
            eltty->dump();
            error("only integer type is supported.");
        }
        size = aty->getNumElements();
    }
    // Dynamic integer array
    else if (t->isIntegerTy() && alloca->isArrayAllocation()) {
        Value *sizeVal = alloca->getArraySize();
        if (ConstantInt *CI = dyn_cast<ConstantInt>(sizeVal)) {
            size = CI->getSExtValue();
        } else {
            error("dynamic array allocation not supported.");
        }
    }
    // Simple pointer on integer
    else if (t->isIntegerTy()) {
        size = 1;
    } else {
        std::cout << "warning: unsupported pointer.\n";
        alloca->dump();
        return NULL;
        
        //t->dump();
        //error("unsupported pointer.");
    }
    // Generate a new ID for this pointer
    ctx->addPtrId(alloca, size);
    return NULL;
}

// =============================================================================
// encode - StoreInst
// =============================================================================
ExprPtr Encoder::encode(StoreInst *store) {
    // Argv checking
    Value *ptr = store->getPointerOperand();
    GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(ptr);
    if (gep && isArgv(gep->getPointerOperand())) {     
        error("Dynamic modifications of the argv array are not allowed.");
    }
    // Get the index
    ExprPtr idx_expr = ctx->getVariable(ptr);
    // Get the value
    Value *value = store->getValueOperand();
    ExprPtr val_expr = ctx->newVariable(value);
    //Get the MEM    
    ExprPtr mem_expr1;
    unsigned memID = ctx->getMemId(store->getParent());
    if (memID==0) {
        mem_expr1 = Expression::mkIntToIntVar("MEM0");
    } else {        
        std::stringstream sstm1;
        sstm1 << "MEM" << ctx->getMemId(store->getParent());
        std::string name1 = sstm1.str();
        mem_expr1 = Expression::mkIntToIntVar(name1);
    }
    // Update the pointer ID
    ctx->updateMemId(store);
    // Create a new un-function
    memID = ctx->getCurrentMemId();
    std::stringstream sstm2;
    sstm2 << "MEM" << memID;
    std::string name2 = sstm2.str();    
    ExprPtr mem_expr2= Expression::mkIntToIntVar(name2);
    ExprPtr update_expr = 
    Expression::mkFunctionUpdate(mem_expr1, idx_expr, val_expr);
    ExprPtr eqExpr = Expression::mkEq(mem_expr2, update_expr);
    return eqExpr; // soft
}

// =============================================================================
// encode - LoadInst 
// =============================================================================
ExprPtr Encoder::encode(LoadInst *load, Formula *formula) {
    Value *ptr = load->getPointerOperand();
    ExprPtr mem_expr;
    // Argv
    unsigned memID = ctx->getMemId(load->getParent());
    GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(ptr);
    if (!gep) {
        //error("cannot encode load instruction.");
        std::cout << "warning: cannot encode load instruction.\n";
        load->dump();
        return NULL;
    }
    if (isArgv(gep->getPointerOperand())) { 
        // Get the un-fun ARGV
        mem_expr = Expression::mkIntToIntVar("ARGV");
        
        // TMP
        ExprPtr lhs_expr = ctx->newVariable(load);
        ExprPtr appargExpr = ctx->getVariable(ptr);
        ExprPtr app_expr = Expression::mkApp(mem_expr, appargExpr); 
        ExprPtr eqExpr = Expression::mkEq(lhs_expr, app_expr);
        formula->assertHard(eqExpr);
        return NULL;
    }
    // Shared memory other than argv
    else if (memID==0) {
        mem_expr = Expression::mkIntToIntVar("MEM0");
    } else {
        std::stringstream sstm;
        sstm << "MEM" << memID;
        std::string name = sstm.str();        
        mem_expr = Expression::mkIntToIntVar(name);
    }
    ExprPtr lhs_expr = ctx->newVariable(load);
    ExprPtr appargExpr = ctx->getVariable(ptr);
    ExprPtr app_expr = Expression::mkApp(mem_expr, appargExpr); 
    ExprPtr eqExpr = Expression::mkEq(lhs_expr, app_expr);
    return eqExpr; // hard or soft
}

// =============================================================================
// encode - GetElementPtrInst
// =============================================================================
ExprPtr Encoder::encode(GetElementPtrInst *gep) {    
    ExprPtr eqExpr = NULL;
    if(AllocaInst *a = dyn_cast<AllocaInst>(gep->getPointerOperand())) {
        // Array (size > 1)
        if(!gep->hasIndices()) {
            error("Pointers not supported!");
        }
        Type *ty = a->getAllocatedType();
        int opId = 0;
        if (ty->isIntegerTy()) {
            opId = 1;
        } else if (ty->isArrayTy()) {
            opId = 2;
        } else {
            error("Pointers not supported!");
        }
        // (= tmp (+ id index))
        Value *index = gep->getOperand(opId);
        ExprPtr indexExpr = ctx->newVariable(index);
        ExprPtr gepExpr = ctx->newVariable(gep);
        unsigned id = ctx->getPtrId(a);
        ExprPtr opExpr = Expression::mkSum(Expression::mkSInt32Num(id), indexExpr);
        ExprPtr assignExpr = Expression::mkEq(gepExpr, opExpr);
        // (< index size)
        ExprPtr arraySizeExpr;
        Value *arraySizeVal = a->getArraySize();
        if (isa<ConstantInt>(arraySizeVal)) {
            uint64_t size = ty->getArrayNumElements();
            arraySizeExpr = Expression::mkSInt32Num(size);
        } else {
            arraySizeExpr = ctx->newVariable(arraySizeVal);
        }
        ExprPtr ltSizeExpr = Expression::mkLt(indexExpr, arraySizeExpr);
        // TCAS : (and (= x (tab index) (ite bb (< index size) true))
        BasicBlock *bb = gep->getParent();
        ExprPtr bbExpr = ctx->getCondVariable(bb);
        if (!bbExpr) {
            bbExpr = ctx->newVariable(bb);
        }
        ExprPtr iteExpr = Expression::mkIte(bbExpr, ltSizeExpr, Expression::mkTrue());
        ExprPtr andExpr = Expression::mkAnd(assignExpr, iteExpr);
        eqExpr = andExpr;
    }
    // Argv
    else if (isArgv(gep->getPointerOperand())) {
        if(!gep->hasIndices()) {
            error("Pointers not supported!");
        }
        Value *index = gep->getOperand(1);
        if(ConstantInt *ci = dyn_cast<ConstantInt>(index)) {
            int cst = (int) ci->getSExtValue();
            if (cst>(int)ctx->getArgvMaxIndex()) ctx->setArgvMaxIndex(cst);
        }
        ExprPtr index_expr = ctx->newVariable(index);
        ExprPtr gep_expr = ctx->newVariable(gep);
        eqExpr = Expression::mkEq(gep_expr, index_expr);
    }
    // Global allocation
    else if(isa<GlobalVariable>(gep->getPointerOperand())) {
        error("global pointer not supported.");
    } else {
        error("chained pointers not supported.");
    }
    return eqExpr; //hard
}



// =============================================================================
// encodeControlFlow 
// =============================================================================
ExprPtr Encoder::encodeControlFlow(Function *targetFun) {
    
    // (= entry true)
    BasicBlock *entry = &targetFun->getEntryBlock();
    ExprPtr entryVar = ctx->newVariable(dyn_cast<Value>(entry));
    ExprPtr trueVar = Expression::mkTrue();
    ExprPtr eqExpr = Expression::mkEq(entryVar, trueVar);
    // Assert as hard
    std::vector<ExprPtr> cfgAndExpr;
    cfgAndExpr.push_back(eqExpr);
    
    // Iterating over the BasicBlocks to create 
    // all the basicblock variables and the transition variables
    for (Function::iterator i = targetFun->begin(), e = targetFun->end(); i != e; ++i) {
        ExprPtr bbExpr = ctx->newVariable(dyn_cast<Value>(i)); 
        ExprPtr notbbExpr = Expression::mkNot(bbExpr);
        std::vector<ExprPtr> predbbTrans;
        std::vector<ExprPtr> prednotbbTrans;
        for (pred_iterator PI = pred_begin(i), E = pred_end(i); PI != E; ++PI) {
            BasicBlock *pred = *PI;
            ExprPtr bb_predbbExpr = ctx->newTransition(pred, i); // t(b',b)
            predbbTrans.push_back(bb_predbbExpr);
            ExprPtr notbb_predbbExpr = Expression::mkNot(bb_predbbExpr);
            prednotbbTrans.push_back(notbb_predbbExpr);
        }
        if (predbbTrans.size()==0) {
            // No predecessors!
        } else {
            // (or (and bb       E0_bb       (not E1_bb) ... (not En_bb))
            //     (and bb       (not E0_bb) E1_bb       ... (not En_bb))
            //     ...
            //     (and bb       (not E0_bb) (not E1_bb) ... En_bb)
            //     (and (not bb) (not E0_bb) (not E1_bb) ... (not E2_bb))
            // )
            std::vector<ExprPtr> orVec;
            for (unsigned i = 0; i<predbbTrans.size(); i++) {
                ExprPtr E1 = predbbTrans[i];
                // (and bb ... )
                std::vector<ExprPtr> andVec;
                andVec.push_back(bbExpr);
                for ( unsigned j = 0; j<predbbTrans.size(); j++) {
                    ExprPtr E2 = predbbTrans[j];
                    if (E1==E2) {
                        andVec.push_back(E2);
                    } else {
                        ExprPtr notE2 = prednotbbTrans[j];
                        andVec.push_back(notE2);
                    } 
                }
                ExprPtr andExpr = Expression::mkAnd(andVec);
                orVec.push_back(andExpr);
            }
            // (and (not bb) (not E0_bb) (not E1_bb) ... (not E2_bb))
            std::vector<ExprPtr> andVec;
            andVec.push_back(notbbExpr);
            std::vector<ExprPtr>::iterator itp3;
            for (itp3=prednotbbTrans.begin(); itp3!=prednotbbTrans.end(); ++itp3) {
                ExprPtr notE3 = *itp3;
                andVec.push_back(notE3);
            }
            ExprPtr andExpr = Expression::mkAnd(andVec);
            orVec.push_back(andExpr);
            // (or ... )
            ExprPtr orExpr = Expression::mkOr(orVec);
            // Assert as hard
            cfgAndExpr.push_back(orExpr);
        }
    }
    return Expression::mkAnd(cfgAndExpr);
}

// =============================================================================
// isArgv
// =============================================================================
bool Encoder::isArgv(const Value* V) {
    const Type* T = V->getType();
    return T->isPointerTy() 
    && T->getContainedType(0)->isPointerTy()
    && V->getName()=="argv";
}