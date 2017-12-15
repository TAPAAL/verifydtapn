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

#include "ReducingGenerator.hpp"
#include "QueryVisitor.hpp"


namespace VerifyTAPN {
    namespace DiscreteVerification {

        void ReducingGenerator::preSetOf(size_t i) {
            auto place = tapn.getPlaces()[i];
            for (auto arc : place->getOutputArcs()) {
                auto t = arc->getInputTransition().getIndex();
                if (!_stubborn[t]) _unprocessed.push_back(t);
                _stubborn[t] = true;
            }
            for (auto arc : place->getProdTransportArcs()) {
                auto t = arc->getTransition().getIndex();
                if (!_stubborn[t]) _unprocessed.push_back(t);
                _stubborn[t] = true;
            }
        }
    
        void ReducingGenerator::postSetOf(size_t i) {
            auto place = tapn.getPlaces()[i];
            for (auto arc : place->getInputArcs()) {
                auto t = arc->getOutputTransition().getIndex();
                if (!_stubborn[t]) _unprocessed.push_back(t);
                _stubborn[t] = true;
            }

            for (auto arc : place->getTransportArcs()) {
                auto t = arc->getTransition().getIndex();
                if (!_stubborn[t]) _unprocessed.push_back(t);
                _stubborn[t] = true;
            }
        }
        
        void ReducingGenerator::inhibPostSetOf(size_t i)
        {
            auto place = tapn.getPlaces()[i];
            for(auto arc : place->getInhibitorArcs())
            {
                auto t = arc->getOutputTransition().getIndex();
                if (!_stubborn[t]) _unprocessed.push_back(t);
                _stubborn[t] = true;
            }
        }


        void ReducingGenerator::from_marking(NonStrictMarkingBase* parent, Mode mode, bool urgent) {
            Generator::from_marking(parent, mode, urgent);
            std::fill(_enabled.begin(), _enabled.end(), false);

            auto& trans = tapn.getTransitions();
            ecnt = 0;
            can_reduce = false;
            for (uint32_t i = 0; i < trans.size(); ++i) {
                auto t = trans[i];
                if (is_enabled(t)) {
                    _enabled[i] = true;
                    _ordering.push_back(i);
                    ++ecnt;
                }

            }

            if (ecnt <= 1) return;

            can_reduce = urgent || seen_urgent;
            if (!can_reduce) {
                for (auto& place : parent->getPlaceList()) {
                    int inv = place.place->getInvariant().getBound();
                    if (place.maxTokenAge() == inv) {
                        can_reduce = true;
                        break;
                    }
                }
            }

            if (!can_reduce) return;

            QueryVisitor<NonStrictMarkingBase> visitor(*parent, tapn);
            BoolResult context;
            query->accept(visitor, context);
            interesting.clear();
            query->accept(interesting, context);
            std::fill(_stubborn.begin(), _stubborn.end(), false);


            // compute the set of unprocessed
            for (size_t i = 0; i < interesting._incr.size(); ++i) {
                if (interesting._incr[i])
                    preSetOf(i);
                if (interesting._decr[i])
                    postSetOf(i);
            }

            assert(!interesting.deadlock);

            // lets construct the set of transitions
            while (!_unprocessed.empty()) {
                uint32_t tr = _unprocessed.front();
                _unprocessed.pop_front();
                auto trans = tapn.getTransitions()[tr];
                if (_enabled[tr]) {
                    for(auto a : trans->getPreset())
                        postSetOf(a->getInputPlace().getIndex());
                    for(auto a : trans->getTransportArcs())
                        postSetOf(a->getSource().getIndex());
                    for(auto a : trans->getInhibitorArcs())
                        inhibPostSetOf(a->getInputPlace().getIndex());
                } else {
                    for(auto a : trans->getPreset())
                    {
                        auto pid = a->getInputPlace().getIndex();
                        if(parent->numberOfTokensInPlace(pid) < a->getWeight())
                        {
                            preSetOf(pid);
                            break;
                        }
                    }

                    for(auto a : trans->getTransportArcs())
                    {
                        auto pid = a->getSource().getIndex();
                        if(parent->numberOfTokensInPlace(pid) < a->getWeight())
                        {
                            preSetOf(pid);
                            break;
                        }
                    }

                    for(auto a : trans->getInhibitorArcs())
                    {
                        auto pid = a->getInputPlace().getIndex();
                        if(parent->numberOfTokensInPlace(pid) >= a->getWeight())
                        {
                            postSetOf(pid);
                            break;
                        }
                    }
                }
            }
        }

        NonStrictMarkingBase* ReducingGenerator::next(bool do_delay) {
            if (ecnt <= 1 || !can_reduce) return Generator::next(do_delay);
            if(current) return Generator::next(do_delay);
            while (!_ordering.empty()) {
                auto t = _ordering.front();
                _ordering.pop_front();
                if (_stubborn[t]) {
                    auto trans = tapn.getTransitions()[t];
#ifndef NDEBUG
                    bool en =
#endif
                    is_enabled(trans, &base_permutation);
                    assert(en);
                    permutation = base_permutation;
                    only_transition(trans);
                    break;
                }
            }
            if(current) return Generator::next(do_delay);
            if(done) return nullptr;
            done = true;
            if(do_delay) return from_delay();
            return nullptr;
        }
        
        void InterestingVisitor::clear() {
            std::fill(_decr.begin(), _decr.end(), 0);
            std::fill(_incr.begin(), _incr.end(), 0);
            deadlock = false;
        }

        void InterestingVisitor::visit(NotExpression& expr, Result& context) {
            negate();
            expr.getChild().accept(*this, context);
            negate();
        }

        void InterestingVisitor::visit(OrExpression& expr, Result& context) {
            if (!negated) {
                expr.getLeft().accept(*this, context);
                expr.getRight().accept(*this, context);
            } else {
                if (expr.getLeft().eval)
                    expr.getLeft().accept(*this, context);
                else if (expr.getRight().eval)
                    expr.getRight().accept(*this, context);
            }
        }

        void InterestingVisitor::visit(AndExpression& expr, Result& context) {
            if (negated) {
                expr.getLeft().accept(*this, context);
                expr.getRight().accept(*this, context);
            } else {
                if (expr.getLeft().eval)
                    expr.getLeft().accept(*this, context);
                else if (expr.getRight().eval)
                    expr.getRight().accept(*this, context);
            }
        }

        static IncDecr ic(true, false);
        static IncDecr dc(true, false);

        void InterestingVisitor::visit(AtomicProposition& expr, Result& context) {
            auto incdec = [this, &expr](bool id1, bool id2) {
                if (id1) expr.getLeft().accept(*this, ic);
                else expr.getLeft().accept(*this, dc);
                if (id2) expr.getRight().accept(*this, ic);
                else expr.getRight().accept(*this, dc);
            };
            if (expr.getOperator() == "<") {
                if (!expr.eval && !negated)
                    incdec(false, true);
                else if (expr.eval && negated)
                    incdec(true, false);
            } else if (expr.getOperator() == ">=") {
                if (!expr.eval && !negated)
                    incdec(true, false);
                else if (expr.eval && negated)
                    incdec(false, true);
            } else if (expr.getOperator() == "<=") {
                if (!expr.eval && !negated)
                    incdec(false, true);
                else if (expr.eval && negated)
                    incdec(true, false);
            } else if (expr.getOperator() == ">") {
                if (!expr.eval && !negated)
                    incdec(true, false);
                else if (expr.eval && negated)
                    incdec(false, true);
            } else if (expr.getOperator() == "==") {
                if (!expr.eval && !negated) {
                    if (expr.getLeft().eval < expr.getRight().eval)
                        incdec(true, false);
                    else
                        incdec(false, true);
                } else if (expr.eval && negated) {
                    incdec(true, true);
                    incdec(false, false);
                }
            } else if (expr.getOperator() == "!=") {
                if (!expr.eval && !negated) {
                    incdec(true, true);
                    incdec(false, false);
                } else if (expr.eval && negated) {
                    if (expr.getLeft().eval < expr.getRight().eval)
                        incdec(true, false);
                    else
                        incdec(false, true);
                }
            } else {
                assert(false);
            }
        }

        void InterestingVisitor::visit(BoolExpression& expr, Result& context) {
            // nothing!
        }

        void InterestingVisitor::visit(Query& query, Result& context) {
            if (query.getQuantifier() == Quantifier::AG) negate();
            query.getChild()->accept(*this, context);
            if (query.getQuantifier() == Quantifier::AG) negate();
        }

        void InterestingVisitor::visit(DeadlockExpression& expr, Result& context) {
            if (!negated) deadlock = !negated;
        }

        void InterestingVisitor::visit(NumberExpression& expr, Result& context) {
            // nothing!
        }

        void InterestingVisitor::visit(IdentifierExpression& expr, Result& context) {
            auto& id = static_cast<IncDecr&> (context);
            if (id.incr) _incr[expr.getPlace()] = true;
            if (id.decr) _decr[expr.getPlace()] = true;
        }

        void InterestingVisitor::visit(MultiplyExpression& expr, Result& context) {
            IncDecr c(true, true);
            expr.getLeft().accept(*this, c);
            expr.getRight().accept(*this, c);
        }

        void InterestingVisitor::visit(MinusExpression& expr, Result& context) {
            auto& id = static_cast<IncDecr&> (context);
            if (id.incr) {
                IncDecr c(false, true);
                expr.accept(*this, c);
            }

            if (id.decr) {
                IncDecr c(false, true);
                expr.accept(*this, c);
            }
        }

        void InterestingVisitor::visit(SubtractExpression& expr, Result& context) {
            auto& id = static_cast<IncDecr&> (context);
            if (id.incr) {
                IncDecr c(false, true);
                expr.getLeft().accept(*this, context);
                expr.getRight().accept(*this, c);
            }

            if (id.decr) {
                IncDecr c(false, true);
                expr.getLeft().accept(*this, context);
                expr.getRight().accept(*this, c);
            }
        }

        void InterestingVisitor::visit(PlusExpression& expr, Result& context) {
            expr.getLeft().accept(*this, context);
            expr.getRight().accept(*this, context);
        }




    }
}
