/* 
 * File:   ReducingGenerator.hpp
 * Author: Peter G. Jensen
 *
 * Created on December 14, 2017, 8:44 PM
 */

#ifndef REDUCINGGENERATOR_HPP
#define REDUCINGGENERATOR_HPP

#include "Generator.h"
#include "DiscreteVerification/DataStructures/light_deque.h"
#include "InterestingVisitor.h"

namespace VerifyTAPN { namespace DiscreteVerification {

    class ReducingGenerator : public Generator {
    protected:
        InterestingVisitor interesting;
        std::vector<bool> _enabled, _stubborn;
        size_t ecnt = 0;
        bool can_reduce = false;
        light_deque<uint32_t> _unprocessed, _ordering;

        bool preSetOf(size_t i);

        bool postSetOf(size_t i, bool check_age, const TAPN::TimeInterval &interval = TAPN::TimeInterval());

        bool inhibPostSetOf(size_t i);

        void zero_time_set(int32_t max_age, const TAPN::TimedPlace *, const TAPN::TimedTransition *);

        bool ample_set(const TAPN::TimedPlace *inv_place, const TAPN::TimedTransition *trans);

        bool compute_closure(bool added_zt);

    public:
        ReducingGenerator(TAPN::TimedArcPetriNet &tapn, AST::Query *query)
                : Generator(tapn, query), interesting(tapn), _enabled(tapn.getTransitions().size()),
                  _stubborn(tapn.getTransitions().size()) {};

        void from_marking(NonStrictMarkingBase *parent, Mode mode = ALL, bool urgent = false) override;

        NonStrictMarkingBase *next(bool do_delay = true) override;
    };

} }

#endif /* REDUCINGGENERATOR_HPP */

