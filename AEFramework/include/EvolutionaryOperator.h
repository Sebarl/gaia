#ifndef EVOLUTIONARY_OPERATOR_H
#define EVOLUTIONARY_OPERATOR_H

#include <vector>
#include "Random.h"

template <class T>
class EvolutionaryOperator
{

    /**
* <p>Apply the operation to each entry in the list of selected
     * candidates.  It is important to note that this method operates on
     * the list of candidates returned by the selection strategy and not
     * on the current population.  Each entry in the list (not each
     * individual - the list may contain the same individual more than
     * once) must be operated on exactly once.</p>
     *
     * <p>Implementing classes should not assume any particular ordering
     * (or lack of ordering) for the selection.  If ordering or
     * shuffling is required, it should be performed by the implementing
     * class.  The implementation should not re-order the list provided
     * but instead should make a copy of the list and re-order that.
     * The ordering of the selection should be totally irrelevant for
     * operators that process each candidate in isolation, such as mutation.
     * It should only be an issue for operators, such as cross-over, that
     * deal with multiple candidates in a single operation.</p>
     * <p><strong>The operator must not modify any of the candidates passed
     * in</strong>.  Instead it should return a list that contains evolved
     * copies of those candidates (umodified candidates can be included in
     * the results without having to be copied).</p>
     * @param selectedCandidates The individuals to evolve.
     * @param rng A source of randomness for stochastic operators (most
     * operators will be stochastic).
     * @return The evolved individuals.
     */
    public:
        EvolutionaryOperator(){}
        virtual ~EvolutionaryOperator(){}
        virtual std::vector<T*> apply(std::vector<T*> selectedCandidates, Random* rng) =0;
};

#endif
