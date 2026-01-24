/* 
 * File:   SMCRunGenerator.cpp
 * Author: Tanguy Dubois
 * 
 * Created on 11 April 2024, 10.13
 */

#include "DiscreteVerification/Generators/SMCRunGenerator.h"

#include <numeric>
#include <deque>
#include <random>
#include <algorithm>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace Util;

        void SMCRunGenerator::prepare(RealMarking *parent) {
            _origin = new RealMarking(*parent);
            _parent = new RealMarking(*_origin);
            RealPlaceList& places = _origin->getPlaceList();
            std::vector<bool> transitionSeen(_defaultTransitionIntervals.size(), false);
            clockValue originMaxDelay = _origin->availableDelay(_numericPrecision);
            std::vector<interval<clockValue>> invInterval = { interval<clockValue>(0, originMaxDelay) };
            for(auto transi : _tapn.getTransitions()) {
                if(transi->getPresetSize() == 0 && transi->getNumberOfInhibitorArcs() == 0) {
                    _defaultTransitionIntervals[transi->getIndex()] = invInterval;
                } else {
                    std::vector<interval<clockValue>> firingDates = transitionFiringDates(transi);
                    _defaultTransitionIntervals[transi->getIndex()] = Util::setIntersection(firingDates, invInterval);
                }
            }
            reset();
        }

        void SMCRunGenerator::reset() {
            if(_trace.size() > 0) {
                for(RealMarking* marking : _trace) {
                    if(marking != nullptr) delete marking;
                }
                _trace.clear();
            }
            else if(_parent != nullptr) delete _parent;
            _parent = new RealMarking(*_origin);
            if(recordTrace) {
                _trace = { new RealMarking(*_origin), _parent };
            }
            _transitionIntervals = _defaultTransitionIntervals;
            _maximal = false;
            _totalTime = 0;
            _totalSteps = 0;
            _sample_index = 0;
            _dates_sampled = std::vector<clockValue>(_transitionIntervals.size(), std::numeric_limits<clockValue>::max());
            bool deadlocked = true;
            for(int i = 0 ; i < _dates_sampled.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                if(!intervals->empty() && intervals->front().lower() == 0) {
                    const Distribution& distrib = _tapn.getTransitions()[i]->getDistribution();
                    _dates_sampled[i] = toClock(distrib.sample(_rng, _sample_index), _numericPrecision);
                }
                deadlocked &=   _transitionIntervals[i].empty() || 
                                (
                                    _transitionIntervals[i].size() == 1 &&
                                    _transitionIntervals[i].front().upper() == 0 &&
                                    _dates_sampled[i] > 0
                                );
            }
            _parent->setDeadlocked(deadlocked);
            for(int i = 0 ; i < _currentPlacesStatistics.size() ; i++) {
                _placesStatistics[i] += _currentPlacesStatistics[i];
                _currentPlacesStatistics[i] = _origin->numberOfTokensInPlace(i);
            }
        }

        SMCRunGenerator SMCRunGenerator::copy() const
        {
            SMCRunGenerator clone(_tapn, _numericPrecision);
            clone._origin = new RealMarking(*_origin);
            clone._numericPrecision = _numericPrecision;
            clone._defaultTransitionIntervals = _defaultTransitionIntervals;
            clone.recordTrace = recordTrace;
            clone.reset();
            return clone;
        }

        void SMCRunGenerator::refreshTransitionsIntervals()
        {
            clockValue max_delay = _parent->availableDelay(_numericPrecision);
            std::vector<interval<clockValue>> invInterval = { interval<clockValue>(0, max_delay) };
            bool deadlocked = true;
            for(auto transi : _tapn.getTransitions()) {
                int i = transi->getIndex();
                if(transi->getPresetSize() == 0 && transi->getNumberOfInhibitorArcs() == 0) {
                    _transitionIntervals[i] = invInterval;
                } else {
                    std::vector<interval<clockValue>> firingDates = transitionFiringDates(transi);
                    _transitionIntervals[i] = Util::setIntersection(firingDates, invInterval);
                }
                bool enabled = (!_transitionIntervals[i].empty()) && (_transitionIntervals[i].front().lower() == 0);
                bool newlyEnabled = enabled && (_dates_sampled[i] == std::numeric_limits<clockValue>::max());
                bool reachedUpper = enabled && !newlyEnabled && (_transitionIntervals[i].front().upper() == 0) && _dates_sampled[i] > 0;
                if(!enabled || reachedUpper) {
                    _dates_sampled[i] = std::numeric_limits<clockValue>::max();
                } else if(newlyEnabled) {
                    const Distribution& distrib = _tapn.getTransitions()[i]->getDistribution();
                    clockValue date = toClock(distrib.sample(_rng, _sample_index), _numericPrecision);
                    if(_transitionIntervals[i].front().upper() > 0 || date == 0) {
                        _dates_sampled[i] = date;
                    }
                }
                deadlocked &=   _transitionIntervals[i].empty() || 
                                (
                                    _transitionIntervals[i].size() == 1 &&
                                    _transitionIntervals[i].front().upper() == 0 &&
                                    _dates_sampled[i] > 0
                                );
            }
            _parent->setDeadlocked(deadlocked);
        }

        void SMCRunGenerator::disableTransitions(RealMarking* marking) {
            for(int i = 0 ; i < _dates_sampled.size() ; i++) {
                clockValue date = _dates_sampled[i];
                if(date == std::numeric_limits<clockValue>::max()) continue;
                TimedTransition* transition = _tapn.getTransitions()[i];
                if(!marking->enables(transition, _numericPrecision)) {
                    _dates_sampled[i] = std::numeric_limits<clockValue>::max();
                }
            }
        }

        RealMarking* SMCRunGenerator::next() {
            auto [transi, delay] = getWinnerTransitionAndDelay();
            
            if(delay == std::numeric_limits<clockValue>::max()) {
                _maximal = true;
                return nullptr;
            }

            if(recordTrace && _parent->getGeneratedBy() != nullptr) {
                _parent = new RealMarking(*_parent);
                _trace.push_back(_parent);
            }

            // std::cout << "Marking ---------------" << std::endl;
            // for(auto& place : _parent->getPlaceList()) {
            //     if(place.numberOfTokens() == 0) continue;
            //     std::cout << "place " << place.placeId() << " ";
            //     for(auto& tok : place.tokens) {
            //         std::cout << tok.getAge() << ", ";
            //     }
            //     std::cout << std::endl;
            // }

            _parent->deltaAge(delay);

            _totalTime += delay;

            _parent->setPreviousDelay(delay + _parent->getPreviousDelay());
        
            if(transi != nullptr) {
                _totalSteps++;
                _transitionsStatistics[transi->getIndex()]++;
                _dates_sampled[transi->getIndex()] = std::numeric_limits<clockValue>::max();
                auto child = fire(transi);
                child->setGeneratedBy(transi);
                if(recordTrace) {
                    _trace.push_back(child);
                } else {
                    delete _parent;
                }
                _parent = child;
            }

            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
                clockValue date = _dates_sampled[i];
                _dates_sampled[i] = (date == std::numeric_limits<clockValue>::max()) ?
                    std::numeric_limits<clockValue>::max() : date - delay;
            }

            refreshTransitionsIntervals();

            return _parent;
        }

        std::pair<TimedTransition*, clockValue> SMCRunGenerator::getWinnerTransitionAndDelay() {
            std::vector<size_t> winner_indexs;
            clockValue date_min = std::numeric_limits<clockValue>::max();
            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                if(intervals->empty()) continue;
                clockValue date = std::numeric_limits<clockValue>::max();
                for(auto& interv : *intervals) {
                    if(interv.lower() > 0) {
                        date = interv.lower();
                        break;
                    }
                    if(interv.upper() > 0) {
                        date = interv.upper();
                        break;
                    }
                }
                date = std::min(_dates_sampled[i], date);
                if(date < date_min) {
                    date_min = date;
                    winner_indexs.clear();
                }
                if(_dates_sampled[i] == date_min) {
                    winner_indexs.push_back(i);
                }
            }
            TimedTransition *winner;
            if(winner_indexs.empty()) { 
                winner = nullptr;
            } else if(winner_indexs.size() == 1) {
                winner = _tapn.getTransitions()[winner_indexs[0]];
            } else {
                winner = chooseWeightedWinner(winner_indexs);
            }
            return std::make_pair(winner, date_min);
        }

        TimedTransition* SMCRunGenerator::chooseWeightedWinner(const std::vector<size_t>& winner_indexs) {
            clockValue total_weight = 0;
            std::vector<size_t> infty_weights;
            for(auto& candidate : winner_indexs) {
                double priority = _tapn.getTransitions()[candidate]->getWeight();
                if(priority == std::numeric_limits<double>::infinity()) {
                    infty_weights.push_back(candidate);
                } else {
                    total_weight += priority;
                }
            }
            if(!infty_weights.empty()) {
                int winner_index = std::uniform_int_distribution<>(0, infty_weights.size() - 1)(_rng);
                return _tapn.getTransitions()[infty_weights[winner_index]];
            }
            if(total_weight == 0) {
                int winner_index = std::uniform_int_distribution<>(0, winner_indexs.size() - 1)(_rng);
                return _tapn.getTransitions()[winner_indexs[winner_index]];
            }
            double winning_weight = std::uniform_real_distribution<>(0.0, total_weight)(_rng);
            for(auto& candidate : winner_indexs) {
                TimedTransition* transi = _tapn.getTransitions()[candidate];
                winning_weight -= transi->getWeight();
                if(winning_weight <= 0) {
                    return transi;
                }
            }
            return _tapn.getTransitions()[winner_indexs[0]];
        }

        std::vector<interval<clockValue>> SMCRunGenerator::transitionFiringDates(TimedTransition* transi) {
            RealPlaceList &places = _parent->getPlaceList();
            std::vector<interval<clockValue>> firingInterval = { interval<clockValue>(0, std::numeric_limits<clockValue>::max()) };
            std::vector<interval<clockValue>> disabled;
            for(InhibitorArc* inhib : transi->getInhibitorArcs()) {
                if(_parent->numberOfTokensInPlace(inhib->getInputPlace().getIndex()) >= inhib->getWeight()) {
                    return disabled;
                } 
            }
            for(TimedInputArc* arc : transi->getPreset()) {
                auto &place = _parent->getPlaceList()[arc->getInputPlace().getIndex()];
                if(place.isEmpty()) return disabled;
                firingInterval = Util::setIntersection<clockValue>(firingInterval, arcFiringDates(arc->getInterval(), arc->getWeight(), place.tokens));
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
                firingInterval = Util::setIntersection<clockValue>(firingInterval, arcFiringDates(arcInterval, arc->getWeight(), place.tokens));
                if(firingInterval.empty()) return firingInterval;
            }
            return firingInterval;
        }

        std::vector<interval<clockValue>> SMCRunGenerator::arcFiringDates(TimeInterval time_interval, uint32_t weight, RealTokenList& tokens) {
            // We assume tokens is SORTED !
            clockValue lower = toClock(time_interval.getLowerBound(), _numericPrecision);
            clockValue upper = toClock(time_interval.getUpperBound(), _numericPrecision);
            Util::interval<clockValue> arcInterval(lower, upper);
            size_t total_tokens = 0;
            for(auto &t : tokens) {
                total_tokens += t.getCount();
            }
            if(total_tokens < weight) return std::vector<interval<clockValue>>();
            std::vector<interval<clockValue>> firingDates;
            std::deque<clockValue> selected;
            for(auto& tokenPckt : tokens) {
                for(int i = 0 ; i < tokenPckt.getCount() ; i++) {
                    selected.push_back(tokenPckt.getAge());
                    if(selected.size() > weight) {
                        selected.pop_front();
                    }
                    if(selected.size() == weight) {
                        interval<clockValue> tokenSetInterval = interval<clockValue>(0, std::numeric_limits<clockValue>::max());
                        for(auto age : selected) {
                            interval<clockValue> shifted = arcInterval;
                            shifted.delta_neg(age);
                            tokenSetInterval = Util::intersect(tokenSetInterval, shifted);
                        }
                        Util::setAdd(firingDates, tokenSetInterval);
                    }
                }
            }
            return firingDates;
        }
        
        std::vector<RealToken> SMCRunGenerator::removeRandom(RealTokenList& tokenList, const TimeInterval& interval, const int weight) {
            std::vector<RealToken> res;
            int remaining = weight;
            std::uniform_int_distribution<> randomTokenIndex(0, tokenList.size() - 1);
            size_t tok_index = randomTokenIndex(_rng);
            size_t tested = 0;
            clockValue lower = toClock(interval.getLowerBound(), _numericPrecision);
            clockValue upper = toClock(interval.getUpperBound(), _numericPrecision);
            while(remaining > 0 && tested < tokenList.size()) {
                RealToken& token = tokenList[tok_index];
                clockValue age = token.getAge();
                if(lower <= age && upper >= age) {
                    res.push_back(RealToken(age, 1));
                    remaining--;
                    tokenList[tok_index].remove(1);
                    if(tokenList[tok_index].getCount() == 0) {
                        tokenList.erase(tokenList.begin() + tok_index);
                        randomTokenIndex = std::uniform_int_distribution<>(0, tokenList.size() - 1);
                    }
                    if(remaining > 0) {
                        tok_index = randomTokenIndex(_rng);
                        tested = 0;
                    }
                } else {
                    tok_index = (tok_index + 1) % tokenList.size();
                    tested++;
                }
            }
            assert(remaining == 0);
            return res;
        }

        std::vector<RealToken> SMCRunGenerator::removeYoungest(RealTokenList& tokenList, const TimeInterval& interval, const int weight) {
            std::vector<RealToken> res;
            int remaining = weight;
            auto iter = tokenList.begin();
            clockValue lower = toClock(interval.getLowerBound(), _numericPrecision);
            clockValue upper = toClock(interval.getUpperBound(), _numericPrecision);
            while(iter != tokenList.end()) {
                clockValue age = iter->getAge();
                if(lower > age || upper < age) {
                    iter++;
                    continue;
                }
                int count = iter->getCount();
                if(count >= remaining) {
                    res.push_back(RealToken(age, remaining));
                    iter->remove(remaining);
                    if(iter->getCount() == 0) tokenList.erase(iter);
                    remaining = 0;
                    break;
                } else {
                    res.push_back(RealToken(age, count));
                    remaining -= count;
                    iter = tokenList.erase(iter);
                }
            }
            assert(remaining == 0);
            return res;
        }

        std::vector<RealToken> SMCRunGenerator::removeOldest(RealTokenList& tokenList, const TimeInterval& interval, const int weight) {
            std::vector<RealToken> res;
            int remaining = weight;
            auto iter = tokenList.rbegin();
            clockValue lower = toClock(interval.getLowerBound(), _numericPrecision);
            clockValue upper = toClock(interval.getUpperBound(), _numericPrecision);
            while(iter != tokenList.rend()) {
                clockValue age = iter->getAge();
                if(lower > age || upper < age) {
                    iter++;
                    continue;
                }
                int count = iter->getCount();
                if(count >= remaining) {
                    res.push_back(RealToken(age, remaining));
                    iter->remove(remaining);
                    if(iter->getCount() == 0) tokenList.erase(std::next(iter).base());
                    remaining = 0;
                    break;
                } else {
                    res.push_back(RealToken(age, count));
                    remaining -= count;
                    iter = decltype(iter)(tokenList.erase(std::next(iter).base()));
                }
            }
            assert(remaining == 0);
            return res;
        }

        RealMarking* SMCRunGenerator::fire(TimedTransition* transi) {
            if (transi == nullptr) {
                assert(false);
                return nullptr;
            }
            auto *child = new RealMarking(*_parent);
            RealPlaceList &placelist = child->getPlaceList();

            for (auto &input : transi->getPreset()) {
                RealPlace& place = placelist[input->getInputPlace().getIndex()];
                RealTokenList& tokenList = place.tokens;
                switch(transi->getFiringMode()) {
                    case SMC::Random:
                        removeRandom(tokenList, input->getInterval(), input->getWeight());
                        break;
                    case SMC::Oldest:
                        removeOldest(tokenList, input->getInterval(), input->getWeight());
                        break;
                    case SMC::Youngest:
                        removeYoungest(tokenList, input->getInterval(), input->getWeight());
                        break;
                    default:
                        removeOldest(tokenList, input->getInterval(), input->getWeight());
                        break;
                }
            }

            std::vector<std::tuple<TimedPlace&, RealToken>> toCreate;
            for (auto &transport : transi->getTransportArcs()) {
                int destInv = transport->getDestination().getInvariant().getBound();
                RealPlace& place = placelist[transport->getSource().getIndex()];
                RealTokenList& tokenList = place.tokens;
                std::vector<RealToken> consumed;
                TimeInterval interval = transport->getInterval();
                if(destInv < interval.getUpperBound()) interval.setUpperBound(destInv, false);
                switch(transi->getFiringMode()) {
                    case SMC::Random:
                        consumed = removeRandom(tokenList, interval, transport->getWeight());
                        break;
                    case SMC::Oldest:
                        consumed = removeOldest(tokenList, interval, transport->getWeight());
                        break;
                    case SMC::Youngest:
                        consumed = removeYoungest(tokenList, interval, transport->getWeight());
                        break;
                    default:
                        consumed = removeOldest(tokenList, interval, transport->getWeight());
                        break;
                }
                for(RealToken token : consumed) {
                    toCreate.push_back({transport->getDestination(), token});
                }
            }

            for (auto* output : transi->getPostset()) {
                TimedPlace &place = output->getOutputPlace();
                RealToken token = RealToken(0, output->getWeight());
                child->addTokenInPlace(place, token);
                int place_i = place.getIndex();
                if(child->numberOfTokensInPlace(place_i) > _currentPlacesStatistics[place_i]) {
                    _currentPlacesStatistics[place_i] = child->numberOfTokensInPlace(place_i);
                }
            }
            for (auto [dest, token] : toCreate) {
                child->addTokenInPlace(dest, token);
                int place_i = dest.getIndex();
                if(child->numberOfTokensInPlace(place_i) > _currentPlacesStatistics[place_i]) {
                    _currentPlacesStatistics[place_i] = child->numberOfTokensInPlace(place_i);
                }
            }
            return child;
        }

        bool SMCRunGenerator::reachedEnd() const {
            return _maximal;
        }

        clockValue SMCRunGenerator::getRunDelay() const {
            return _totalTime;
        }

        int SMCRunGenerator::getRunSteps() const {
            return _totalSteps;
        }

        void SMCRunGenerator::printTransitionStatistics(std::ostream &out, const size_t& n) const {
            out << std::endl << "TRANSITION STATISTICS";
            for (unsigned int i = 0; i < _transitionsStatistics.size(); i++) {
                if ((i) % 6 == 0) {
                    out << std::endl;
                    out << "<" << _tapn.getTransitions()[i]->getName() << ";" << (((double) _transitionsStatistics[i]) / n) << ">";
                } else {
                    out << " <" << _tapn.getTransitions()[i]->getName() << ";" << (((double) _transitionsStatistics[i]) / n) << ">";
                }
            }
            out << std::endl;
            out << std::endl;
        }

        void SMCRunGenerator::printPlaceStatistics(std::ostream &out, const size_t& n) const {
            out << std::endl << "PLACE-BOUND STATISTICS";
            for (unsigned int i = 0; i < _placesStatistics.size(); i++) {
                if ((i) % 6 == 0) {
                    out << std::endl;
                    out << "<" << _tapn.getPlaces()[i]->getName() << ";" << (((double) _placesStatistics[i]) / n) << ">";
                } else {
                    out << " <" << _tapn.getPlaces()[i]->getName() << ";" << (((double) _placesStatistics[i]) / n) << ">";
                }
            }
            out << std::endl;
            out << std::endl;
        }

        void SMCRunGenerator::mergeStatistics(const SMCRunGenerator& other) {
            for(int i = 0 ; i < _transitionsStatistics.size() ; i++) {
                _transitionsStatistics[i] += other._transitionsStatistics[i];
            }
            for(int i = 0 ; i < _placesStatistics.size() ; i++) {
                _placesStatistics[i] += other._placesStatistics[i];
            }
            // for(int i = 0 ; i < _placesStatistics.size() ; i++) {
            //     if(_placesStatistics[i] < other._placesStatistics[i]) {
            //         _placesStatistics[i] = other._placesStatistics[i];
            //     }
            // }   
        }

        std::stack<RealMarking*> SMCRunGenerator::getTrace() const {
            std::stack<RealMarking*> trace;
            for(int i = 0 ; i < _trace.size() ; i++) {
                RealMarking* marking = _trace[_trace.size() - 1 - i];
                if(marking == nullptr) trace.push(nullptr);
                else {
                    RealMarking* toSave = new RealMarking(*marking);
                    toSave->setGeneratedBy(marking->getGeneratedBy());
                    toSave->setPreviousDelay(marking->getPreviousDelay());
                    toSave->setDeadlocked(marking->canDeadlock(_tapn, 0));
                    trace.push(toSave);
                }
            }
            return trace;
        }

    }
}
