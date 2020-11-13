/* 
 * File:   NextEnabledGenerator.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 13 November 2020, 10.15
 */

#include "DiscreteVerification/Generators/NextEnabledGenerator.h"
#include <cassert>

namespace VerifyTAPN {
    namespace DiscreteVerification {
        using namespace TAPN;

        NextEnabledGenerator::NextEnabledGenerator(const TimedArcPetriNet& tapn)
        : _tapn(tapn), _max_tokens(0) {
            for (auto* transition : tapn.getTransitions()) {

                if (transition->getPreset().size() + transition->getTransportArcs().size() == 0) {
                    _allways_enabled.push_back(transition);
                } else {
                    size_t tokens = 0;
                    size_t index = std::numeric_limits<size_t>::max();
                    for (auto* arc : transition->getPreset()) {
                        index = std::min((size_t)arc->getInputPlace().getIndex(), index);
                        tokens += arc->getWeight();
                    }

                    for (auto* arc : transition->getTransportArcs()) {
                        index = std::min((size_t)arc->getSource().getIndex(), index);
                        tokens += arc->getWeight();
                    }
                    while (index >= _place_transition.size()) _place_transition.emplace_back();
                    _place_transition[index].push_back(transition);
                    _max_tokens = std::max(tokens,
                            _max_tokens);
                }
            }
        }

        void NextEnabledGenerator::prepare(const NonStrictMarkingBase* marking) {
            _parent = marking;
            _did_noinput = false;
            _place = 0;
            _transition = 0;
        }

        std::pair<const TimedTransition*, bool>
        NextEnabledGenerator::next_transition(std::vector<size_t>* permutations, std::function<bool(const TimedTransition*)> filter) {
            if (!_did_noinput) {
                while (_transition < _allways_enabled.size()) {
                    auto trans = _allways_enabled[_transition];
                    ++_transition; // increment for next time!
                    if (!filter(trans) || is_inhibited(trans)) continue;
                    return std::make_pair(trans, false);
                }
                _did_noinput = true;
                _transition = 0;
            }

            do {

                if(_place >= _parent->getPlaceList().size()) return std::make_pair(nullptr, false);
                size_t placeindex = _parent->getPlaceList()[_place].place->getIndex();
                if (_place_transition.size() <= placeindex ||
                        _transition >= _place_transition[placeindex].size()) {
                    ++_place;
                    _transition = 0;
                }

                // no more places with tokens, we are done
                if (_place >= _parent->getPlaceList().size())
                    return std::make_pair(nullptr, false);

                placeindex = _parent->getPlaceList()[_place].place->getIndex();
                // no more interesting places
                if (_place_transition.size() <= placeindex)
                    return std::make_pair(nullptr, false);

                // no out transitions
                if (_place_transition[placeindex].empty()) {
                    ++_place;
                    _transition = 0;
                    continue;
                }

                auto trans = _place_transition[placeindex][_transition];
                ++_transition;
                if (filter(trans) && is_enabled(trans, permutations))
                    return std::make_pair(trans, true);
            } while (true);
            assert(false);
        }

        const InhibitorArc* NextEnabledGenerator::is_inhibited(const TimedTransition *trans) const {
            auto &placelist = _parent->getPlaceList();
            auto pit = placelist.begin();

            for (auto* inhib : trans->getInhibitorArcs()) {
                while (pit != placelist.end() &&
                        inhib->getInputPlace().getIndex() > pit->place->getIndex())
                    ++pit;

                int tokens = inhib->getWeight();
                if (pit != placelist.end() &&
                        inhib->getInputPlace().getIndex() == pit->place->getIndex()) {
                    for (auto &t : pit->tokens)
                        tokens -= t.getCount();
                }
                if (tokens <= 0) return inhib;
            }
            return nullptr;
        }

        bool NextEnabledGenerator::is_enabled(const TimedTransition *trans, std::vector<size_t> *permutations) const {

            if (is_inhibited(trans)) return false;

            auto missing_tokens = compute_missing(trans, permutations);
            if (missing_tokens) return false;
            return true;
        }

        const TimedPlace *NextEnabledGenerator::compute_missing(const TimedTransition *trans, std::vector<size_t> *permutations) const {
            auto& placelist = _parent->getPlaceList();

            size_t arccounter = 0;
            auto pit = placelist.begin();
            for (auto &input : trans->getPreset()) {
                int source = input->getInputPlace().getIndex();
                while (pit != placelist.end() &&
                        pit->place->getIndex() < source)
                    ++pit;

                if (pit == placelist.end() || pit->place->getIndex() != source)
                    return &input->getInputPlace();

                int weight = input->getWeight();
                auto &tokenlist = pit->tokens;
                for (size_t index = 0; index < tokenlist.size(); ++index) {
                    auto &token = tokenlist[index];
                    if (input->getInterval().contains(token.getAge())) {
                        if (permutations) {
                            int n_tokens = std::min(weight, token.getCount());
                            for (int i = 0; i < n_tokens; ++i) {
                                (*permutations)[arccounter] = index;
                                ++arccounter;
                            }
                        }
                        weight -= token.getCount();
                        if (weight < 0) break;
                    }
                }
                if (weight > 0) return &input->getInputPlace();
            }

            pit = placelist.begin();
            for (auto* transport : trans->getTransportArcs()) {
                int source = transport->getSource().getIndex();
                while (pit != placelist.end() &&
                        pit->place->getIndex() < source)
                    ++pit;

                if (pit == placelist.end() || pit->place->getIndex() != source)
                    return &transport->getSource();

                int weight = transport->getWeight();
                auto &tokenlist = pit->tokens;
                for (size_t index = 0; index < tokenlist.size(); ++index) {
                    auto &token = tokenlist[index];
                    if (transport->getInterval().contains(token.getAge())) {
                        if (permutations) {
                            int n_tokens = std::min(weight, token.getCount());
                            for (int i = 0; i < n_tokens; ++i) {
                                (*permutations)[arccounter] = index;
                                ++arccounter;
                            }
                        }
                        weight -= token.getCount();
                        if (weight < 0) break;
                    }
                }
                if (weight > 0) return &transport->getSource();
            }
            return nullptr;
        }
    }

}

