/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   InterestingVisitor.h
 * Author: pgj
 *
 * Created on 3 November 2020, 09.29
 */

#ifndef INTERESTINGVISITOR_H
#define INTERESTINGVISITOR_H

#include "Core/TAPN/TAPN.hpp"
#include "Core/Query/Visitor.hpp"

#include "DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"
#include "DiscreteVerification/DataStructures/NonStrictMarking.hpp"

#include <vector>
#include <algorithm>

namespace VerifyTAPN { namespace DiscreteVerification {
    class ReducingGenerator;
    struct IncDecr : public Result {
        IncDecr(bool i, bool d)
                : incr(i), decr(d) {};

        bool incr, decr;
    };

    

    class InterestingVisitor : public Visitor {
    public: // visitor methods
        friend class ReducingGenerator;

        explicit InterestingVisitor(const TAPN::TimedArcPetriNet &tapn)
                : _incr(tapn.getPlaces().size()), _decr(tapn.getPlaces().size()) {
            _incr.shrink_to_fit();
            _decr.shrink_to_fit();
        };

        void visit(NotExpression &expr, Result &context) override;

        void visit(OrExpression &expr, Result &context) override;

        void visit(AndExpression &expr, Result &context) override;

        void visit(AtomicProposition &expr, Result &context) override;

        void visit(BoolExpression &expr, Result &context) override;

        void visit(Query &query, Result &context) override;

        void visit(DeadlockExpression &expr, Result &context) override;

        void visit(NumberExpression &expr, Result &context) override;

        void visit(IdentifierExpression &expr, Result &context) override;

        void visit(MultiplyExpression &expr, Result &context) override;

        void visit(MinusExpression &expr, Result &context) override;

        void visit(SubtractExpression &expr, Result &context) override;

        void visit(PlusExpression &expr, Result &context) override;

        bool isNegated() { return _negated; }

        void negate() { _negated = !_negated; }

        void clear();

        bool deadlock() const { return _deadlock; }
        
        bool increments(size_t p) const { return _incr[p]; }
        bool decrements(size_t p) const { return _decr[p]; }
        
    private:
        bool _negated = false;
        bool _deadlock = false;
        std::vector<bool> _incr;
        std::vector<bool> _decr;
    };    
} }
#endif /* INTERESTINGVISITOR_H */

