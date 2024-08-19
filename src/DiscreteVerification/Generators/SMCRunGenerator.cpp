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
            double originMaxDelay = _origin->availableDelay();
            std::vector<interval<double>> invInterval = { interval<double>(0, originMaxDelay) };
            for(auto transi : _tapn.getTransitions()) {
                if(transi->getPresetSize() == 0) {
                    _defaultTransitionIntervals[transi->getIndex()] = invInterval;
                } else {
                    std::vector<interval<double>> firingDates = transitionFiringDates(transi);
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
            _dates_sampled = std::vector<double>(_transitionIntervals.size(), std::numeric_limits<double>::infinity());
            bool deadlocked = true;
            for(int i = 0 ; i < _dates_sampled.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                if(!intervals->empty() && intervals->front().lower() == 0) {
                    const Distribution& distrib = _tapn.getTransitions()[i]->getDistribution();
                    _dates_sampled[i] = distrib.sample(_rng);
                }
                deadlocked &=   _transitionIntervals[i].empty() || 
                                (
                                    _transitionIntervals[i].front().upper() == 0 &&
                                    _dates_sampled[i] > 0
                                );
            }
            _parent->setDeadlocked(deadlocked);
        }

        SMCRunGenerator SMCRunGenerator::copy() const
        {
            SMCRunGenerator clone(_tapn);
            clone._origin = new RealMarking(*_origin);
            clone._defaultTransitionIntervals = _defaultTransitionIntervals;
            clone.recordTrace = recordTrace;
            clone.reset();
            return clone;
        }

        void SMCRunGenerator::refreshTransitionsIntervals()
        {
            double max_delay = _parent->availableDelay();
            std::vector<interval<double>> invInterval = { interval<double>(0, max_delay) };
            bool deadlocked = true;
            for(auto transi : _tapn.getTransitions()) {
                int i = transi->getIndex();
                if(transi->getPresetSize() == 0) {
                    _transitionIntervals[i] = invInterval;
                } else {
                    std::vector<interval<double>> firingDates = transitionFiringDates(transi);
                    _transitionIntervals[i] = Util::setIntersection(firingDates, invInterval);                    
                }
                bool enabled = (!_transitionIntervals[i].empty()) && (_transitionIntervals[i].front().lower() == 0);
                bool newlyEnabled = enabled && (_dates_sampled[i] == std::numeric_limits<double>::infinity());
                if(!enabled) {
                    _dates_sampled[i] = std::numeric_limits<double>::infinity();
                } else if(newlyEnabled) {
                    const Distribution& distrib = _tapn.getTransitions()[i]->getDistribution();
                    _dates_sampled[i] = distrib.sample(_rng);
                }
                deadlocked &=   _transitionIntervals[i].empty() || 
                                (
                                    _transitionIntervals[i].front().upper() == 0 &&
                                    _dates_sampled[i] > 0
                                );
            }
            _parent->setDeadlocked(deadlocked);
        }

        void SMCRunGenerator::disableTransitions(RealMarking* marking) {
            for(int i = 0 ; i < _dates_sampled.size() ; i++) {
                double date = _dates_sampled[i];
                if(date == std::numeric_limits<double>::infinity()) continue;
                TimedTransition* transition = _tapn.getTransitions()[i];
                if(!marking->enables(transition)) {
                    _dates_sampled[i] = std::numeric_limits<double>::infinity();
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

            _parent->setPreviousDelay(delay + _parent->getPreviousDelay());
        
            if(transi != nullptr) {
                _totalSteps++;
                _transitionsStatistics[transi->getIndex()]++;
                _dates_sampled[transi->getIndex()] = std::numeric_limits<double>::infinity();
                auto [inter, child] = fire(transi);
                disableTransitions(inter);
                delete inter;
                child->setGeneratedBy(transi);
                if(recordTrace) {
                    _trace.push_back(child);
                    _parent = new RealMarking(*child);
                    _trace.push_back(_parent);
                } else {
                    delete _parent;
                    _parent = child;
                }
            }

            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
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
            for(int i = 0 ; i < _transitionIntervals.size() ; i++) {
                auto* intervals = &_transitionIntervals[i];
                if(intervals->empty()) continue;
                interval<double>& first = intervals->front();
                double date = first.lower() > 0 ? first.lower() :
                                first.upper() > 0 ? first.upper() : 
                                std::numeric_limits<double>::infinity();
                if(date < date_min) {
                    date_min = date;
                    winner_indexs.clear();
                }
                date = _dates_sampled[i];
                if(date != std::numeric_limits<double>::infinity() && date <= first.upper()) {
                    if(date < date_min) {
                        date_min = date;
                        winner_indexs.clear();
                    }
                    if(date == date_min) {
                        winner_indexs.push_back(i);
                    }
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
            double total_weight = 0.0f;
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
        
        std::vector<RealToken> SMCRunGenerator::removeRandom(RealTokenList& tokenList, const TimeInterval& interval, const int weight) {
            std::vector<RealToken> res;
            int remaining = weight;
            std::uniform_int_distribution<> randomTokenIndex(0, tokenList.size() - 1);
            size_t tok_index = randomTokenIndex(_rng);
            size_t tested = 0;
            while(remaining > 0 && tested < tokenList.size()) {
                RealToken& token = tokenList[tok_index];
                if(interval.contains(token.getAge())) {
                    res.push_back(RealToken(token.getAge(), 1));
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
            while(iter != tokenList.end()) {
                double age = iter->getAge();
                if(!interval.contains(age)) {
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
            while(iter != tokenList.rend()) {
                double age = iter->getAge();
                if(!interval.contains(age)) {
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

        std::tuple<RealMarking*, RealMarking*> SMCRunGenerator::fire(TimedTransition* transi) {
            if (transi == nullptr) {
                assert(false);
                return { nullptr, nullptr };
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

            RealMarking* intermediary = new RealMarking(*child);

            for (auto* output : transi->getPostset()) {
                TimedPlace &place = output->getOutputPlace();
                RealToken token = RealToken(0.0, output->getWeight());
                child->addTokenInPlace(place, token);
            }
            for (auto [dest, token] : toCreate) {
                child->addTokenInPlace(dest, token);
            }
            return { intermediary, child };
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