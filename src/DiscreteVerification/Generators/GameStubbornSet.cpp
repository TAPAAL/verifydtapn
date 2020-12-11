/* 
 * File:   GameStubbornSet.cpp
 * Author: Peter G. Jensen
 * 
 * Created on 14 November 2020, 15.36
 */

#include "DiscreteVerification/Generators/GameStubbornSet.h"
#include "DiscreteVerification/Generators/RangeVisitor.h"

namespace VerifyTAPN {
    namespace DiscreteVerification {

        GameStubbornSet::GameStubbornSet(const TimedArcPetriNet& tapn, AST::Query* query)
        : StubbornSet(tapn, query) {
            _safe = std::make_unique<uint8_t[]>(tapn.getTransitions().size());
            _fireing_bounds = std::make_unique<uint32_t[]>(tapn.getTransitions().size());
            _future_enabled = std::make_unique<bool[]>(tapn.getPlaces().size());
            _place_bounds = std::make_unique<std::pair<uint32_t,uint32_t>[]>(tapn.getPlaces().size());
            _places_seen = std::make_unique<uint8_t[]>(tapn.getPlaces().size());
            compute_safe();

        };

        void GameStubbornSet::compute_safe() {
            
            const auto check_production_safety = [this](const TimedPlace& place, const TimedTransition * t, int lower, int upper) {
                TimeInterval iv(false, lower, upper, false);
                for (const TimedInputArc* i : place.getInputArcs()) {
                    if (i->getOutputTransition().isEnvironment() && iv.intersects(i->getInterval())) {
                        _safe[t->getIndex()] = false;
                        return;
                    }
                }
                for (const TransportArc* a : place.getTransportArcs()) {
                    if (a->getTransition().isEnvironment() && iv.intersects(a->getInterval())) {
                        _safe[t->getIndex()] = false;
                        return;
                    }
                }
            };

            const auto check_consumption_safety = [this](const TimedPlace& place, const TimedTransition * trans) {
                for (const InhibitorArc* i : place.getInhibitorArcs()) {
                    if (i->getOutputTransition().isEnvironment()) {
                        _safe[trans->getIndex()] = false;
                        return;
                    }
                }
            };

            std::fill(&_safe[0], &_safe[_tapn.getTransitions().size()], true);

            for (auto trans : _tapn.getTransitions()) {
                if (trans->isEnvironment()) continue;
                for (const OutputArc* a : trans->getPostset()) {
                    check_production_safety(a->getOutputPlace(), trans, 0, 0);
                }
                if (!_safe[trans->getIndex()]) continue;
                for (const TransportArc* a : trans->getTransportArcs()) {
                    auto prod = trans->getProduced(&a->getSource());
                    check_production_safety(a->getSource(), trans, a->getInterval().getLowerBound(), a->getInterval().getUpperBound());

                    if (prod < a->getWeight())
                        check_consumption_safety(a->getDestination(), trans);
                }
                if (!_safe[trans->getIndex()]) continue;
                for (const TimedInputArc* a : trans->getPreset()) {
                    uint32_t prod = trans->getProduced(&a->getInputPlace());
                    if (prod < a->getWeight())
                        check_consumption_safety(a->getInputPlace(), trans);
                }
            }
        }

        bool GameStubbornSet::stubborn_filter(size_t t) const {
            if(_future_enabled[t])
                return true;
            else
                return false;
        }

        void GameStubbornSet::prepare(NonStrictMarkingBase *parent) {            
            _has_env = false;
            _has_ctrl = false;
            _env_trans.clear();
            _ctrl_trans.clear();
            _prepare(parent, [this](auto a) {
                if (a->isControllable()) _has_ctrl = true;
                else _has_env = true;
            },
            [this] {
                if (_has_env && _has_ctrl) return false; // not both!

                if (_has_env) {
                    if(reach())
                        return false;                    
                }
                else compute_future_enabled(true);
                // add all opponents actions
                for(auto* t : _tapn.getTransitions())
                    if(t->isControllable() != _has_ctrl)
                        set_stubborn(t);
                return true;
            });
            

            if(!_can_reduce) return;
            assert(!_has_ctrl || !_has_env);
            
            if (_has_ctrl)
            {
                _ctrl_trans = _enabled_set;
                for(auto ti : _ctrl_trans) {
                    if(!_safe[ti] && is_stubborn(ti))
                    {
                        _can_reduce = false;
                        _ctrl_trans.clear();
                    }
                }
            }
            else
            {
                // we have a few extra conditions here!
                assert(_enabled_set.size() > 0);
                uint32_t candidate = *_enabled_set.begin();
                for(auto t : _enabled_set)
                {
                    if(is_stubborn(t))
                    {
                        candidate = t;
                        break;
                    }
                }
                const TimedTransition* tt = _tapn.getTransitions()[candidate];

                // add incrementers of inhib origins
                for(const InhibitorArc* inhib : tt->getInhibitorArcs())
                {
                    for(const OutputArc* incr : inhib->getInputPlace().getOutputArcs())
                    {
                        if(incr->getWeight() > incr->getInputTransition().getConsumed(&incr->getOutputPlace()))
                            set_stubborn(incr->getInputTransition());
                    }
                    for(const TransportArc* incr : inhib->getInputPlace().getProdTransportArcs())
                    {
                        if(incr->getWeight() > incr->getTransition().getConsumed(&incr->getSource()))
                            set_stubborn(incr->getTransition());
                    }
                }

                // add pre-post-sets that intersect with current marking
                // still missing intersection with guard
                for (auto* a : tt->getPreset())
                    if(a->getWeight() <= _parent->numberOfTokensInPlace(a->getInputPlace().getIndex()))
                        postset_of(a->getInputPlace().getIndex(), a->getInterval());
                for (auto* a : tt->getTransportArcs())
                    if(a->getWeight() <= _parent->numberOfTokensInPlace(a->getSource().getIndex()))
                        postset_of(a->getSource().getIndex(), a->getInterval());

                // need to recompute closure
                compute_closure();

                _env_trans = _enabled_set;
            }
        }

        bool GameStubbornSet::urgent_priority(const TimedTransition* urg_trans, const TimedTransition* trans) const
        {
            if(urg_trans == nullptr) return true;
            return _safe[trans->getIndex()] && !_safe[urg_trans->getIndex()];
        }

        bool GameStubbornSet::zt_priority(const TimedTransition* trans, const TimedPlace* inv_place) const {
            if(trans && (_safe[trans->getIndex()] || inv_place == nullptr))
                return true;
            else
                return false;
        }

        const TimedTransition* GameStubbornSet::pop_next(bool controllable) {
            auto& queue = controllable ? _ctrl_trans : _env_trans;
            while(!queue.empty())
            {                
                auto el = queue.front();
                queue.pop_front();
                if(!is_stubborn(el)) continue;
                return _tapn.getTransitions()[el];
            }
            return nullptr;
        }

        bool GameStubbornSet::reach() {
            constexpr auto inf = std::numeric_limits<uint32_t>::max();
            std::vector<uint32_t> waiting;
            compute_future_enabled(false);
            const auto handle_transition = [this, &waiting](const TimedTransition * trans) {
                const auto t = trans->getIndex();
                if (trans->isControllable()) return;
                if (!_future_enabled[trans->getIndex()]) return;
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

            const auto handle_place = [this, &handle_transition](size_t p) {
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
                    if (a->getInputTransition().isControllable()) continue;
                    auto cons = a->getInputTransition().getConsumed(place);
                    if (cons >= a->getWeight()) continue;
                    sum += (a->getWeight() - cons) * _fireing_bounds[a->getInputTransition().getIndex()];
                }

                for (auto a : place->getProdTransportArcs()) {
                    if (&a->getSource() == &a->getDestination()) continue;
                    auto bounds = _fireing_bounds[a->getTransition().getIndex()];
                    if (bounds == inf) return;
                    if (bounds == 0) continue;
                    if (a->getTransition().isControllable()) continue;
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
                if(t->isControllable()) continue;
                if(_future_enabled[t->getIndex()])
                {
                    _fireing_bounds[t->getIndex()] = inf;
                    handle_transition(t);
                }
                else
                    _fireing_bounds[t->getIndex()] = 0;
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
                    if (a->getInputTransition().isControllable()) continue;

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
            RangeVisitor visitor(_tapn, *_parent, _place_bounds.get());
            IntResult context; // -1 false, 0 = unknown, 1 = true
            _query->accept(visitor, context);
            if(context.value == 0) return true;
            else {
#ifndef NDEBUG
                BoolResult context;
                _query->accept(visitor, context);
                assert(context.value || context.value == -1);
                assert(!context.value || context.value == 1);
#endif
                return false;
            }
        }

        void GameStubbornSet::compute_future_enabled(bool controllable)
        {
            std::fill(&_future_enabled[0], &_future_enabled[_tapn.getPlaces().size()], false);
            std::stack<uint32_t> waiting;

            auto color_transition = [this,&waiting](const TimedTransition* trans)
            {
                if(_future_enabled[trans->getIndex()]) return;
                _future_enabled[trans->getIndex()] = true;
                {
                    // check for decrementors
                    for(TimedInputArc* arc : trans->getPreset())
                    {
                        if(trans->getProduced(&arc->getInputPlace()) < arc->getWeight())
                        {
                            auto pid = arc->getInputPlace().getIndex();
                            if((_places_seen[pid] & DECR) == 0)
                                waiting.push(pid);
                            _places_seen[pid] |= DECR;
                        }
                    }
                    for(TransportArc* arc : trans->getTransportArcs())
                    {
                        if(trans->getProduced(&arc->getSource()) < arc->getWeight())
                        {
                            auto pid = arc->getSource().getIndex();
                            if((_places_seen[pid] & DECR) == 0)
                                waiting.push(pid);
                            _places_seen[pid] |= DECR;
                        }
                    }
                }
                {
                    // color incrementors
                    for(OutputArc* arc : trans->getPostset())
                    {
                        auto id = arc->getOutputPlace().getIndex();
                        if((_places_seen[id] & INCR) ==  0)
                            waiting.push(id);
                        _places_seen[id] |= INCR;
                    }

                    for(TransportArc* arc : trans->getTransportArcs())
                    {
                        auto id = arc->getDestination().getIndex();
                        if((_places_seen[id] & INCR) ==  0)
                            waiting.push(id);
                        _places_seen[id] |= INCR;
                    }
                }
            };

            const auto check_enable_transition = [this,&waiting,&color_transition,controllable](const TimedTransition& trans)
            {
                if(trans.isControllable() == controllable) return;
                for(auto* inhib : trans.getInhibitorArcs())
                {
                    auto pid = inhib->getInputPlace().getIndex();
                    if((_places_seen[pid] & DECR)) continue;
                    if(_parent->numberOfTokensInPlace(pid) < inhib->getWeight()) continue;
                    return; // no proof that it will be uninhibited
                }

                for(auto* input : trans.getPreset()) {
                    auto pid = input->getInputPlace().getIndex();
                    if((_places_seen[pid] & INCR)) continue;
                    if(_parent->numberOfTokensInPlace(pid) >= input->getWeight()) continue;
                    return; // no proof that it will ever be enabled
                }

                for(auto* input : trans.getTransportArcs()) {
                    auto pid = input->getSource().getIndex();
                    if((_places_seen[pid] & INCR)) continue;
                    if(_parent->numberOfTokensInPlace(pid) >= input->getWeight()) continue;
                    return; // no proof that it will ever be enabled
                }
                color_transition(&trans);
            };

            // bootstrap
            for(auto& t : _tapn.getTransitions())
                if(t->isControllable() == controllable && is_enabled(t->getIndex()))
                    color_transition(t);

            // saturate
            while(!waiting.empty())
            {
                auto p = waiting.top();
                waiting.pop();
                if((_places_seen[p] & DECR) != 0)
                {
                    for(auto* inhib : _tapn.getPlace(p).getInhibitorArcs())
                        check_enable_transition(inhib->getOutputTransition());
                }
                if((_places_seen[p] & INCR) != 0)
                {
                    for(auto* input : _tapn.getPlace(p).getInputArcs())
                        check_enable_transition(input->getOutputTransition());
                    for(auto* input : _tapn.getPlace(p).getTransportArcs())
                        check_enable_transition(input->getTransition());
                }
            }
        }

    }
}

