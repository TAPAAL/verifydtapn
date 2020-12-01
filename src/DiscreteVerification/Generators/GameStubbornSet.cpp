/* 
 * File:   GameStubbornSet.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 14 November 2020, 15.36
 */

#include "DiscreteVerification/Generators/GameStubbornSet.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        GameStubbornSet::GameStubbornSet(const TimedArcPetriNet& tapn, AST::Query* query)
        : StubbornSet(tapn, query) {
            compute_safe();
            _safe = std::make_unique<uint8_t[]>(tapn.getTransitions().size());
            _fireing_bounds = std::make_unique<uint32_t[]>(tapn.getTransitions().size());
            _place_bounds = std::make_unique<std::pair<uint32_t,uint32_t>[]>(tapn.getPlaces().size());
            _places_seen = std::make_unique<uint8_t[]>(tapn.getPlaces().size());
        };

        void GameStubbornSet::compute_safe() {

            auto check_production_safety = [this](const TimedPlace& place, const TimedTransition * t) {
                for (const TimedInputArc* i : place.getInputArcs()) {
                    if (i->getOutputTransition().isEnvironment()) {
                        // _stub_enable[t] |= UNSAFE;
                        return;
                    }
                }
                for (const TransportArc* a : place.getTransportArcs()) {
                    if (a->getTransition().isEnvironment()) {
                        // _stub_enable[t] |= UNSAFE;
                        return;
                    }
                }
            };

            auto check_consumption_safety = [this](const TimedPlace& place, const TimedTransition * trans) {
                for (const InhibitorArc* i : place.getInhibitorArcs()) {
                    if (i->getOutputTransition().isEnvironment()) {
                        //_stub_enable[t] |= UNSAFE;
                        return;
                    }
                }
            };

            for (auto trans : _tapn.getTransitions()) {
                if (trans->isEnvironment()) continue;
                for (const OutputArc* a : trans->getPostset()) {
                    auto cons = trans->getConsumed(&a->getOutputPlace());
                    if (cons >= a->getWeight()) continue;
                    check_production_safety(a->getOutputPlace(), trans);
                }
                //if (_stub_enable[t] & UNSAFE) continue;
                for (const TransportArc* a : trans->getTransportArcs()) {
                    auto cons = trans->getConsumed(&a->getDestination());
                    auto prod = trans->getProduced(&a->getSource());
                    if (cons < a->getWeight())
                        check_production_safety(a->getSource(), trans);

                    if (prod < a->getWeight())
                        check_consumption_safety(a->getDestination(), trans);
                }
                //if (_stub_enable[t] & UNSAFE) continue;
                for (const TimedInputArc* a : trans->getPreset()) {
                    uint32_t prod = trans->getProduced(&a->getInputPlace());
                    if (prod < a->getWeight())
                        check_consumption_safety(a->getInputPlace(), trans);
                }
            }

            for (const TimedTransition* t : _tapn.getTransitions()) {
                if (t->isEnvironment()) continue;
                    //if (_stub_enable[t->getIndex()] & UNSAFE) std::cerr << t->getName() << " UNSAFE" << std::endl;
                else std::cerr << t->getName() << " " << std::endl;
            }
        }

        void GameStubbornSet::prepare(NonStrictMarkingBase *parent) {
            bool has_env = false;
            bool has_ctrl = false;
            _env_trans.clear();
            _ctrl_trans.clear();

            return _prepare(parent, [&has_env, &has_ctrl](auto a) {
                if (a->isControllable()) has_ctrl = true;
                else has_env = true;
            },
            [this, &has_env, &has_ctrl] {
                if (has_env && has_ctrl) return false; // not both!
                return true;

                if (has_env) {
                    if(reach() > 0) return false; // environment can change outcome
                }
                // add all opponents actions
                for(auto* t : _tapn.getTransitions())
                    if(t->isControllable() != has_ctrl)
                        set_stubborn(t);
                // check if its unsafe!
            });
            // we need to check for safety!
            assert(!has_ctrl || !has_env);
            if (has_ctrl)
                _ctrl_trans = _enabled_set;
            else
                _env_trans = _enabled_set;
        }

        const TimedTransition* GameStubbornSet::pop_next(bool controllable) {
            auto& queue = controllable ? _ctrl_trans : _env_trans;
            if (queue.empty()) return nullptr;
            else {
                auto el = queue.front();
                queue.pop_front();
                return _tapn.getTransitions()[el];
            }
        }

        int GameStubbornSet::reach() {
            constexpr auto inf = std::numeric_limits<uint32_t>::max();
            std::vector<uint32_t> waiting;
            auto handle_transition = [this, &waiting](const TimedTransition * trans) {
                const auto t = trans->getIndex();
                if (trans->isEnvironment()) return;
                /*if((_stub_enable[t] & FUTURE_ENABLED) == 0)
                    return;*/
                auto mx = inf;
                for (const TransportArc* ta : trans->getTransportArcs()) {
                    if (&ta->getSource() == &ta->getDestination()) continue;
                    if (_place_bounds[ta->getSource().getIndex()].second == inf) continue;
                    auto produced = trans->getProduced(&ta->getSource());
                    if (produced >= ta->getWeight()) continue; // not decreasing
                    mx = std::min(mx, (uint32_t) _place_bounds[ta->getSource().getIndex()].second / (ta->getWeight() - produced));
                }

                for (const TimedInputArc* ta : trans->getPreset()) {
                    if (_place_bounds[ta->getInputPlace().getIndex()].second == inf) continue;
                    auto produced = trans->getProduced(&ta->getInputPlace());
                    if (produced >= ta->getWeight()) continue; // not decreasing
                    mx = std::min(mx, (uint32_t) _place_bounds[ta->getInputPlace().getIndex()].second / (ta->getWeight() - produced));
                }
                assert(mx <= _fireing_bounds[t]);
                if (_fireing_bounds[t] != mx) {
                    _fireing_bounds[t] = mx;
                    for (const TransportArc* ta : trans->getTransportArcs()) {
                        if (&ta->getSource() == &ta->getDestination()) continue;
                        auto cons = trans->getConsumed(&ta->getDestination());
                        if (cons >= ta->getWeight()) continue;
                        auto pid = ta->getDestination().getIndex();
                        _places_seen[pid] |= WAITING;
                        waiting.push_back(pid);
                    }
                    for (const OutputArc* ta : trans->getPostset()) {
                        auto cons = trans->getConsumed(&ta->getOutputPlace());
                        if (cons >= ta->getWeight()) continue;
                        auto pid = ta->getOutputPlace().getIndex();
                        _places_seen[pid] |= WAITING;
                        waiting.push_back(pid);
                    }
                }
            };

            auto handle_place = [this, &handle_transition](size_t p) {
                if (_place_bounds[p].second == 0)
                    return;
                _places_seen[p] &= ~WAITING;

                // place loop
                uint64_t sum = 0;
                const TimedPlace* place = _tapn.getPlaces()[p];
                for (auto a : place->getOutputArcs()) {
                    auto bounds = _fireing_bounds[a->getInputTransition().getIndex()];
                    if (bounds == inf) return;
                    if (bounds == 0) continue;
                    if (a->getInputTransition().isEnvironment()) continue;
                    auto cons = a->getInputTransition().getConsumed(place);
                    if (cons >= a->getWeight()) continue;
                    sum += (a->getWeight() - cons) * _fireing_bounds[a->getInputTransition().getIndex()];
                }

                for (auto a : place->getProdTransportArcs()) {
                    if (&a->getSource() == &a->getDestination()) continue;
                    auto bounds = _fireing_bounds[a->getTransition().getIndex()];
                    if (bounds == inf) return;
                    if (bounds == 0) continue;
                    if (a->getTransition().isEnvironment()) continue;
                    auto cons = a->getTransition().getConsumed(place);
                    if (cons >= a->getWeight()) continue;
                    sum += (a->getWeight() - cons) * _fireing_bounds[a->getTransition().getIndex()];
                }

                assert(sum <= _place_bounds[p].second);
                if (_place_bounds[p].second != sum) {
                    _place_bounds[p].second = sum;
                    for (auto a : place->getInputArcs())
                        handle_transition(&a->getOutputTransition());
                    for (auto a : place->getTransportArcs())
                        handle_transition(&a->getTransition());
                }

            };

            // initialize places
            for (size_t p = 0; p < _tapn.getPlaces().size(); ++p) {
                auto ub = inf;
                bool all_neg = true;
                for (const OutputArc* arc : _tapn.getPlaces()[p]->getOutputArcs()) {
                    if (arc->getInputTransition().isControllable()) continue;
                    auto cons = arc->getInputTransition().getConsumed(_tapn.getPlaces()[p]);
                    if (cons < arc->getWeight()) {
                        all_neg = false;
                        break;
                    }
                }
                if (!all_neg) {
                    for (const TransportArc* arc : _tapn.getPlaces()[p]->getProdTransportArcs()) {
                        if (arc->getTransition().isControllable()) continue;
                        auto cons = arc->getTransition().getConsumed(_tapn.getPlaces()[p]);
                        if (cons < arc->getWeight()) {
                            all_neg = false;
                            break;
                        }
                    }
                }
                if (all_neg) {
                    ub = _parent->numberOfTokensInPlace(p);
                }
                _place_bounds[p] = std::make_pair(inf, ub);
            }
            // initialize counters
            for (auto& t : _tapn.getTransitions()) {
                if(t->isEnvironment()) continue;
                //if(_stub_enable[t] & FUTURE_ENABLED)
                {
                    _fireing_bounds[t->getIndex()] = inf;
                    handle_transition(t);
                }
                /*else
                    _fireing_bounds[t] = 0;*/
            }

            while (!waiting.empty()) {
                auto p = waiting.back();
                waiting.pop_back();
                handle_place(p);
            }

            for (auto* p : _tapn.getPlaces()) {
                bool done = false;
                auto& pb = _place_bounds[p->getIndex()];
                for (auto a : p->getOutputArcs()) {
                    if (a->getInputTransition().isEnvironment()) continue;

                    auto bound = _fireing_bounds[a->getInputTransition().getIndex()];
                    auto cons = a->getInputTransition().getConsumed(p);
                    auto take = a->getWeight() - cons;

                    if (cons <= a->getWeight()) continue;
                    if (bound == inf) {
                        done = true;
                        break;
                    }
                    if (take * bound >= pb.first) {
                        done = true;
                        break;
                    }
                    pb.first -= (take * bound);
                }
                if (!done) for (auto* a : p->getProdTransportArcs()) {
                        if (a->getTransition().isControllable()) continue;

                        auto bound = _fireing_bounds[a->getTransition().getIndex()];
                        auto cons = a->getTransition().getConsumed(p);
                        auto take = a->getWeight() - cons;

                        if (cons <= a->getWeight()) continue;
                        if (bound == inf) {
                            done = true;
                            break;
                        }
                        if (take * bound >= pb.first) {
                            done = true;
                            break;
                        }
                        pb.first -= (take * bound);
                    }
                if (done) pb.first = 0;
            }
            /*RangeVisitor visitor(*_parent, _tapn);
            IntResult context; // -1 false, 0 = unknown, 1 = true
            query->accept(visitor, context);*/
            //return context.value;
            return 0;
        }
    }
}

