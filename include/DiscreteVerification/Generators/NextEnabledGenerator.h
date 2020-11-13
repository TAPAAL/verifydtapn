/* 
 * File:   NextEnabledGenerator.h
 * Author: Peter G. Jensen
 *
 * Created on 13 November 2020, 10.15
 */

#ifndef NEXTENABLEDGENERATOR_H
#define NEXTENABLEDGENERATOR_H

#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"

#include "Core/TAPN/TAPN.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {
        using namespace TAPN;
        class NextEnabledGenerator {
        public:

            NextEnabledGenerator(const TimedArcPetriNet& tapn);
            
            void prepare(const NonStrictMarkingBase *parent);
            std::pair<const TimedTransition*, bool> next_transition(std::function<bool(const TimedTransition*)> filter = [](const TimedTransition*) { return true;  }, 
                                                                    std::vector<size_t>* permutations = nullptr);
            size_t max_tokens() const { return _max_tokens; }
        protected:
            const InhibitorArc *inhibited(const TimedTransition *trans) const;
            bool is_enabled(const TimedTransition *trans, std::vector<size_t> *permutations) const;
            const TimedPlace* compute_missing(const TimedTransition *trans, std::vector<size_t> *permutations) const;
        private:
            typedef std::vector<const TAPN::TimedTransition *> transitions_t;
            const TimedArcPetriNet& _tapn;
            const NonStrictMarkingBase *_parent{};
            transitions_t _allways_enabled;
            std::vector<transitions_t> _place_transition;
            bool _did_noinput{};
            size_t _place{};
            size_t _transition{};
            size_t _max_tokens;
        };
    }
}

#endif /* NEXTENABLEDGENERATOR_H */

