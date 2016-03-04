/**
 * \file YicesSolver.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   12 January 2015
 */

#ifndef _YICESSOLVER_H
#define _YICESSOLVER_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include "yices_c.h"

#include "Expression.h"
#include "Formula.h"

/** 
 * \class YicesSolver
 *
 * \brief Warper class for the Yices-1 SMT solver C API.
 *
 * This warper class provides access to the
 * basic Yices 1 functionalities.
 */
class YicesSolver {
        
private:
    /**
     * Logical context
     *
     * A context stores a collection of declarations and assertions.
     */
    yices_context ctx;
    /**
     * Integer 8, 32, 64 bit types
     */
    yices_type int8_ty, int32_ty, int64_ty;
    /**
     * Uninterpreted function type for array: int32->int32
     */
    yices_type int32toint32_ty;
    /**
     * Integer division operator
     */
    yices_expr intdiv_op;
    /**
     * Integer modulo operator
     */
    yices_expr intmod_op;
    /**
     * Model
     *
     * A model assigns constant values to variables defined in
     * a context. The context must be known to be consistent for a model
     * to be available. The model is constructed by calling check (or its
     * relatives) then getModel.
     */
    yices_model model;
    /**
     * Map of SNIPER expressions to their yices IDs.
     */
    std::map<ExprPtr, assertion_id> expr2ids;
    /**
     * Map of SNIPER expressions to their yices expressions.
     */
    std::map<ExprPtr, yices_expr> expr2yexpr;
    
public:
    /**
     * Default constructor.
     */
    YicesSolver() : ctx(0), model(NULL) { }
    /**
     * Destructor.
     */
    ~YicesSolver() { }
    
    /**
     * \brief Initialize the solver.
     *
     * This function creates the logical context, types and operators.
     * This function should be call before calling check or maxSat.
     */
    void init();
    
    /**
     * \brief Assert an expression in the logical context.
     *
     * \param e An expression.
     */
    void addToContext(ExprPtr e);
    
    /**
     * \brief Assert a formula in the logical context.
     *
     * \param e A formula.
     */
    void addToContext(Formula *f);
    
    /**
     * \brief Check if the logical context is satisfiable.
     *
     * This function ignores the soft/hard property associated with the
     * expressions. If the context is satisfiable, then getModel() can
     * be used to obtain a model.
     *
     * \return l_true if the context is satisfiable, or
     *         l_false if the context is unsatisfiable, or
     *         l_undef if it was not possible to decide
     *         due to an incompletness.
     */
    int check();
    
    /**
     * \brief Check if the logical context is satisfiable.
     *
     * This function ignores the soft/hard property
     * associated with the expressions.
     * If the context is satisfiable, then getModel()
     * can be used to obtain a model.
     *
     * \param A formula
     * \return l_true if the context is satisfiable, or
     *         l_false if the context is unsatisfiable, or
     *         l_undef if it was not possible to decide
     *         due to an incompletness.
     */
     int check(Formula *f);
    
    /**
     * \brief Compute the maximal satisfying assignment for
     *        the asserted soft constraints.
     *
     * If a maximal satisfying assignment was found,
     * then getModel() can be used to obtain a model.
     *
     * \return l_true if a maximal satisfying assignment was found, or
     *         l_false if the context is unsatisfiable (this may
     *         happen if the context has hard constraints), or
     *         l_undef if it was not possible to decide because
     *         of an incompleteness.
     */
    int         maxSat(Formula *f);
    
    /**
     * \brief Compute the maximal satisfying assignment for
     *        the asserted soft constraints.
     *
     * If a maximal satisfying assignment was found,
     * then getModel() can be used to obtain a model.
     *
     * \param f A formula
     * \return l_true if a maximal satisfying assignment was found, or
     *         l_false if the context is unsatisfiable (this may
     *         happen if the context has hard constraints), or
     *         l_undef if it was not possible to decide because
     *         of an incompleteness.
     */
    int maxSat();
    
    /**
     * \brief Return a model for a satisfiable logical context.
     *
     * This should be only called if check() or maxSat()
     * returned l_true or l_undef.
     * Calls to functions that modify the context invalidate the model.
     *
     * \return an empty string if a model is not available, or
     *         a string representating a model.
     */
    std::string getModel();
    
    /**
     * \brief Return the cost of the current model, converted
     *        to a double-precision floating point number.
     *
     * This should be only called when a valid model is available.
     *
     * \return a cost
     */
    double getCostAsDouble();
    
    /**
     * \brief Get the integer value assigned to variable v in the current model.
     *
     * This should be only called when a valid model is available.
     * In case of errors, this function does not return.
     *
     * \param name The name of an integer variable present in the expressions.
     * \return the value of the variable.
     */
    int getValue(std::string name);
    
    /**
     * \brief Get the integer value assigned to variable v in the current model.
     *
     * This should be only called when a valid model is available.
     * In case of errors, this function returns anyway.
     *
     * \param name The name of an integer variable present in the expressions.
     * \param error A reference to a boolean variable to check for errors after
     *        the function has returned. The variable error is true if an error
     *        occured, false otherwise.
     * \return the value of the variable, or
     *         0 if no variable is named name, or
     *           if the variable is not a proper declaration or
     *           not the declaration of a numerical variable, or
     *           if the variable has no value assigned in current model
     *           (typically, this means that the variable does not
     *           occur in the asserted constraints), or
     *           if the variable has a value that cannot be converted to long,
     *           because it is rational or too big.
     */
    int getValue(std::string name, bool &error);
    
    /**
     * \brief Get the integer value assigned to variable v in the current model.
     *
     * This should be only called when a valid model is available.
     * In case of errors, this function returns anyway.
     *
     * \param name The name of an integer variable present in the expressions.
     * \return the value of the variable, or
     *         0 if no variable is named name, or
     *           if the variable is not a proper declaration or
     *           not the declaration of a numerical variable, or
     *           if the variable has no value assigned in current model
     *           (typically, this means that the variable does not
     *           occur in the asserted constraints), or
     *           if the variable has a value that cannot be converted to long,
     *           because it is rational or too big.
     */
    int getValueOrZero(std::string name);
    
    /**
     * \brief Get the boolean value assigned to variable v in the current model.
     *
     * This should be only called when a valid model is available.
     * In case of errors, this function returns anyway.
     *
     * \param name The name of an boolean variable present in the expressions.
     * \return the value of the variable, or
     *         -3 if the variable was not declared.
     */
    int getBoolValue(std::string name);
    
    /**
     * \brief Return the expressions that are not satisfied in the current model.
     *
     * This should be only called when a valid model is available.
     *
     * \return a vector of expressions
     */
    std::vector<ExprPtr> getUnsatExpressions();
    
    /**
     * \brief Return the expressions that are satisfied in the current model.
     *
     * This should be only called when a valid model is available.
     *
     * \return a vector of expressions
     */
    std::vector<ExprPtr> getSatExpressions();
    
    /**
     * \brief Create a backtracking point.
     *
     * The logical context can be viewed as a stack of contexts.
     * The scope level is the number of elements on this stack.
     * The stack of contexts is simulated using trail (undo) stacks.
     */
    void push();
    
    /**
     * \brief Backtrack.
     *
     * Restores the context from the top of the stack, and pops it off
     * the stack. Any changes to the logical context (by addToContext())
     * between the matching push() and pop() operators are flushed,
     * and the context is completely restored to what it was right
     * before the push().
     */
    void pop();
    
    /**
     * \brief Clean the solver state.
     *
     * This function deletes the logical context and the current model.
     */
    void clean();
    
private:
    /**
     * \brief Return a yices expression representing the SNIPER expression
     *        given as argument given arguments.
     *
     * This function deletes the logical context and the current model.
     *
     * \param e A SNIPER expression
     * \return a yices expression
     */
    yices_expr makeYicesExpression(ExprPtr e);
      
}; 

#endif // _YICESSOLVER_H
