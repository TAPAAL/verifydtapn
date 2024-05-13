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

        void SMCRunGenerator::prepare(NonStrictMarkingBase *parent) {
            _origin = parent;
            _parent = _origin;
            PlaceList& places = _origin->getPlaceList();
            std::vector<bool> transitionSeen(_defaultTransitionIntervals.size(), false);
            _originMaxDelay = std::numeric_limits<int>::max();
            for(auto &pit : places) {
                if(pit.place->getInvariant().getBound() != std::numeric_limits<int>::max()) {
                    int place_max_delay = pit.place->getInvariant().getBound() - pit.maxTokenAge();
                    if(place_max_delay < _originMaxDelay) {
                        _originMaxDelay = place_max_delay;
                    }
                }
                for(auto arc : pit.place->getInputArcs()) {
                    TimedTransition &transi = arc->getOutputTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval> firingDates = transitionFiringDates(&transi);
                    _defaultTransitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
                for(auto arc : pit.place->getTransportArcs()) {
                    TimedTransition &transi = arc->getTransition();
                    if(transitionSeen[transi.getIndex()]) continue;
                    std::vector<interval> firingDates = transitionFiringDates(&transi);
                    _defaultTransitionIntervals[transi.getIndex()] = firingDates;
                    transitionSeen[transi.getIndex()] = true;
                }
            }
            for(auto &transi : _tapn.getTransitions()) {
                if(transi->getPresetSize() == 0) {
                    _defaultTransitionIntervals[transi->getIndex()] = { interval(0, std::numeric_limits<int>::max()) };
                    transitionSeen[transi->getIndex()] = true;
                }
            }
            std::vector<interval> invInterval = { interval(0, _originMaxDelay) };
            for(auto iter = _defaultTransitionIntervals.begin() ; iter != _defaultTransitionIntervals.end() ; iter++) {
                if(iter->empty()) continue;
                *iter = Util::setIntersection(*iter, invInterval);
            }
            reset();
        }

        void SMCRunGenerator::reset() {
            _parent = new NonStrictMarkingBase(*_origin);
            _transitionIntervals = _defaultTransitionIntervals;
            _maximal = false;
            _max_delay = _originMaxDelay;
            _totalTime = 0;
            _totalSteps = 0;
            _modifiedPlaces.clear();
        } 

        void SMCRunGenerator::refreshTransitionsIntervals() {
            PlaceList& places = _parent->getPlaceList();
            std::vector<bool> transitionSeen(_transitionIntervals.size(), false);
            _max_delay = std::numeric_limits<int>::max();
            for(auto &pit : places) {
                if(pit.place->getInvariant().getBound() != std::numeric_limits<int>::max()
                    && pit.tokens.size() > 0) {
                    int place_max_delay = pit.place->getInvariant().getBound() - pit.maxTokenAge();
                    if(place_max_delay < _max_delay) {
                        _max_delay = place_max_delay;
                    }
                }
                auto placePos = std::find(_modifiedPlaces.begin(), _modifiedPlaces.end(), pit.place->getIndex());
                if(placePos != _modifiedPlaces.end()) {
                    _modifiedPlaces.erase(placePos);
                    for(auto arc : pit.place->getInputArcs()) {
                        TimedTransition &transi = arc->getOutputTransition();
                        if(transitionSeen[transi.getIndex()]) continue;
                        std::vector<interval> firingDates = transitionFiringDates(&transi);
                        _transitionIntervals[transi.getIndex()] = firingDates;
                        transitionSeen[transi.getIndex()] = true;
                    }
                    for(auto arc : pit.place->getInhibitorArcs()) {
                        TimedTransition &transi = arc->getOutputTransition();
                        if(transitionSeen[transi.getIndex()]) continue;
                        std::vector<interval> firingDates = transitionFiringDates(&transi);
                        _transitionIntervals[transi.getIndex()] = firingDates;
                        transitionSeen[transi.getIndex()] = true;
                    }
                    for(auto arc : pit.place->getTransportArcs()) {
                        TimedTransition &transi = arc->getTransition();
                        if(transitionSeen[transi.getIndex()]) continue;
                        std::vector<interval> firingDates = transitionFiringDates(&transi);
                        _transitionIntervals[transi.getIndex()] = firingDates;
                        transitionSeen[transi.getIndex()] = true;
                    }
                }
            }
            for(auto modified : _modifiedPlaces) { // Cleanup disabled transitions
                TimedPlace place = _tapn.getPlace(modified);
                for(auto arc : place.getInputArcs()) {
                    int transiIndex = arc->getOutputTransition().getIndex();
                    if(transitionSeen[transiIndex]) continue;
                    _transitionIntervals[transiIndex].clear();
                }
                for(auto arc : place.getTransportArcs()) {
                    int transiIndex = arc->getTransition().getIndex();
                    if(transitionSeen[transiIndex]) continue;
                    _transitionIntervals[transiIndex].clear();
                }
            }
            std::vector<interval> invInterval = { interval(0, _max_delay) };
            for(auto iter = _transitionIntervals.begin() ; iter != _transitionIntervals.end() ; iter++) {
                if(iter->empty()) continue;
                *iter = Util::setIntersection(*iter, invInterval);
            }
        }

        NonStrictMarkingBase* SMCRunGenerator::next() {
            auto [transi, delay] = getWinnerTransitionAndDelay();
            
            if(delay == std::numeric_limits<int>::max()) {
                _maximal = true;
                return nullptr;
            }

            _parent->incrementAge(delay);
            _totalTime += delay;
            _modifiedPlaces.clear();
        
            if(transi != nullptr) {
                _totalSteps++;
                _transitionsStatistics[transi->getIndex()]++;
                NonStrictMarkingBase* child = fire(transi);
                delete _parent;
                _parent = child;
            }

            // Translate intervals, so we don't have to compute some of them next
            for(auto iter = _transitionIntervals.begin() ; iter != _transitionIntervals.end() ; iter++) {
                Util::setDeltaIntoPositive(*iter, -delay);
            }
            refreshTransitionsIntervals();

            return _parent;
        }

        std::pair<TimedTransition*, int> SMCRunGenerator::getWinnerTransitionAndDelay() {
            std::vector<size_t> winner_indexs;
            size_t transi_index = 0;
            int date_min = std::numeric_limits<int>::max();
            std::random_device rd; 
            std::mt19937 gen(rd());
            for(auto iter = _transitionIntervals.begin() ; iter != _transitionIntervals.end() ; iter++) {
                if(iter->empty()) {
                    transi_index++;
                    continue;
                }
                int date;
                TimedTransition* transition = _tapn.getTransitions()[transi_index];
                int max_accessible = _semantics == Weak ? _max_delay : std::min(Util::setLength(*iter), _max_delay);
                if(transition->isUrgent()) {
                    date = iter->front().lower();
                } else if(transition->hasCustomProbabilityRate()) {
                    std::geometric_distribution<> distrib(transition->getProbabilityRate());
                    date = std::clamp(distrib(gen), 0, max_accessible);
                } else if(max_accessible == std::numeric_limits<int>::max()) {
                    std::geometric_distribution<> distrib(_defaultRate);
                    date = distrib(gen);
                } else {
                    std::uniform_int_distribution<> distrib(0, max_accessible); 
                    date = distrib(gen);
                }
                if(_semantics == Strong) {
                    date = Util::valueAt(*iter, date);
                }
                if(date < date_min) {
                    date_min = date;
                    winner_indexs.clear();
                }
                if(date == date_min && Util::contains(*iter, date)) {
                    winner_indexs.push_back(transi_index);
                }
                transi_index++;
            }
            if(date_min == std::numeric_limits<int>::max()) {
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

        std::vector<interval> SMCRunGenerator::transitionFiringDates(TimedTransition* transi) {
            PlaceList &places = _parent->getPlaceList();
            std::vector<interval> firingInterval = { interval(0, std::numeric_limits<int>::max()) };
            std::vector<interval> disabled;
            for(InhibitorArc* inhib : transi->getInhibitorArcs()) {
                if(_parent->numberOfTokensInPlace(inhib->getInputPlace().getIndex()) >= inhib->getWeight()) {
                    return disabled;
                } 
            }
            for(TimedInputArc* arc : transi->getPreset()) {
                auto pit = places.begin();
                while(pit != places.end() && pit->place->getIndex() != arc->getInputPlace().getIndex()) {
                    pit++;
                }
                if(pit == places.end()) return disabled;
                firingInterval = Util::setIntersection(firingInterval, arcFiringDates(arc->getInterval(), arc->getWeight(), pit->tokens));
                if(firingInterval.empty()) return firingInterval;
            }
            for(TransportArc* arc : transi->getTransportArcs()) {
                auto pit = places.begin();
                while(pit != places.end() && pit->place->getIndex() != arc->getSource().getIndex()) {
                    ++pit;
                }
                if(pit == places.end()) return disabled;
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

        std::vector<interval> SMCRunGenerator::arcFiringDates(TimeInterval time_interval, uint32_t weight, TokenList& tokens) {
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
        }

        Util::interval SMCRunGenerator::remainingForToken(const interval& arcInterval, const Token& token) {
            interval tokenInterv = arcInterval;
            tokenInterv.delta(-token.getAge());
            return tokenInterv.positive();
        }

        NonStrictMarkingBase* SMCRunGenerator::fire(TimedTransition* transi) {
            if (transi == nullptr) {
                assert(false);
                return nullptr;
            }
            std::random_device rd; 
            std::mt19937 gen(rd());
            auto *child = new NonStrictMarkingBase(*_parent);
            child->setGeneratedBy(nullptr);
            child->setParent(_parent);
            PlaceList &placelist = child->getPlaceList();

            auto pit = placelist.begin();
            for (auto &input : transi->getPreset()) {
                int source = input->getInputPlace().getIndex();
                while (pit->place->getIndex() != source) {
                    ++pit;
                    assert(pit != placelist.end());
                }
                TokenList& tokenlist = pit->tokens;
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
                if(tokenlist.size() == 0) pit = placelist.erase(pit);
            }

            std::vector<std::pair<TransportArc*, int>> toCreate;
            pit = placelist.begin();
            for (auto &transport : transi->getTransportArcs()) {
                int source = transport->getSource().getIndex();
                int destInv = transport->getDestination().getInvariant().getBound();
                while (pit->place->getIndex() != source) {
                    ++pit;
                    assert(pit != placelist.end());
                }
                TokenList& tokenlist = pit->tokens;
                int remaining = transport->getWeight();
                std::uniform_int_distribution<> randomTokenIndex(0, tokenlist.size() - 1);
                size_t tok_index = randomTokenIndex(gen);
                size_t tested = 0;
                while(remaining > 0 && tested < tokenlist.size()) {
                    int age = tokenlist[tok_index].getAge();
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
                        toCreate.push_back(std::make_pair(transport, age));
                    } else {
                        tok_index = (tok_index + 1) % tokenlist.size();
                        tested++;
                    }
                }
                assert(remaining == 0);
                _modifiedPlaces.push_back(source);
                if(tokenlist.size() == 0) pit = placelist.erase(pit);
            }
            for (auto* output : transi->getPostset()) {
                TimedPlace &place = output->getOutputPlace();
                Token token = Token(0, output->getWeight());
                child->addTokenInPlace(place, token);
                _modifiedPlaces.push_back(place.getIndex());
            }
            for (auto [transportArc, age] : toCreate) {
                TimedPlace &place = transportArc->getDestination();
                child->addTokenInPlace(place, age);
                _modifiedPlaces.push_back(place.getIndex());
            }
            return child;
        }

        bool SMCRunGenerator::reachedEnd() const {
            return _maximal;
        }

        int SMCRunGenerator::getRunDelay() const {
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

        void SMCRunGenerator::setSemantics(SMCSemantics semantics) {
            _semantics = semantics;
        }

        SMCSemantics SMCRunGenerator::getSemantics() const {
            return _semantics;
        }

    }
}