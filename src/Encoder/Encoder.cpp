/**
 * \file Encoder.cpp
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
 * \date   3 March 2016
 */

#include "Encoder.h"

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
    if(opCode==Instruction::Mul || opCode==Instruction::FMul ||
       opCode==Instruction::UDiv || opCode==Instruction::SDiv ||
       opCode==Instruction::FDiv) {
        assert((isa<ConstantInt>(arg1) || isa<ConstantInt>(arg2)) &&
               "Non-linear arithmetic not supported!");
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
            assert("Floating point operations are not supported!");
            break;
        case Instruction::URem:
        case Instruction::SRem:
        case Instruction::Shl:
        case Instruction::LShr:
        case Instruction::AShr: 
            assert("Instruction not supported.");
            break;
        default:
            llvm_unreachable("Illegal opCode");
    }
    // (= r (op arg1 arg2))
    ExprPtr eqExpr = Expression::mkEq(rVar, opExpr);
    eqExpr->setInstruction(bo);
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
    eqExpr->setInstruction(select);
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
        assert(vTaken && "No phi incoming value!");
        // TODO
        //ExprPtr valExpr = ctx->getLocalVariable(phi, vTaken, phi->getBasicBlockIndex(forBlock));
        ExprPtr valExpr = ctx->newVariable(vTaken);
        ExprPtr phiExpr = ctx->newVariable(dyn_cast<Value>(phi));
        ExprPtr eqExpr = Expression::mkEq(phiExpr, valExpr);
        eqExpr->setInstruction(phi);
        return eqExpr;
    }
    // TCAS
    if (phi->getParent()->getName()=="alt_sep_test.exit") {
        ctx->setTCASAssertVarName(dyn_cast<Value>(phi)->getName().str());
    }
    
    // One incoming edge
    // # <bb>
    // # x2 = phi [x0, <bb0>]
    // (= x2 x0)
    if(phi->getNumIncomingValues()==1) {
        Value *last_val = phi->getIncomingValue(phi->getNumIncomingValues()-1);
        ExprPtr valExpr = ctx->getLocalVariable(phi, last_val, phi->getNumOperands()-1);
        if(valExpr==NULL) {
            valExpr = ctx->newVariable(last_val);
        }
        ExprPtr phiExpr = ctx->newVariable(dyn_cast<Value>(phi));
        ExprPtr eqExpr = Expression::mkEq(phiExpr, valExpr);
        eqExpr->setInstruction(phi);
        return eqExpr; // hard
    }
    
    // More than one incoming edge
    // # <bb>
    // # x2 = phi [x0, <bb0>] [x1, <bb1>]
    // (= x2 (ite bb0_bb x0 (ite bb1_bb x1 x2)))
    ExprPtr phiExpr = ctx->newVariable(dyn_cast<Value>(phi));
    ExprPtr last_valExpr = phiExpr;
    BasicBlock *bb = phi->getParent();
    ExprPtr iteExpr = NULL;
    for (int i = phi->getNumIncomingValues()-1; i >=0; i-=1) { // reverse
        Value *val = phi->getIncomingValue(i);
        BasicBlock *predbb = phi->getIncomingBlock(i);
        ExprPtr valExpr = ctx->getLocalVariable(phi, val, i);
        if(valExpr==NULL) {
            valExpr = ctx->newVariable(val);
        }
        ExprPtr predbb_bbExpr = ctx->getTransition(predbb, bb); // t(predbb,b)
        iteExpr = Expression::mkIte(predbb_bbExpr, valExpr, last_valExpr);
        last_valExpr = iteExpr;
    }
    // (= x (ITE ... ))
    ExprPtr eqExpr = Expression::mkEq(phiExpr, iteExpr);
    eqExpr->setInstruction(phi);
    return eqExpr; //har
}


// =============================================================================
// encode - BranchInst 
// =============================================================================
ExprPtr Encoder::encode(BranchInst *br, LoopInfoPass *loops) {

    ExprPtr brExpr = NULL;
    BasicBlock *bb = br->getParent();
    
    // --- Unconditional branching ---
    if(br->isUnconditional()) {
        BasicBlock *nextbb       = br->getSuccessor(0);
        ExprPtr    bb_nextbbExpr = ctx->getTransition(bb, nextbb);
        // No predecessor (entry block)
        BasicBlock *entryBB = &bb->getParent()->getEntryBlock();
        if (bb==entryBB) {
            // # bb:
            // # br label <nextbb>
            // (= bb_nextbb true)
            ExprPtr trueExpr = Expression::mkTrue();
            brExpr = Expression::mkEq(bb_nextbbExpr, trueExpr);
        }
        // One or more predecessors
        else {
            // # bb:         ; preds = %predbb1,..., predbbn
            // # br label <nextbb>
            // (= bb_nextbb (or predbb1_bb ... predbbn_bb))
            std::vector<ExprPtr> predTrans;
            for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
                BasicBlock *pred = *PI;
                ExprPtr    predbbi_bbExpr = ctx->getTransition(pred, bb);
                predTrans.push_back(predbbi_bbExpr);
            }
            ExprPtr orExpr = Expression::mkOr(predTrans);
            brExpr = Expression::mkEq(bb_nextbbExpr, orExpr);
        }
    }
    // --- Conditional branching ---
    else {
        
        Value *cond = br->getCondition();
        if(!isa<ICmpInst>(cond) && !cond->getType()->isIntegerTy(1)) {
            assert("br instruction with no icmp instruction.");
        }
        ExprPtr condExpr = ctx->getLocalVariable(br, cond, 0);
        if(condExpr==NULL) {
            condExpr = ctx->newVariable(cond);
        }
        ExprPtr notCondExpr = Expression::mkNot(condExpr);
        BasicBlock *nextBB1 = br->getSuccessor(0);
        BasicBlock *nextBB2 = br->getSuccessor(1);
        ExprPtr bb_nextbb1Expr = ctx->getTransition(bb, nextBB1);
        ExprPtr bb_nextbb2Expr = ctx->getTransition(bb, nextBB2);
        // Loops: Check if we branch to a loop latch basicblock
        assert(!loops->getLoopLatch(br) && "br instruction: loop latch");
        
        // No predecessor (entry block)
        BasicBlock *entryBB = &bb->getParent()->getEntryBlock();
        if (bb==entryBB) {
            // # bb:
            // # br cond <nextbb1> <nextbb2>
            // (and (= bb_nextbb1 cond) (= bb_nextbb2 (not cond)))
            ExprPtr e1 = Expression::mkEq(bb_nextbb1Expr, condExpr);
            ExprPtr e2 = Expression::mkEq(bb_nextbb2Expr, notCondExpr);
            brExpr = Expression::mkAnd(e1, e2);
        }
        // One or more predecessors
        else {
            // # bb:         ; preds = %predbb1,..., predbbn
            // # br cond <nextbb1> <nextbb2>
            // (and (= bb_nextbb1 (and cond       (or predbb1_bb ... predbbn_bb)))
            //      (= bb_nextbb2 (and (not cond) (or predbb1_bb ... predbbn_bb))))
            std::vector<ExprPtr> predTrans;
            for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
                BasicBlock *pred = *PI;
                ExprPtr    predbbi_bbExpr = ctx->getTransition(pred, bb);
                predTrans.push_back(predbbi_bbExpr);
            }
            ExprPtr orExpr = Expression::mkOr(predTrans);
            ExprPtr e1 = Expression::mkAnd(condExpr,    orExpr);
            ExprPtr e2 = Expression::mkAnd(notCondExpr, orExpr);
            ExprPtr e11 = Expression::mkEq(bb_nextbb1Expr, e1);
            ExprPtr e22 = Expression::mkEq(bb_nextbb2Expr, e2);
            brExpr = Expression::mkAnd(e11, e22);
        }
    }
    brExpr->setInstruction(br);
    return brExpr;
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
        ExprPtr eqExpr = Expression::mkEq(defTransVar, trueVar);
        eqExpr->setInstruction(s);
        return eqExpr;
    }
    ExprPtr falseVar = Expression::mkFalse();
    ExprPtr condVar  = ctx->getLocalVariable(s, cond, 0);
    if(condVar==NULL) {
        condVar = ctx->newVariable(cond);
    }
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
    iteExpr->setInstruction(s);
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
    eqExpr->setInstruction(icmp);
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
        assert("Unsupported type for SExt instruction!");
    }
    expr->setInstruction(sext);
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
        assert("Unsupported type for ZExt instruction!");
    }
    expr->setInstruction(zext);
    return expr; //hard
}
   
// =============================================================================
// encode - CallInst
// =============================================================================
ExprPtr Encoder::encode(CallInst *call, Formula *preCond, Formula *postCond) {
    
    StringRef functionName;
    Function *F = call->getCalledFunction();
    // Direct call
    if (F) {
        functionName = F->getName();
    } 
    // Indirect call
    else {
        F = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
        assert(F && "Unresolvable indirect function call!");
        functionName = F->getName();
    }
    // Assert - Post-condition
    if(functionName==Frontend::SNIPER_ASSERT_FUN_NAME ||
       functionName=="sniper_reportAssert") {
        unsigned i = (functionName==Frontend::SNIPER_ASSERT_FUN_NAME?0:1);
        Value *argOp    = call->getArgOperand(i);
        ExprPtr argExpr = ctx->newVariable(argOp);
        postCond->add(argExpr);
        return NULL;
    }
    // Assume - Pre-condition
    else if(functionName==Frontend::SNIPER_ASSUME_FUN_NAME ||
            functionName=="sniper_reportAssume") {
        unsigned i = (functionName==Frontend::SNIPER_ASSUME_FUN_NAME?0:1);
        Value *argOp    = call->getArgOperand(i);
        ExprPtr argExpr = ctx->newVariable(argOp);
        preCond->add(argExpr);
        return NULL;
    }
    // Concolic profiling functions
    else if (functionName.startswith(Frontend::SNIPER_FUN_PREFIX)) {
            return NULL;
    }
    // Special assert for loops
    else if(call->getNumArgOperands()==1 &&
            functionName==Frontend::SNIPER_LOOP_ASSERT_FUN_NAME) {
        BasicBlock *bb = call->getParent();
        ExprPtr arg_bb = ctx->getCondVariable(bb); 
        ExprPtr notbb_expr = Expression::mkNot(arg_bb);
        notbb_expr->setInstruction(call);
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
        eqExpr->setInstruction(call);
        return eqExpr; // hard
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
        assert(eltty->isIntegerTy() && "Only integer type is supported!");
        size = aty->getNumElements();
    }
    // Dynamic integer array
    else if (t->isIntegerTy() && alloca->isArrayAllocation()) {
        Value *sizeVal = alloca->getArraySize();
        ConstantInt *CI = dyn_cast<ConstantInt>(sizeVal);
        assert(CI && "Dynamic array allocation not supported!");
        size = CI->getSExtValue();
    }
    // Simple pointer on integer
    else if (t->isIntegerTy()) {
        size = 1;
    } else {
        std::cout << "warning: unsupported pointer.\n";
        alloca->dump();
        return NULL;
        
        //t->dump();
        //assert("unsupported pointer.");
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
        assert("Dynamic modifications of the argv array are not allowed!");
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
    eqExpr->setInstruction(store);
    return eqExpr; // soft
}

// =============================================================================
// encode - LoadInst 
// =============================================================================
ExprPtr Encoder::encode(LoadInst *load, Formula *AS) {
    Value *ptr = load->getPointerOperand();
    ExprPtr mem_expr;
    // Argv
    unsigned memID = ctx->getMemId(load->getParent());
    GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(ptr);
    if (!gep) {
        //assert("Cannot encode load instruction!");
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
        AS->add(eqExpr);
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
    eqExpr->setInstruction(load);
    return eqExpr; // hard or soft
}

// =============================================================================
// encode - GetElementPtrInst
// =============================================================================
ExprPtr Encoder::encode(GetElementPtrInst *gep) {    
    ExprPtr eqExpr = NULL;
    if(AllocaInst *a = dyn_cast<AllocaInst>(gep->getPointerOperand())) {
        // Array (size > 1)
        assert(gep->hasIndices() && "Pointers are not supported!");
        Type *ty = a->getAllocatedType();
        int opId = 0;
        if (ty->isIntegerTy()) {
            opId = 1;
        } else if (ty->isArrayTy()) {
            opId = 2;
        } else {
            assert("Pointers are not supported!");
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
        
        //ExprPtr zero = Expression::mkSInt32Num(0);
        //ExprPtr geSizeExpr = Expression::mkGe(indexExpr, zero);
        //ExprPtr sizeExpr = Expression::mkAnd(geSizeExpr, ltSizeExpr);
        
        // TCAS : (and (= x (tab index) (ite (or predbb0_bb..predbbn_bb) (< index size) true))
        BasicBlock *bb = gep->getParent();
        std::vector<ExprPtr> predTrans;
        for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
            BasicBlock *pred = *PI;
            ExprPtr    predbbi_bbExpr = ctx->getTransition(pred, bb);
            predTrans.push_back(predbbi_bbExpr);
        }
        ExprPtr sizeCheckExpr = NULL;
        if (predTrans.empty()) {
            sizeCheckExpr = ltSizeExpr;
        } else {
            ExprPtr orExpr = Expression::mkOr(predTrans);
            sizeCheckExpr  = Expression::mkIte(orExpr, ltSizeExpr, Expression::mkTrue());
        }
        ExprPtr andExpr = Expression::mkAnd(assignExpr, sizeCheckExpr);
        eqExpr = andExpr;
    }
    // Argv
    else if (isArgv(gep->getPointerOperand())) {
        assert(gep->hasIndices() && "Pointers are not supported!");
        Value *index = gep->getOperand(1);
        ExprPtr index_expr = ctx->newVariable(index);
        ExprPtr gep_expr = ctx->newVariable(gep);
        eqExpr = Expression::mkEq(gep_expr, index_expr);
    }
    // Global allocation
    else if(isa<GlobalVariable>(gep->getPointerOperand())) {
        assert("Global pointers are not supported!");
    } else {
        assert("Chained pointers are not supported!");
    }
    eqExpr->setInstruction(gep);
    return eqExpr; //hard
}

// =============================================================================
// encode - ReturnInst
// =============================================================================
ExprPtr Encoder::encode(ReturnInst *ret) {
    assert(ret && "Instruction is null!");
    BasicBlock *bb = ret->getParent();
    std::vector<ExprPtr> predTrans;
    for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; ++PI) {
        BasicBlock *pred = *PI;
        ExprPtr t = ctx->getTransition(pred, bb); // t(b',b)
        predTrans.push_back(t);
    }
    if (!predTrans.empty()) {
        ExprPtr e = Expression::mkOr(predTrans);
        e->setInstruction(ret);
        return e;
    } else {
        return NULL;
    }
}

// =============================================================================
// prepareControlFlow
//
// Create all transitions variables to be available
// for encoding the BR and PHI instructions
// =============================================================================
void Encoder::prepareControlFlow(Function *targetFun) {
    // Iterate over the BasicBlocks to
    // create the transition variables
    for (Function::iterator i = targetFun->begin(), e = targetFun->end(); i != e; ++i) {
        for (pred_iterator PI = pred_begin(i), E = pred_end(i); PI != E; ++PI) {
            BasicBlock *pred = *PI;
            ctx->newTransition(pred, i); // t(b',b)
        }
    }
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