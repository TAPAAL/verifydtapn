/* 
 * File:   Generator.h
 * Author: Peter G. Jensen
 *
 * Created on 15 September 2015, 17:25
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "Core/TAPN/TAPN.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"

#include "NextEnabledGenerator.h"

#include <vector>
#include <algorithm>

namespace VerifyTAPN { namespace DiscreteVerification {
    class Generator {
        typedef std::vector<const TAPN::TimedTransition *> transitions_t;
    protected:
        const TAPN::TimedArcPetriNet &_tapn;
        NextEnabledGenerator _transition_iterator; 
        NonStrictMarkingBase *_parent{};
        size_t _num_children{};
        const TAPN::TimedTransition *_current{};
        std::vector<size_t> _permutation;
        std::vector<size_t> _base_permutation;
        bool _done{};
        bool _seen_urgent{};
        const TAPN::TimedTransition *_trans = nullptr;
        AST::Query *_query;
        std::vector<uint32_t> _transitionStatistics;
        const TAPN::TimedTransition *_last_fired = nullptr;
    public:
        Generator(const TAPN::TimedArcPetriNet &tapn, AST::Query *query);

        virtual void prepare(NonStrictMarkingBase *parent);

        virtual NonStrictMarkingBase *next(bool do_delay = true);
        
        void printTransitionStatistics(std::ostream &out) const;
        
        const TAPN::TimedTransition *last_fired() const { return _last_fired; }
        
        size_t children();
        
        bool urgent() { return _seen_urgent; };
        
        bool only_transition(const TAPN::TimedTransition *trans);
        
        void reset();

    protected:
        virtual NonStrictMarkingBase *_next(bool do_delay, std::function<bool(const TimedTransition*)> filter);
        NonStrictMarkingBase *from_delay();
        NonStrictMarkingBase* fire_no_input(const TimedTransition* );
        NonStrictMarkingBase* fire(const TimedTransition* t);

        NonStrictMarkingBase *next_transition_permutation();
    };
} }
#endif    /* GENERATOR_H */

