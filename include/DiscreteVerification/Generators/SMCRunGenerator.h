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
#include "Core/Query/SMCQuery.hpp"
#include "DiscreteVerification/DataStructures/RealMarking.hpp"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        class SMCRunGenerator {

        public:

            SMCRunGenerator(TAPN::TimedArcPetriNet &tapn)
            : _tapn(tapn)
            , _defaultTransitionIntervals(tapn.getTransitions().size()) 
            , _transitionsStatistics(tapn.getTransitions().size(), 0)
            {
                std::random_device rd;
                _rng = std::ranlux48(rd());
            };

            virtual void prepare(RealMarking *parent);
            virtual RealMarking* next();
            virtual void reset();

            SMCRunGenerator copy() const;

            RealMarking* getMarking() { return _parent; }

            void refreshTransitionsIntervals();

            std::vector<Util::interval<double>> transitionFiringDates(TimedTransition* transi);
            std::vector<Util::interval<double>> arcFiringDates(TimeInterval time_interval, uint32_t weight, RealTokenList& tokens);
            
            std::pair<TimedTransition*, double> getWinnerTransitionAndDelay();

            RealMarking* fire(TimedTransition* transi);

            bool reachedEnd() const;

            double getRunDelay() const;
            int getRunSteps() const;

            void printTransitionStatistics(std::ostream &out) const;
            
        protected:
        
            Util::interval<double> remainingForToken(const Util::interval<double>& arcInterval, const RealToken& t);
            TimedTransition* chooseWeightedWinner(const std::vector<size_t>& winner_indexs);
            
            bool _maximal = false;
            TimedArcPetriNet& _tapn;
            std::vector<std::vector<Util::interval<double>>> _defaultTransitionIntervals; // Type not pretty, but need disjoint intervals
            std::vector<std::vector<Util::interval<double>>> _transitionIntervals; // Type not pretty, but need disjoint intervals
            std::vector<double> _dates_sampled;
            std::vector<uint32_t> _transitionsStatistics;
            RealMarking* _origin;
            RealMarking* _parent;
            double _lastDelay = 0;
            std::vector<int> _modifiedPlaces;
            double _totalTime = 0;
            int _totalSteps = 0;
            double _max_delay = 0;
            double _originMaxDelay = 0;

            std::ranlux48 _rng;

            std::vector<RealMarking*> _trace;
            
        };

    }
}

#endif /* SMCRUNGENERATOR_H */

