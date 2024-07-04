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

            SMCRunGenerator(TAPN::TimedArcPetriNet &tapn, SMCSemantics semantics = Weak)
            : _tapn(tapn)
            , _defaultTransitionIntervals(tapn.getTransitions().size()) 
            , _transitionsStatistics(tapn.getTransitions().size(), 0)
            , _semantics(semantics)
            {};

            virtual void prepare(NonStrictMarkingBase *parent);
            virtual RealMarking* next();
            virtual void reset();

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

            void setSemantics(SMCSemantics semantics);
            SMCSemantics getSemantics() const;
            
        protected:
        
            Util::interval<double> remainingForToken(const Util::interval<double>& arcInterval, const RealToken& t);
            
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
            SMCSemantics _semantics = Weak;
            
        };

    }
}

#endif /* SMCRUNGENERATOR_H */

