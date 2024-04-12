/* 
 * File:   RandomRunGenerator.cpp
 * Author: Tanguy Dubois
 * 
 * Created on 11 April 2024, 10.13
 */

#include "DiscreteVerification/Generators/RandomRunGenerator.h"

#include <numeric>
#include <random>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using Util::interval;

        void RandomRunGenerator::prepare(NonStrictMarkingBase *parent) {
            _parent = parent;
            _origin = parent;
            PlaceList& places = parent->getPlaceList();
            std::vector<bool> transitionSeen(_transitionIntervals.size(), false);
            int max_delay = std::numeric_limits<int>::max();
            for(auto &pit : places) {
                if(pit.place->getInvariant().getBound() < max_delay) {
                    max_delay = pit.place->getInvariant().getBound();
                }
                for(auto arc : pit.place->getInputArcs()) {
                    TimedTransition &transi = arc->getOutputTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval> firingDates = transitionFiringDates(&transi);
                    _transitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
            }
            std::vector<interval> invInterval = { interval(0, max_delay) };
            for(auto iter = _transitionIntervals.begin() ; iter != _transitionIntervals.end() ; iter++) {
                if(iter->empty()) continue;
                *iter = Util::setIntersection(*iter, invInterval);
            }
            _defaultTransitionIntervals = _transitionIntervals;
        }

        NonStrictMarkingBase* RandomRunGenerator::next(bool do_delay = true) {
            auto transi_delay = getWinnerTransitionAndDelay();
            TimedTransition* transi = transi_delay.first;
            int delay = transi_delay.second;

            NonStrictMarkingBase* child(_parent);
            _parent->incrementAge(delay);
            child = fire(transi);

            // Delta to intervals, so we don't have to compute some of them next
            for(auto iter = _transitionIntervals.begin() ; iter != _transitionIntervals.end() ; iter++) {
                Util::setDelta(*iter, -delay);
            }
            
            return child;
        }

        std::pair<TimedTransition*, int> RandomRunGenerator::getWinnerTransitionAndDelay() {
            size_t winner_index = 0;
            size_t transi_index = 0;
            int date_min = std::numeric_limits<int>::max();
            std::random_device rd; 
            std::mt19937 gen(rd());
            for(auto iter = _transitionIntervals.begin() ; iter != _transitionIntervals.end() ; iter++) {
                if((*iter).empty()) {
                    transi_index++;
                    continue;
                }
                int date;
                if(_tapn.getTransitions()[transi_index]->isUrgent()) {
                    date = (*iter)[0].lower();
                } else {
                    int intervalLength = Util::setLength(*iter);
                    std::uniform_int_distribution<> distrib(0, intervalLength);
                    date = Util::valueAt(*iter, distrib(gen));
                }
                if(date < date_min) {
                    date_min = date;
                    winner_index = transi_index;
                }
                transi_index++;
            }
            TimedTransition *winner = _tapn.getTransitions()[winner_index];
            return std::make_pair(winner, date_min);
        }

        void RandomRunGenerator::reset() {
            _transitionIntervals = std::vector<std::vector<interval>>(_tapn.getTransitions().size());
        }

        std::vector<interval> RandomRunGenerator::transitionFiringDates(TimedTransition* transi) {
            PlaceList &places = _parent->getPlaceList();
            std::vector<interval> firingInterval;
            for(TimedInputArc* arc : transi->getPreset()) {
                auto pit = places.begin();
                while(pit != places.end() && pit->place->getIndex() != arc->getInputPlace().getIndex()) {
                    ++pit;
                }
                if(pit == places.end()) return firingInterval;
                firingInterval = Util::setIntersection(firingInterval, arcFiringDates(arc->getInterval(), arc->getWeight(), pit->tokens));
                if(firingInterval.empty()) return firingInterval;
            }
            for(TransportArc* arc : transi->getTransportArcs()) {
                auto pit = places.begin();
                while(pit != places.end() && pit->place->getIndex() != arc->getSource().getIndex()) {
                    ++pit;
                }
                if(pit == places.end()) return firingInterval;
                TimeInvariant targetInvariant = arc->getDestination().getInvariant();
                TimeInterval arcInterval = arc->getInterval();
                if(targetInvariant.getBound() < arcInterval.getUpperBound()) {
                    arcInterval.setUpperBound(targetInvariant.getBound(), targetInvariant.isBoundStrict());
                } else if(targetInvariant.getBound() == arcInterval.getUpperBound()) {
                    arcInterval.setUpperBound(targetInvariant.getBound(), 
                        targetInvariant.isBoundStrict() || arcInterval.isUpperBoundStrict());
                }
                firingInterval = Util::setIntersection(firingInterval, arcFiringDates(arcInterval, arc->getWeight(), pit->tokens));
                if(firingInterval.empty()) return firingInterval;
            }
            return firingInterval;
        }

        std::vector<interval> RandomRunGenerator::arcFiringDates(TimeInterval time_interval, uint32_t weight, TokenList& tokens) {
            // We assume tokens is SORTED !
            Util::interval arcInterval(time_interval.getLowerBound(), time_interval.getUpperBound());
            size_t total_tokens = 0;
            for(auto &t : tokens) {
                total_tokens += t.getCount();
            }
            if(total_tokens < weight) return std::vector<interval>();
            std::vector<interval> firingDates;
            size_t firstTokenIndex = 0;
            size_t consumedInFirst = 0;
            while(firstTokenIndex < tokens.size()) {
                Token t = tokens[firstTokenIndex];
                interval tokensSetInterval = remainingForToken(arcInterval, t);
                size_t inThisSet = t.getCount() - consumedInFirst;
                consumedInFirst++;
                if(inThisSet <= weight) {
                    size_t remaining = weight - inThisSet;
                    size_t nextSet = firstTokenIndex + 1;
                    while(remaining >= 0 && nextSet < tokens.size()) {
                        Token nextToken = tokens[nextSet];
                        Util::intersect(tokensSetInterval, remainingForToken(arcInterval, nextToken));
                        if(nextToken.getCount() >= remaining) break;
                        remaining -= nextToken.getCount();
                        nextSet++;
                    }
                    if(remaining > 0) {
                        return firingDates; // Reached end of tokens...
                    }
                }
                Util::setAdd(firingDates, tokensSetInterval);
                if(consumedInFirst >= t.getCount()) {
                    consumedInFirst = 0;
                    firstTokenIndex++;
                }
            }
            return firingDates;

            /* IF TOKENS NOT SORTED !
            std::vector<interval> tokensIntervals;
            for(auto &token : tokens) {
                interval tokenInterv = arcInterval;
                tokenInterv.delta(-token.getAge());
                tokenInterv = tokenInterv.positive();
                if(tokenInterv.empty()) continue;
                for(int count = 0 ; count < std::min((uint32_t) token.getCount(), weight) ; count++) // Useless to include more than weight, would induce no new combination
                    tokensIntervals.push_back(tokenInterv);
            }
            //if(weight == 1) return tokensIntervals;  // would be great but not ordered
            std::vector<size_t> selected(weight);
            std::iota(selected.begin(), selected.end(), 0);
            while(selected[0] <= tokensIntervals.size() - weight) { 
                // Iterates over every (WEIGHT in TOKENS) to compute possible intervals
                // Ex weight = 2 in tokens = 4 : 1100 -> 1010 -> 1001 -> 0110 -> 0101 -> 0011
                interval current = tokensIntervals[selected[0]];
                for(int i = 1 ; i < selected.size() ; i++) {
                    current = Util::intersect(current, tokensIntervals[selected[i]]);
                }
                if(!current.empty()) {
                    Util::setAdd(firingDates, current);
                }
                selected.back()++;
                if(selected.back() == tokensIntervals.size() && selected[0] <= tokensIntervals.size() - weight) {
                    size_t pending = 1;
                    size_t toMove = selected.size() - pending - 1;
                    selected[toMove]++;
                    while(selected[toMove] == tokensIntervals.size() - pending && toMove > 0) { // Try to find most leftish index to increment until reaching the first
                        pending++;
                        toMove--;
                        selected[toMove]++;
                    }
                    for(int i = toMove ; i <= toMove + pending ; i++) { // Setting new values now that shifting is complete
                        selected[i + 1] = selected[i] + 1;
                    }
                }
            }
            return firingDates;
            */
        }

        Util::interval RandomRunGenerator::remainingForToken(const interval& arcInterval, const Token& token) {
            interval tokenInterv = arcInterval;
            tokenInterv.delta(-token.getAge());
            return tokenInterv.positive();
        }

    }
}