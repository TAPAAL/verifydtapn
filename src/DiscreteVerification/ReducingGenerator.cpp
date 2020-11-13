/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ReducingGenerator.cpp
 * Author: Peter G. Jensen
 * 
 * Created on December 14, 2017, 8:44 PM
 */

#include "DiscreteVerification/ReducingGenerator.hpp"
#include "DiscreteVerification/QueryVisitor.hpp"


namespace VerifyTAPN { namespace DiscreteVerification {

    bool ReducingGenerator::preSetOf(size_t i) {
        auto place = tapn.getPlaces()[i];
        bool zt = false;
        for (auto* arc : place->getOutputArcs()) {
            auto t = arc->getInputTransition().getIndex();
            assert(t < tapn.getTransitions().size());
            if (!_stubborn[t]) _unprocessed.push_back(t);
            zt |= _enabled[t] && tapn.getTransitions()[t]->isUrgent();
            _stubborn[t] = true;
        }
        for (auto* arc : place->getProdTransportArcs()) {
            auto t = arc->getTransition().getIndex();
            assert(t < tapn.getTransitions().size());
            if (!_stubborn[t]) _unprocessed.push_back(t);
            zt |= _enabled[t] && tapn.getTransitions()[t]->isUrgent();
            _stubborn[t] = true;
        }
        return zt;
    }

    bool ReducingGenerator::postSetOf(size_t i, bool check_age, const TAPN::TimeInterval &interval) {
        auto place = tapn.getPlaces()[i];
        bool zt = false;
        if (check_age) {
            auto &tl = parent->getTokenList(place->getIndex());
            if (!tl.empty()) {
                if (place->getInvariant().getBound() == tl.back().getAge())
                    zt = true;
            }
        }
        for (auto* arc : place->getInputArcs()) {
            auto t = arc->getOutputTransition().getIndex();
            assert(t < tapn.getTransitions().size());
            if (!interval.intersects(arc->getInterval())) continue;
            if (!_stubborn[t]) _unprocessed.push_back(t);
            zt |= _enabled[t] && tapn.getTransitions()[t]->isUrgent();
            _stubborn[t] = true;
        }

        for (auto* arc : place->getTransportArcs()) {
            if (&arc->getSource() == &arc->getDestination()) continue;
            auto t = arc->getTransition().getIndex();
            assert(t < tapn.getTransitions().size());
            if (!interval.intersects(arc->getInterval())) continue;
            if (!_stubborn[t]) _unprocessed.push_back(t);
            zt |= _enabled[t] && tapn.getTransitions()[t]->isUrgent();
            _stubborn[t] = true;
        }
        return zt;
    }

    bool ReducingGenerator::inhibPostSetOf(size_t i) {
        bool zt = false;
        auto place = tapn.getPlaces()[i];
        for (auto* arc : place->getInhibitorArcs()) {
            auto t = arc->getOutputTransition().getIndex();
            assert(t < tapn.getTransitions().size());
            if (!_stubborn[t]) _unprocessed.push_back(t);
            zt |= _enabled[t] && tapn.getTransitions()[t]->isUrgent();
            _stubborn[t] = true;
        }
        return zt;
    }


    void ReducingGenerator::from_marking(NonStrictMarkingBase *parent, Mode mode, bool urgent) {
        Generator::from_marking(parent, mode, urgent);
        std::fill(_enabled.begin(), _enabled.end(), false);
        assert(_ordering.empty());
        auto &trans = tapn.getTransitions();
        const TAPN::TimedTransition *urg_trans = nullptr;
        ecnt = 0;
        can_reduce = false;
        for (uint32_t i = 0; i < trans.size(); ++i) {
            auto t = trans[i];
            if (is_enabled(t)) {
                _enabled[i] = true;
                _ordering.push_back(i);
                if (urg_trans == nullptr && t->isUrgent()) {
                    assert(i < tapn.getTransitions().size());
                    urg_trans = t;
                }
                ++ecnt;
            }
        }

        if (ecnt <= 1) {
            _ordering.clear();
            return;
        }

        const TAPN::TimedPlace *inv_place = nullptr;
        int32_t max_age = -1;
        can_reduce = urgent || !_unprocessed.empty() || urg_trans != nullptr;
        if (!can_reduce) {
            for (auto &place : parent->getPlaceList()) {
                int inv = place.place->getInvariant().getBound();
                max_age = place.maxTokenAge();
                if (max_age == inv) {
                    can_reduce = true;
                    inv_place = place.place;
                    break;
                }
            }
        }

        if (!can_reduce) {
            _ordering.clear();
            return;
        }

        std::fill(_stubborn.begin(), _stubborn.end(), false);

        bool added_zero_time = ample_set(inv_place, urg_trans);
        added_zero_time |= compute_closure(added_zero_time);
        if (!added_zero_time) {
            zero_time_set(max_age, inv_place, urg_trans);
            compute_closure(true);
        }

    }

    void ReducingGenerator::zero_time_set(int32_t max_age, const TAPN::TimedPlace *inv_place,
                                          const TAPN::TimedTransition *trans) {
        if (trans) {
            // reason for urgency is an urgent edge
            _stubborn[trans->getIndex()] = true;
            auto t = trans->getIndex();
            _unprocessed.push_back(t);
            for (auto* a : trans->getInhibitorArcs())
                preSetOf(a->getInputPlace().getIndex());
        } else {
            // reason for urgency is an invariant
            assert(inv_place);
            for (auto* a : inv_place->getInputArcs()) {
                auto &interval = a->getInterval();
                if (interval.contains(max_age)) {
                    auto t = a->getOutputTransition().getIndex();

                    if (!_stubborn[t]) {
                        _stubborn[t] = true;
                        assert(t < tapn.getTransitions().size());
                        _unprocessed.push_back(t);
                    }
                }
            }
            for (auto* a : inv_place->getTransportArcs()) {
                if (&a->getDestination() == &a->getSource()) continue;
                auto &interval = a->getInterval();
                if (interval.contains(max_age)) {
                    uint32_t t = a->getTransition().getIndex();
                    if (!_stubborn[t]) {
                        _stubborn[t] = true;
                        assert(t < tapn.getTransitions().size());
                        _unprocessed.push_back(t);
                    }
                }
            }
        }
    }

    bool ReducingGenerator::ample_set(const TAPN::TimedPlace *inv_place, const TAPN::TimedTransition *trans) {
        bool added_zt = false;
        QueryVisitor<NonStrictMarkingBase> visitor(*parent, tapn);
        BoolResult context;
        query->accept(visitor, context);
        interesting.clear();
        query->accept(interesting, context);

        // compute the set of unprocessed
        for (size_t i = 0; i < interesting._incr.size(); ++i) {
            if (interesting._incr[i])
                added_zt |= preSetOf(i);
            if (interesting._decr[i])
                added_zt |= postSetOf(i, !added_zt);
        }

        if (interesting.deadlock && !trans && !added_zt) {
            // for now, just pick a single enabled,
            // verifypn has a good heuristic for this
            size_t min = 0;
            size_t max = _enabled.size();
            if (inv_place) {
                for (auto* a : inv_place->getInputArcs()) {
                    if (_enabled[a->getOutputTransition().getIndex()]) {
                        min = a->getOutputTransition().getIndex();
                        max = min + 1;
                        break;
                    }
                }
                if (max != min + 1) {
                    for (auto* a : inv_place->getTransportArcs()) {
                        if (_enabled[a->getTransition().getIndex()]) {
                            min = a->getTransition().getIndex();
                            max = min + 1;
                            break;
                        }
                    }
                }
            }
            for (size_t i = min; i < max; ++i) {
                if (_enabled[i]) {
                    auto trans = tapn.getTransitions()[i];
                    for (auto* a : trans->getPreset())
                        added_zt |= postSetOf(a->getInputPlace().getIndex(), !added_zt);
                    for (auto* a : trans->getTransportArcs())
                        added_zt |= postSetOf(a->getSource().getIndex(), !added_zt);
                    for (auto* a : trans->getInhibitorArcs()) {
                        auto &place = a->getInputPlace();
                        for (auto* arc : place.getInhibitorArcs()) {
                            added_zt |= preSetOf(arc->getInputPlace().getIndex());
                        }
                    }
                    break;
                }
            }
        }
        return added_zt;
    }

    bool ReducingGenerator::compute_closure(bool added_zt) {
        // Closure computation time!
        while (!_unprocessed.empty()) {
            uint32_t tr = _unprocessed.front();
            _unprocessed.pop_front();
            auto trans = tapn.getTransitions()[tr];
            assert(tr < tapn.getTransitions().size());
            assert(trans);
            if (_enabled[tr]) {
                for (auto* a : trans->getPreset())
                    added_zt |= postSetOf(a->getInputPlace().getIndex(), !added_zt, a->getInterval());
                for (auto* a : trans->getPostset())
                    added_zt |= inhibPostSetOf(a->getOutputPlace().getIndex());
                for (auto* a : trans->getTransportArcs()) {
                    added_zt |= postSetOf(a->getSource().getIndex(), !added_zt, a->getInterval());
                    added_zt |= inhibPostSetOf(a->getDestination().getIndex());
                }
            } else {

                if (auto inhib = inhibited(trans)) {
                    auto &p = inhib->getInputPlace();
                    auto &tl = parent->getTokenList(p.getIndex());
                    for (auto* arc : p.getInputArcs()) {
                        uint32_t trans = arc->getOutputTransition().getIndex();
                        if (!_stubborn[trans]) {
                            for (const auto& t : tl)
                                if (arc->getInterval().contains(t.getAge())) {
                                    _stubborn[trans] = true;
                                    _unprocessed.push_back(trans);
                                    break;
                                }
                        }
                    }
                    for (auto* arc : p.getTransportArcs()) {
                        uint32_t trans = arc->getTransition().getIndex();
                        if (!_stubborn[trans]) {
                            for (const auto& t : tl)
                                if (arc->getInterval().contains(t.getAge())) {
                                    _stubborn[trans] = true;
                                    _unprocessed.push_back(trans);
                                    break;
                                }
                        }
                    }
                } else {
                    // first we need to find the non-enabler
                    auto place = compute_missing(trans, nullptr);
                    bool found = false;
                    // add preset if zero is in guard
                    TAPN::TimeInterval interval;
                    for (auto* a : trans->getPreset()) {
                        if (&a->getInputPlace() == place) {
                            interval = a->getInterval();
                            if (interval.contains(0))
                                added_zt |= preSetOf(place->getIndex());
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        for (auto* a : trans->getTransportArcs()) {
                            if (&a->getSource() == place) {
                                interval = a->getInterval();
                                if (a->getInterval().contains(0))
                                    added_zt |= preSetOf(place->getIndex());
                                break;
                            }
                        }
                    }

                    // take care of transport-arcs
                    for (auto* a : place->getProdTransportArcs()) {
                        auto t = &a->getTransition();
                        uint32_t id = t->getIndex();
                        if (_stubborn[id]) continue;
                        if (a->getInterval().intersects(interval)) {
                            _stubborn[id] = true;
                            _unprocessed.push_back(id);
                        }
                    }
                }
            }
        }
        return added_zt;
    }

    NonStrictMarkingBase *ReducingGenerator::next(bool do_delay) {
        if (ecnt <= 1 || !can_reduce) {
            assert(_ordering.empty());
            auto nxt = Generator::next(do_delay);
            return nxt;
        }
        if (current) {
            auto nxt = Generator::next(false);
            return nxt;
        }
        while (!_ordering.empty()) {
            done = false;
            did_noinput = false;
            auto t = _ordering.front();
            _ordering.pop_front();
            if (_stubborn[t]) {
                auto trans = tapn.getTransitions()[t];

#ifndef NDEBUG
                bool en =
#endif
                        only_transition(trans);
                assert(en);
                break;
            }
        }
        if (current) {
            auto next = Generator::next(false);
            assert(next);
            return next;
        }
        if (done) {
            assert(_ordering.empty());
            return nullptr;
        }
        done = true;
        assert(_ordering.empty());
        _trans = nullptr;
        if (do_delay) return from_delay();
        return nullptr;
    }
} }
