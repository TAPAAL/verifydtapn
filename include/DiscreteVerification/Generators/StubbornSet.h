/* 
 * File:   StubbornSet.h
 * Author: Peter G. Jensen
 *
 * Created on 13 November 2020, 12.20
 */

#ifndef STUBBORNSET_H
#define STUBBORNSET_H

#include "DiscreteVerification/DataStructures/light_deque.h"

#include "InterestingVisitor.h"
#include "NextEnabledGenerator.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        using namespace TAPN;
        class StubbornSet {
        public:
            StubbornSet(const TimedArcPetriNet& tapn, AST::Query* query);
            void prepare(NonStrictMarkingBase *parent);
            const TimedTransition* pop_next();
            bool urgent() const { return _urgent_enabled; }
            bool empty() const;
            bool irreducable() const { return !_can_reduce; }
        private:
            const TimedArcPetriNet& _tapn;
            AST::Query* _query;
            NextEnabledGenerator _gen_enabled;
            InterestingVisitor _interesting;
            std::vector<bool> _enabled, _stubborn;
            size_t _ecnt = 0;
            bool _can_reduce = false;
            light_deque<uint32_t> _unprocessed, _ordering;
            NonStrictMarkingBase* _parent;
            bool _urgent_enabled;
            
            bool preSetOf(size_t i);

            bool postSetOf(size_t i, bool check_age, const TAPN::TimeInterval &interval = TAPN::TimeInterval());

            bool inhibPostSetOf(size_t i);

            void zero_time_set(int32_t max_age, const TAPN::TimedPlace *, const TAPN::TimedTransition *);

            bool ample_set(const TAPN::TimedPlace *inv_place, const TAPN::TimedTransition *trans);

            bool compute_closure(bool added_zt);
        };
    }
}
#endif /* STUBBORNSET_H */

