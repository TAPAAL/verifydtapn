/* 
 * File:   SMCRunGenerator.cpp
 * Author: Tanguy Dubois
 * 
 * Created on 11 April 2024, 10.13
 */

#include "DiscreteVerification/Generators/SMCRunGenerator.h"

#include <numeric>
#include <random>
#include <algorithm>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using Util::interval;

        void SMCRunGenerator::prepare(RealMarking *parent) {
            _origin = new RealMarking(*parent);
            _parent = new RealMarking(*_origin);
            RealPlaceList& places = _origin->getPlaceList();
            std::vector<bool> transitionSeen(_defaultTransitionIntervals.size(), false);
            _originMaxDelay = std::numeric_limits<double>::infinity();
            for(auto &pit : places) {
                if(pit.place->getInvariant().getBound() != std::numeric_limits<int>::max()) {
                    double place_max_delay = pit.availableDelay();
                    if(place_max_delay < _originMaxDelay) {
                        _originMaxDelay = place_max_delay;
                    }
                }
                for(auto arc : pit.place->getInputArcs()) {
                    TimedTransition &transi = arc->getOutputTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval<double>> firingDates = transitionFiringDates(&transi);
                    _defaultTransitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
                for(auto arc : pit.place->getTransportArcs()) {
                    TimedTransition &transi = arc->getTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval<double>> firingDates = transitionFiringDates(&transi);
                    _defaultTransitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
            }
            for(auto &transi : _tapn.getTransitions()) {
                if(transi->getPresetSize() == 0) {
                    _defaultTransitionIntervals[transi->getIndex()] = { interval<double>(0, std::numeric_limits<double>::infinity()) };
                    transitionSeen[transi->getIndex()] = true;
                }
            }
            std::vector<interval<double>> invInterval = { interval<double>(0, _originMaxDelay) };
            for(auto iter = _defaultTransitionIntervals.begin() ; iter != _defaultTransitionIntervals.end() ; iter++) {
                if(iter->empty()) continue;
                *iter = Util::setIntersection(*iter, invInterval);
            }
            reset();
        }

        void SMCRunGenerator::reset() {
            std::random_device rd;
            std::ranlux48 gen(rd());
            if(_parent != nullptr) {
                delete _parent;
            }
            _parent = new RealMarking(*_origin);
            _transitionIntervals = _defaultTransitionIntervals;
            _maximal = false;
            _max_delay = _originMaxDelay;
            _totalTime = 0;
            _totalSteps = 0;
            _modifiedPlaces.clear();
            _dates_sampled = std::vector<double>(_transitionIntervals.size(), std::numeric_limits<double>::infinity());
            for(int i = 0 ; i < _dates_sampled.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                if(!intervals->empty() && intervals->front().lower() == 0) {
                    const SMCDistribution& distrib = _tapn.getTransitions()[i]->getDistribution();
                    _dates_sampled[i] = distrib.sample(gen);
                }
            }
        }

        void SMCRunGenerator::refreshTransitionsIntervals() {
            RealPlaceList& places = _parent->getPlaceList();
            std::vector<bool> transitionSeen(_transitionIntervals.size(), false);
            std::random_device rd;
            std::ranlux48 gen(rd());
            _max_delay = _parent->availableDelay();
            for(auto &modified : _modifiedPlaces) {
                const TimedPlace& place = _tapn.getPlace(modified);
                for(auto arc : place.getInputArcs()) {
                    TimedTransition &transi = arc->getOutputTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval<double>> firingDates = transitionFiringDates(&transi);
                    _transitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
                for(auto arc : place.getInhibitorArcs()) {
                    TimedTransition &transi = arc->getOutputTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval<double>> firingDates = transitionFiringDates(&transi);
                    _transitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
                for(auto arc : place.getTransportArcs()) {
                    TimedTransition &transi = arc->getTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval<double>> firingDates = transitionFiringDates(&transi);
                    _transitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
            }
            std::vector<interval<double>> invInterval = { interval<double>(0, _max_delay) };
            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
                _transitionIntervals[i] = Util::setIntersection<double>(_transitionIntervals[i], invInterval);
                bool enabled = (!_transitionIntervals[i].empty()) && (_transitionIntervals[i].front().lower() == 0);
                bool newlyEnabled = enabled && (_dates_sampled[i] == std::numeric_limits<double>::infinity());
                if(!enabled) {
                    _dates_sampled[i] = std::numeric_limits<double>::infinity();
                } else if(_transitionIntervals[i].front().upper() == 0 && !newlyEnabled) { // Happens if not fired
                    _transitionIntervals[i].clear();
                    _dates_sampled[i] = std::numeric_limits<double>::infinity();
                } else if(newlyEnabled) {
                    const SMCDistribution& distrib = _tapn.getTransitions()[i]->getDistribution();
                    _dates_sampled[i] = distrib.sample(gen);
                }
            }
        }

        RealMarking* SMCRunGenerator::next() {
            auto [transi, delay] = getWinnerTransitionAndDelay();
            
            if(delay == std::numeric_limits<double>::infinity()) {
                _maximal = true;
                return nullptr;
            }

            _parent->deltaAge(delay);
            _totalTime += delay;
            _modifiedPlaces.clear();
        
            if(transi != nullptr) {
                _totalSteps++;
                _transitionsStatistics[transi->getIndex()]++;
                _dates_sampled[transi->getIndex()] = std::numeric_limits<double>::infinity();
                RealMarking* child = fire(transi);
                delete _parent;
                _parent = child;
            }

            // Translate intervals, so we don't have to compute some of them next
            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                Util::setDeltaIntoPositive(*intervals, -delay);
                double date = _dates_sampled[i];
                _dates_sampled[i] = (date == std::numeric_limits<double>::infinity()) ?
                    std::numeric_limits<double>::infinity() : date - delay;
            }

            refreshTransitionsIntervals();

            return _parent;
        }

        std::pair<TimedTransition*, double> SMCRunGenerator::getWinnerTransitionAndDelay() {
            std::vector<size_t> winner_indexs;
            double date_min = std::numeric_limits<double>::infinity();
            std::random_device rd;
            std::ranlux48 gen(rd());
            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                if(!intervals->empty()) {
                    interval<double>& first = intervals->front();
                    double date = first.lower() == 0 ? first.upper() : first.lower();
                    if(date < date_min) {
                        date_min = date;
                        winner_indexs.clear();
                    }
                }
                double date = _dates_sampled[i];
                if(date != std::numeric_limits<double>::infinity()) {
                    if(date < date_min) {
                        date_min = date;
                        winner_indexs.clear();
                    }
                    if(date == date_min) {
                        winner_indexs.push_back(i);
                    }
                }
            }
            if(date_min == std::numeric_limits<double>::infinity()) {
                return std::make_pair(nullptr, date_min);
            }
            TimedTransition *winner;
            if(winner_indexs.size() == 0) { 
                winner = nullptr;
            } else {
                std::uniform_int_distribution<> distrib(0, winner_indexs.size() - 1);
                winner = _tapn.getTransitions()[winner_indexs[distrib(gen)]];
            }
            return std::make_pair(winner, date_min);
        }

        std::vector<interval<double>> SMCRunGenerator::transitionFiringDates(TimedTransition* transi) {
            RealPlaceList &places = _parent->getPlaceList();
            std::vector<interval<double>> firingInterval = { interval<double>(0, std::numeric_limits<double>::infinity()) };
            std::vector<interval<double>> disabled;
            for(InhibitorArc* inhib : transi->getInhibitorArcs()) {
                if(_parent->numberOfTokensInPlace(inhib->getInputPlace().getIndex()) >= inhib->getWeight()) {
                    return disabled;
                } 
            }
            for(TimedInputArc* arc : transi->getPreset()) {
                auto &place = _parent->getPlaceList()[arc->getInputPlace().getIndex()];
                if(place.isEmpty()) return disabled;
                firingInterval = Util::setIntersection<double>(firingInterval, arcFiringDates(arc->getInterval(), arc->getWeight(), place.tokens));
                if(firingInterval.empty()) return firingInterval;
            }
            for(TransportArc* arc : transi->getTransportArcs()) {
                auto &place = _parent->getPlaceList()[arc->getSource().getIndex()];
                if(place.isEmpty()) return disabled;
                TimeInvariant targetInvariant = arc->getDestination().getInvariant();
                TimeInterval arcInterval = arc->getInterval();
                if(targetInvariant.getBound() < arcInterval.getUpperBound()) {
                    arcInterval.setUpperBound(targetInvariant.getBound(), targetInvariant.isBoundStrict());
                } 
                firingInterval = Util::setIntersection<double>(firingInterval, arcFiringDates(arcInterval, arc->getWeight(), place.tokens));
                if(firingInterval.empty()) return firingInterval;
            }
            return firingInterval;
        }

        std::vector<interval<double>> SMCRunGenerator::arcFiringDates(TimeInterval time_interval, uint32_t weight, RealTokenList& tokens) {
            // We assume tokens is SORTED !
            Util::interval<double> arcInterval(time_interval.getLowerBound(), time_interval.getUpperBound());
            size_t total_tokens = 0;
            for(auto &t : tokens) {
                total_tokens += t.getCount();
            }
            if(total_tokens < weight) return std::vector<interval<double>>();
            std::vector<interval<double>> firingDates;
            size_t firstTokenIndex = 0;
            size_t consumedInFirst = 0;
            while(firstTokenIndex < tokens.size()) {
                RealToken t = tokens[firstTokenIndex];
                interval<double> tokensSetInterval = remainingForToken(arcInterval, t);
                size_t inThisSet = t.getCount() - consumedInFirst;
                consumedInFirst++;
                if(inThisSet <= weight) {
                    size_t remaining = weight - inThisSet;
                    size_t nextSet = firstTokenIndex + 1;
                    while(remaining >= 0 && nextSet < tokens.size()) {
                        RealToken nextToken = tokens[nextSet];
                        tokensSetInterval = Util::intersect<double>(tokensSetInterval, remainingForToken(arcInterval, nextToken));
                        if(nextToken.getCount() >= remaining) break;
                        remaining -= nextToken.getCount();
                        nextSet++;
                    }
                    if(remaining > 0) {
                        return firingDates; // Reached end of tokens...
                    }
                }
                Util::setAdd<double>(firingDates, tokensSetInterval);
                if(consumedInFirst >= t.getCount()) {
                    consumedInFirst = 0;
                    firstTokenIndex++;
                }
            }
            return firingDates;
        }

        Util::interval<double> SMCRunGenerator::remainingForToken(const interval<double>& arcInterval, const RealToken& token) {
            interval<double> tokenInterv = arcInterval;
            tokenInterv.delta(-token.getAge());
            return tokenInterv.positive();
        }

        RealMarking* SMCRunGenerator::fire(TimedTransition* transi) {
            if (transi == nullptr) {
                assert(false);
                return nullptr;
            }
            std::random_device rd; 
            std::ranlux48 gen(rd());
            auto *child = new RealMarking(*_parent);
            RealPlaceList &placelist = child->getPlaceList();

            for (auto &input : transi->getPreset()) {
                int source = input->getInputPlace().getIndex();
                RealPlace& place = placelist[source];
                RealTokenList& tokenlist = place.tokens;
                int remaining = input->getWeight();
                std::uniform_int_distribution<> randomTokenIndex(0, tokenlist.size() - 1);
                size_t tok_index = randomTokenIndex(gen);
                size_t tested = 0;
                while(remaining > 0 && tested < tokenlist.size()) {
                    if(input->getInterval().contains(tokenlist[tok_index].getAge())) {
                        remaining--;
                        tokenlist[tok_index].remove(1);
                        if(tokenlist[tok_index].getCount() == 0) {
                            tokenlist.erase(tokenlist.begin() + tok_index);
                            randomTokenIndex = std::uniform_int_distribution<>(0, tokenlist.size() - 1);
                        }
                        if(remaining > 0) {
                            tok_index = randomTokenIndex(gen);
                            tested = 0;
                        }
                    } else {
                        tok_index = (tok_index + 1) % tokenlist.size();
                        tested++;
                    }
                }
                assert(remaining == 0);
                _modifiedPlaces.push_back(source);
            }

            for (auto &transport : transi->getTransportArcs()) {
                int source = transport->getSource().getIndex();
                int dest = transport->getDestination().getIndex();
                int destInv = transport->getDestination().getInvariant().getBound();
                RealPlace& place = placelist[source];
                RealTokenList& tokenlist = place.tokens;
                int remaining = transport->getWeight();
                std::uniform_int_distribution<> randomTokenIndex(0, tokenlist.size() - 1);
                size_t tok_index = randomTokenIndex(gen);
                size_t tested = 0;
                while(remaining > 0 && tested < tokenlist.size()) {
                    double age = tokenlist[tok_index].getAge();
                    if(transport->getInterval().contains(age) && age <= destInv) {
                        remaining--;
                        tokenlist[tok_index].remove(1);
                        if(tokenlist[tok_index].getCount() == 0) {
                            tokenlist.erase(tokenlist.begin() + tok_index);
                            randomTokenIndex = std::uniform_int_distribution<>(0, tokenlist.size() - 1);
                        }
                        if(remaining > 0) {
                            tok_index = randomTokenIndex(gen);
                            tested = 0;
                        }
                        child->addTokenInPlace(transport->getDestination(), age);
                    } else {
                        tok_index = (tok_index + 1) % tokenlist.size();
                        tested++;
                    }
                }
                assert(remaining == 0);
                _modifiedPlaces.push_back(source);
                _modifiedPlaces.push_back(dest);
            }
            for (auto* output : transi->getPostset()) {
                TimedPlace &place = output->getOutputPlace();
                RealToken token = RealToken(0.0, output->getWeight());
                child->addTokenInPlace(place, token);
                _modifiedPlaces.push_back(place.getIndex());
            }
            return child;
        }

        bool SMCRunGenerator::reachedEnd() const {
            return _maximal;
        }

        double SMCRunGenerator::getRunDelay() const {
            return _totalTime;
        }

        int SMCRunGenerator::getRunSteps() const {
            return _totalSteps;
        }

        void SMCRunGenerator::printTransitionStatistics(std::ostream &out) const {
            out << std::endl << "TRANSITION STATISTICS";
            for (unsigned int i = 0; i < _transitionsStatistics.size(); i++) {
                if ((i) % 6 == 0) {
                    out << std::endl;
                    out << "<" << _tapn.getTransitions()[i]->getName() << ":" << _transitionsStatistics[i] << ">";
                } else {
                    out << " <" << _tapn.getTransitions()[i]->getName() << ":" << _transitionsStatistics[i] << ">";
                }
            }
            out << std::endl;
            out << std::endl;
        }

    }
}