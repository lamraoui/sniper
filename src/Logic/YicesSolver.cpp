/**
 * YicesSolver.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/07/05
 * @copyright : NII 2013
 */

#include "YicesSolver.h"


void YicesSolver::addToContext(ExprPtr e) {
    assert(ctx && "Context is null!");
    yices_expr expr = makeYicesExpression(e);
    if (e->isHard()) {
        // Hard assert
        yices_assert(ctx, expr);
    } else {
        // Soft assert
        assertion_id i = yices_assert_weighted(ctx, expr, 1);
        expr2ids[e]    = i;
        expr2yexpr[e]  = expr;
    }
}

void YicesSolver::addToContext(Formula *f) {
    assert(f && "Expecting a valid formula!");
    std::vector<ExprPtr> E = f->getExprs();
    for(ExprPtr e : E) {
        addToContext(e);
    }
}

int YicesSolver::maxSat() {
    assert(ctx && "Context is null!");
    // Solve the formula
    int val = yices_max_sat(ctx);
    // Save the model
    model = yices_get_model(ctx);
    return val;
}
int YicesSolver::check() {
    assert(ctx && "Context is null!");
    // Solve the formula
    int val = yices_check(ctx);
    // Save the model
    model = yices_get_model(ctx);
    return val;
}
void YicesSolver::push() { 
    yices_push(ctx);
}

void YicesSolver::pop() {
    yices_pop(ctx);
}

int YicesSolver::maxSat(Formula *f) {
    assert(f && "Expecting a valid formula!");
    assert(ctx && "Context is null!");
    // Construct and assert the Yices expression
    addToContext(f);
    // Solve the formula
    int val = yices_max_sat(ctx);
    // Save the model
    model = yices_get_model(ctx);
    return val;
}

int YicesSolver::check(Formula *f) {
    assert(f && "Expecting a valid formula!");
    assert(ctx && "Context is null!");
    // Construct and assert the Yices expression
    addToContext(f);
    // Solve the formula
    int val = yices_check(ctx);
    // Save the model
    model = yices_get_model(ctx);
    return val;
}

std::string YicesSolver::getModel() {
    assert(model && "Model is empty!");
    // Save the model into a string
    std::stringstream buffer;
    std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
    yices_display_model(model); // printed into buffer
    std::string model_str = buffer.str();
    std::cout.rdbuf(old);
    return model_str;
}

int YicesSolver::getValue(std::string name) { 
    assert(model && "Model is empty!");
    assert(ctx && "Context is null!");
    yices_var_decl d = yices_get_var_decl_from_name(ctx, name.c_str());
    assert(d!=0 && "yices_get_var_decl_from_name");
    long value;
    int error = yices_get_int_value(model, d, &value);
    assert(error!=1 && "cannot extract value from model:\n" &&
                        "v is not a proper declaration or not the" &&
                        "declaration of a numerical variable\n" &&
                        "v has no value assigned in model m (typically," &&
                        " this means that v does not occur in the"
                        " asserted constraints)\n" &&
                        "v has a value that cannot be converted to long," &&
                        " because it is rational or too big\n");
    return value;
}

int YicesSolver::getValue(std::string name, bool &error) {
    assert(ctx && "Context is null!");
    if (model==NULL) {
        error = true;
        return 0;
    }
    yices_var_decl d = yices_get_var_decl_from_name(ctx, name.c_str());
    if (d!=0) {
        long value;
        int e = yices_get_int_value(model, d, &value);
        if (e==1) {
            error = false;
            return value;
        }
    }
    error = true;
    return 0;
}

int YicesSolver::getBoolValue(std::string name) {
    assert(model && "Model is empty!");
    assert(ctx && "Context is null!");
    yices_var_decl d = 
    yices_get_var_decl_from_name(ctx, name.c_str());
    if (d!=0) { 
        lbool val = yices_get_value(model, d);
        return val;
    } else {
        std::cout << ">> " << name << std::endl;
        std::cerr << "error: yices_get_var_decl_from_name\n";
        return -3;
    }
}

int YicesSolver::getValueOrZero(std::string name) {
    assert(ctx && "Context is null!");
    if (model==NULL) {
        return 0;
    }
    yices_var_decl d = yices_get_var_decl_from_name(ctx, name.c_str());
    if (d!=0) { 
        long value;
        int error = yices_get_int_value(model, d, &value);                 
        if (error==1) {
            return value;
        }
    }
    return 0;
}

double YicesSolver::getCostAsDouble() {
    assert(model && "Model is empty!");
    return yices_get_cost_as_double(model);
}

std::vector<ExprPtr> YicesSolver::getUnsatExpressions() {
    assert(model && "Model is empty!");
    std::vector<ExprPtr> unsatExprs;
    std::map<ExprPtr, assertion_id>::iterator it;
    for(it = expr2ids.begin(); it != expr2ids.end(); it++) {
        ExprPtr e = it->first;
        assertion_id i = it->second;
        if(!yices_get_assertion_value(model, i)) {
            unsatExprs.push_back(e);
        }
    }
    return unsatExprs;
}

std::vector<ExprPtr> YicesSolver::getSatExpressions() {
    assert(model && "Model is empty!");
    std::vector<ExprPtr> satExprs;
    std::map<ExprPtr, assertion_id>::iterator it;
    for(it = expr2ids.begin(); it != expr2ids.end(); it++) {
        ExprPtr e = it->first;
        assertion_id i = it->second;
        if(yices_get_assertion_value(model, i)) {
            satExprs.push_back(e);
        }
    }
    return satExprs;
}

void YicesSolver::init() {
    
    // Delete context, model, etc
    clean();
    
    // Create a context
    this->ctx = yices_mk_context();
    
    // Integer 8 bit
    this->int8_ty = yices_parse_type(ctx, "(subrange -128 127)");
    if (this->int8_ty==NULL) {
        const char *msg = yices_get_last_error_message();
        std::cout << msg << std::endl;
        exit(1);
    }
    // Integer 32 bit
    this->int32_ty = yices_parse_type(ctx, "(subrange -2147483648 2147483647)");
    if (this->int32_ty==NULL) {
        const char *msg = yices_get_last_error_message();
        std::cout << msg << std::endl;
        exit(1);
    }
    // Integer 64 bit
    this->int64_ty =
    yices_parse_type(ctx, "(subrange -9223372036854775808 9223372036854775807)");
    if (this->int64_ty==NULL) {
        const char *msg = yices_get_last_error_message();
        std::cout << msg << std::endl;
        exit(1);
    }
    
    // Create uninterpreted function type (INT->INT)
    yices_type domain[1];
    domain[0] = int32_ty;
    this->int32toint32_ty = yices_mk_function_type(this->ctx, domain, 1, int32_ty);
    // Create div and mod operator
    yices_var_decl intdiv_decl = yices_get_var_decl_from_name(ctx, "div");
    this->intdiv_op = yices_mk_var_from_decl(ctx, intdiv_decl);
    yices_var_decl intmod_decl = yices_get_var_decl_from_name(ctx, "mod");
    this->intmod_op = yices_mk_var_from_decl(ctx, intmod_decl);    
    // Inform Yices that only the arithmetic theory is going to be used.
    // yices_set_arith_only (int flag);
    this->model = NULL;
}

void YicesSolver::clean() {
    model = NULL;
    expr2ids.clear();
    expr2yexpr.clear();
    if (ctx!=0) {
        yices_del_context(ctx);
    }
    ctx = 0;
}

yices_expr YicesSolver::makeYicesExpression(ExprPtr e) {
    assert(ctx && "Context is null!");
    switch (e->getOpCode()) {
        case Expression::True:
            return yices_mk_true(ctx);
        case Expression::False:
            return yices_mk_false(ctx);
        case Expression::UInt32Num: {
            UInt32NumExprPtr ne = std::static_pointer_cast<UInt32NumExpression>(e);
            return yices_mk_num(ctx, ne->getValue());
        }
        case Expression::SInt32Num: {
            SInt32NumExprPtr ne = std::static_pointer_cast<SInt32NumExpression>(e);
            return yices_mk_num(ctx, ne->getValue());
        }
        case Expression::BoolVar: {
            BoolVarExprPtr be = std::static_pointer_cast<BoolVarExpression>(e);
            // Check if the varibale has already been created
            yices_var_decl d = 
            yices_get_var_decl_from_name(ctx, be->getName().c_str());
            if (d==0) {
                d = yices_mk_bool_var_decl(ctx, be->getName().c_str());	
            }
            return yices_mk_bool_var_from_decl(ctx, d);
        }
        case Expression::IntVar: {
            IntVarExprPtr ie = std::static_pointer_cast<IntVarExpression>(e);
            // Check if the varibale has already been created
            yices_var_decl d = 
            yices_get_var_decl_from_name(ctx, ie->getName().c_str());
            if (d==0) {
                d = yices_mk_var_decl(ctx, ie->getName().c_str(), int32_ty);
            }
            return yices_mk_var_from_decl(ctx, d);
        }
        case Expression::IntToIntVar: {
            IntToIntVarExprPtr ie =
            std::static_pointer_cast<IntToIntVarExpression>(e);
            // Check if the varibale has already been created
            yices_var_decl d = 
            yices_get_var_decl_from_name(ctx, ie->getName().c_str()); 	
            if (d==0) {
                d = yices_mk_var_decl(ctx, ie->getName().c_str(), int32toint32_ty);
            }
            return yices_mk_var_from_decl(ctx, d);
        }
        case Expression::ToParse: {
            ToParseExprPtr te = std::static_pointer_cast<ToParseExpression>(e);
            return yices_parse_expression(ctx, te->getString().c_str());
        }
        case Expression::Gt: {
            GtExprPtr gt = std::static_pointer_cast<GtExpression>(e);
            yices_expr e1 = makeYicesExpression(gt->getExpr1());
            yices_expr e2 = makeYicesExpression(gt->getExpr2());
            return yices_mk_gt(ctx, e1, e2); 
        }
        case Expression::Ge: {
            GeExprPtr ge = std::static_pointer_cast<GeExpression>(e);
            yices_expr e1 = makeYicesExpression(ge->getExpr1());
            yices_expr e2 = makeYicesExpression(ge->getExpr2());
            return yices_mk_ge(ctx, e1, e2);    
        }
        case Expression::Le: {
            LeExprPtr le = std::static_pointer_cast<LeExpression>(e);
            yices_expr e1 = makeYicesExpression(le->getExpr1());
            yices_expr e2 = makeYicesExpression(le->getExpr2());
            return yices_mk_le(ctx, e1, e2);    
        }
        case Expression::Lt: {
            LtExprPtr lt = std::static_pointer_cast<LtExpression>(e);
            yices_expr e1 = makeYicesExpression(lt->getExpr1());
            yices_expr e2 = makeYicesExpression(lt->getExpr2());
            return yices_mk_lt(ctx, e1, e2);    
        }
        case Expression::Diseq: {
            DiseqExprPtr de = std::static_pointer_cast<DiseqExpression>(e);
            yices_expr e1 = makeYicesExpression(de->getExpr1());
            yices_expr e2 = makeYicesExpression(de->getExpr2());
            return yices_mk_diseq(ctx, e1, e2);    
        }
        case Expression::Eq: {
            EqExprPtr ee = std::static_pointer_cast<EqExpression>(e);
            yices_expr e1 = makeYicesExpression(ee->getExpr1());
            yices_expr e2 = makeYicesExpression(ee->getExpr2());
            return yices_mk_eq(ctx, e1, e2);    
        }
        case Expression::Not: {
            NotExprPtr ne = std::static_pointer_cast<NotExpression>(e);
            yices_expr e1 = makeYicesExpression(ne->get());
            return yices_mk_not(ctx, e1);
        }
        case Expression::And: {
            AndExprPtr ae = std::static_pointer_cast<AndExpression>(e);
            std::vector<ExprPtr> es = ae->getExprs();
            unsigned n = es.size();
            assert(n>0 && "empty AND expression!");
            if (n==1) {
                return makeYicesExpression(es.back());
            }
            yices_expr args[n];
            for(unsigned i=0; i<n; i++) {
                args[i] = makeYicesExpression(es[i]); 
            }   
            return yices_mk_and(ctx, args, n);   
        }
        case Expression::Or: {
            OrExprPtr oe = std::static_pointer_cast<OrExpression>(e);
            std::vector<ExprPtr> es = oe->getExprs();
            unsigned n = es.size();
            assert(n>0 && "empty OR expression!");
            if (n==1) {
                return makeYicesExpression(es.back());
            }
            yices_expr args[n];
            for(unsigned i=0; i<n; i++) {
                args[i] = makeYicesExpression(es[i]); 
            }   
            return yices_mk_or(ctx, args, n);   
        }
        case Expression::Xor: {
            XorExprPtr xe = std::static_pointer_cast<XorExpression>(e);
            std::vector<ExprPtr> es = xe->getExprs();
            unsigned n = es.size();
            assert(n>0 && "empty XOR expression!");
            assert(n<=3 && "unsupported XOR expression (too many args)!");
            if (n==1) {
                return makeYicesExpression(es.back());
            }
            else if (n==2) {
                ExprPtr orExpr = Expression::mkOr(es[0], es[1]);
                ExprPtr andExpr = Expression::mkAnd(es[0], es[1]);
                ExprPtr notExpr = Expression::mkNot(andExpr);
                ExprPtr xorExpr = Expression::mkAnd(orExpr, notExpr);
                return makeYicesExpression(xorExpr);
            } else if (n==3) {
                ExprPtr e1= es[0];
                ExprPtr e2= es[1];
                ExprPtr e3= es[2];
                ExprPtr not_e1= Expression::mkNot(es[0]);
                ExprPtr not_e2= Expression::mkNot(es[1]);
                ExprPtr not_e3= Expression::mkNot(es[2]);
                // (and e1' e2' e3)
                std::vector<ExprPtr> v1;
                v1.push_back(not_e1);
                v1.push_back(not_e2);
                v1.push_back(e3);
                ExprPtr and1 = Expression::mkAnd(v1);
                // (and e1' e2 e3')
                std::vector<ExprPtr> v2;
                v2.push_back(not_e1);
                v2.push_back(e2);
                v2.push_back(not_e3);
                ExprPtr and2 = Expression::mkAnd(v2);
                // (and e1 e2' e3')
                std::vector<ExprPtr> v3;
                v3.push_back(e1);
                v3.push_back(not_e2);
                v3.push_back(not_e3);
                ExprPtr and3 = Expression::mkAnd(v3);
                // (and e1 e2 e3)
                std::vector<ExprPtr> v4;
                v4.push_back(e1);
                v4.push_back(e2);
                v4.push_back(e3);
                ExprPtr and4 = Expression::mkAnd(v4);
                // (or and1 and2 and3 and4)
                std::vector<ExprPtr> v5;
                v5.push_back(and1);
                v5.push_back(and2);
                v5.push_back(and3);
                v5.push_back(and4);
                ExprPtr xorExpr = Expression::mkOr(v5);
                return makeYicesExpression(xorExpr);
            }
        }
        case Expression::Sum: {
            SumExprPtr se = std::static_pointer_cast<SumExpression>(e);
            std::vector<ExprPtr> es = se->getExprs();
            unsigned n = es.size();
            assert(n>0 && "empty SUM expression!");
            if (n==1) {
                return makeYicesExpression(es.back());
            }
            yices_expr args[n];
            for(unsigned i=0; i<n; i++) {
                args[i] = makeYicesExpression(es[i]); 
            }   
            return yices_mk_sum(ctx, args, n);   
        }   
        case Expression::Sub:  {
            SubExprPtr se = std::static_pointer_cast<SubExpression>(e);
            std::vector<ExprPtr> es = se->getExprs();
            unsigned n = es.size();
            assert(n>0 && "empty SUB expression!");
            if (n==1) {
                return makeYicesExpression(es.back());
            }
            yices_expr args[n];
            for(unsigned i=0; i<n; i++) {
                args[i] = makeYicesExpression(es[i]); 
            }   
            return yices_mk_sub(ctx, args, n);   
        }   
        case Expression::Mul: {
            MulExprPtr me = std::static_pointer_cast<MulExpression>(e);
            std::vector<ExprPtr> es = me->getExprs();
            unsigned n = es.size();
            assert(n>0 && "empty MUL expression!");
            if (n==1) {
                return makeYicesExpression(es.back());
            }
            yices_expr args[n];
            for(unsigned i=0; i<n; i++) {
                args[i] = makeYicesExpression(es[i]); 
            }   
            return yices_mk_mul(ctx, args, n);   
        }               
        case Expression::Ite: {
            IteExprPtr ie = std::static_pointer_cast<IteExpression>(e);
            yices_expr e1 = makeYicesExpression(ie->getExpr1());
            yices_expr e2 = makeYicesExpression(ie->getExpr2());
            yices_expr e3 = makeYicesExpression(ie->getExpr3());
            return yices_mk_ite(ctx, e1, e2, e3);
        }
        case Expression::App: {
            AppExprPtr ae = std::static_pointer_cast<AppExpression>(e);
            yices_expr e1 = makeYicesExpression(ae->getExpr1());
            yices_expr args[1];
            args[0] = makeYicesExpression(ae->getExpr2());
            return yices_mk_app(ctx, e1, args, 1); 
        }
        case Expression::Update: {
            UpdateExprPtr ue = std::static_pointer_cast<UpdateExpression>(e);
            yices_expr e1 = makeYicesExpression(ue->getExpr1());
            yices_expr e2[1];
            e2[0] = makeYicesExpression(ue->getExpr2());
            yices_expr e3 = makeYicesExpression(ue->getExpr3());
            return yices_mk_function_update(ctx, e1, e2, 1, e3);
        }
        case Expression::Div: {
            DivExprPtr de = std::static_pointer_cast<DivExpression>(e);
            yices_expr args[2];
            args[0] = makeYicesExpression(de->getExpr1());
            args[1] = makeYicesExpression(de->getExpr2());
            return yices_mk_app(ctx, intdiv_op, args, 2);
        }
        case Expression::Mod: {
            ModExprPtr de = std::static_pointer_cast<ModExpression>(e);
            yices_expr args[2];
            args[0] = makeYicesExpression(de->getExpr1());
            args[1] = makeYicesExpression(de->getExpr2());
            return yices_mk_app(ctx, intmod_op, args, 2);
        }
        default:
            llvm_unreachable("Illegal expression opcode!");
            break;
    }
}
