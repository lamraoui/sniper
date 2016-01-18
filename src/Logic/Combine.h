/**
 * \file Combine.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   15 January 2015
 */

#ifndef _COMBINE_H
#define _COMBINE_H

#include <vector>

#include "Utils/HittingSet.h"
#include "Logic/Formula.h"
#include "Options.h"

/**
 * \class Combine
 *
 * \brief Diagnoses (MCSes) combination methods.
 *
 * This class provides methods to combine sets of diagnoses. 
 * Diagnoses are minimal correction subsets (MCSes).
 * Each of these MCSes is a root cause candidate for one failing execution,
 * which is triggered by the error-inducing inputs associated to this MCS.
 * The problem of combining diagnoses is to generate sets of root causes
 * that can be used to potentially fix all the failing executions induced by 
 * all the provided error-inducing inputs.  Such a set is called a 
 * "complete diagnosis". With a complete diagnosis, a user can pottentially 
 * fix all the faults in the program.
 *
 * This class implements three combination techniques: a flattening-based 
 * combination, a hitting-set-based combination and a pair-wise-based 
 * combination.
 *
 */
class Combine {
    
public:
    /**
     * Combination Methods
     */
    enum Method {
        MHS, /*!< Minimal hitting-set-based combination */
        PWU, /*!< Pair-wise-union-based combination */
        FLA, /*!< Flattening-based combination */
        NONE /*!< No combination */
    };
    
public:
    /**
     * \brief Minimal hitting-set-based combination method.
     *
     * Suppose that we have a set \a M of MCSes, then if we compute the
     * minimal hitting set of \a M we obtain a set of sets H, which contains
     * complete diagnoses because H is a set of elements that covers every 
     * set in \a M by having at least one element in common with it.
     *
     * \param M A vector of MCSes.
     * \return a set of complete diagnosis.
     */
    static SetOfFormulasPtr combineByMHS(std::vector<SetOfFormulasPtr> M);
    
    /**
     * \brief Pair-wise-union-based combination method.
     *
     * This combination technique is based on a pair-wise union. 
     * This method ensures that if no fault is missed by the diagnoses 
     * generation algorithm of SNIPER, then no fault is missed after the 
     * combination.
     *
     * \param M A vector of MCSes.
     * \return a set of complete diagnosis.
     */
    static SetOfFormulasPtr combineByPWU(std::vector<SetOfFormulasPtr> M);
    
    /**
     * \brief Flattening-based combination method.
     *
     * The flattening-based combination technique is a simple method 
     * to combine diagnosis. This method puts all elements of \a M in 
     * a single set. This method is not computationally costly as compared 
     * with other methods, however, it produces little information regarding
     * the relations between the root causes. This can be a problem when 
     * dealing with  multi-fault programs because the engineer does not 
     * know if she has to take into account one or more than one root 
     * cause of this set to correct the faulty program.
     *
     * \param M A vector of MCSes.
     * \return a set of clauses, which are root causes.
     */
    static SetOfFormulasPtr combineByFlatten(std::vector<SetOfFormulasPtr> M);
    
private:
    /**
     * \brief No combination method.
     *
     *
     * \param M A vector of MCSes
     * \param Diag An MCSes (output).
     */
    static void pairwiseUnion(std::vector<SetOfFormulasPtr> M,
                            SetOfFormulasPtr Diag);

};

#endif // _COMBINE_H
