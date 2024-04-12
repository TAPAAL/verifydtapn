/* 
 * File:   RandomRunGenerator.h
 * Author: Tanguy Dubois
 * 
 * Created on 11 April 2024, 10.13
 */

#ifndef RANDOMRUNGENERATOR_H
#define RANDOMRUNGENERATOR_H
#include "DiscreteVerification/Generators/Generator.h"
#include "DiscreteVerification/Util/IntervalOps.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class RandomRunGenerator : protected Generator {
        protected:
        public:
            RandomRunGenerator(TAPN::TimedArcPetriNet &tapn, AST::Query *query)
            : Generator(tapn, query), _transitionIntervals(tapn.getTransitions().size()) {
            };
            virtual void prepare(NonStrictMarkingBase *parent) override;
            virtual NonStrictMarkingBase* next(bool do_delay = true) override;
            virtual void reset();

            std::vector<Util::interval> transitionFiringDates(TimedTransition* transi);
            std::vector<Util::interval> arcFiringDates(TimeInterval time_interval, uint32_t weight, TokenList& tokens);
            
            std::pair<TimedTransition*, int> getWinnerTransitionAndDelay();
            
        protected:
            Util::interval remainingForToken(const Util::interval& arcInterval, const Token& t);

            NonStrictMarkingBase *_next(bool do_delay, std::function<bool(const TimedTransition*)> filter) override;
            bool _maximal = false;
            std::vector<std::vector<Util::interval>> _transitionIntervals; // Type not pretty, but need disjoint intervals
            std::vector<std::vector<Util::interval>> _defaultTransitionIntervals; // Type not pretty, but need disjoint intervals
            NonStrictMarkingBase* _origin;
        };

    }
}

#endif /* RANDOMRUNGENERATOR_H */

