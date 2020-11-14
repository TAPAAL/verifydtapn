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
            bool irreducable() const { return !_can_reduce; }
        protected:
            const TimedArcPetriNet& _tapn;
            AST::Query* _query;
            light_deque<uint32_t> _enabled_set;
        private:
            NextEnabledGenerator _gen_enabled;
            InterestingVisitor _interesting;
            std::vector<bool> _enabled, _stubborn;
            light_deque<uint32_t> _unprocessed;
            NonStrictMarkingBase* _parent;
            bool _urgent_enabled = false;
            bool _can_reduce = false;
            
        protected:
            void _prepare(NonStrictMarkingBase *parent, std::function<void(const TimedTransition*)>&& enabled_monitor, std::function<bool(void)>&& extra_conditions);
            bool is_singular();
            void reset(NonStrictMarkingBase*);
            void clear_stubborn();
            const TimedTransition* compute_enabled(std::function<void(const TimedTransition*)>&& monitor = [](auto){});
            std::pair<const TimedPlace*,uint32_t> invariant_place(const TimedTransition* t = nullptr);
            
            bool preset_of(size_t i);

            bool postset_of(size_t i, bool check_age, const TAPN::TimeInterval &interval = TAPN::TimeInterval());

            bool inhib_postset_of(size_t i);

            void zero_time_set(int32_t max_age, const TAPN::TimedPlace *, const TAPN::TimedTransition *);

            bool ample_set(const TAPN::TimedPlace *inv_place, const TAPN::TimedTransition *trans);

            bool compute_closure(bool added_zt);
        };
    }
}
#endif /* STUBBORNSET_H */

