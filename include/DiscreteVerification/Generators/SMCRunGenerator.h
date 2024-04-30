/* 
 * File:   RandomRunGenerator.h
 * Author: Tanguy Dubois
 * 
 * Created on 11 April 2024, 10.13
 */

#ifndef SMCRUNGENERATOR_H
#define SMCRUNGENERATOR_H
#include "DiscreteVerification/Generators/Generator.h"
#include "DiscreteVerification/Util/IntervalOps.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class SMCRunGenerator {

        public:

            SMCRunGenerator(TAPN::TimedArcPetriNet &tapn, float defaultRate = 0.1)
            : _tapn(tapn)
            , _defaultTransitionIntervals(tapn.getTransitions().size()) 
            , _transitionsStatistics(tapn.getTransitions().size(), 0)
            , _defaultRate(defaultRate)
            {};

            virtual void prepare(NonStrictMarkingBase *parent);
            virtual NonStrictMarkingBase* next();
            virtual void reset();

            void refreshTransitionsIntervals();

            std::vector<Util::interval> transitionFiringDates(TimedTransition* transi);
            std::vector<Util::interval> arcFiringDates(TimeInterval time_interval, uint32_t weight, TokenList& tokens);
            
            std::pair<TimedTransition*, int> getWinnerTransitionAndDelay();

            NonStrictMarkingBase* fire(TimedTransition* transi);

            bool reachedEnd() const;

            int getRunDelay() const;
            int getRunSteps() const;

            void printTransitionStatistics(std::ostream &out) const;
            
        protected:
        
            Util::interval remainingForToken(const Util::interval& arcInterval, const Token& t);

            bool _maximal = false;
            TimedArcPetriNet& _tapn;
            std::vector<std::vector<Util::interval>> _defaultTransitionIntervals; // Type not pretty, but need disjoint intervals
            std::vector<std::vector<Util::interval>> _transitionIntervals; // Type not pretty, but need disjoint intervals
            std::vector<uint32_t> _transitionsStatistics;
            NonStrictMarkingBase* _origin;
            NonStrictMarkingBase* _parent;
            int _lastDelay = 0;
            std::vector<int> _modifiedPlaces;
            int _totalTime = 0;
            int _totalSteps = 0;
            int _max_delay = 0;
            float _defaultRate = 0.1;
            
        };

    }
}

#endif /* SMCRUNGENERATOR_H */

