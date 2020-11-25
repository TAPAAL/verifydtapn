#ifndef VISITOR_HPP_
#define VISITOR_HPP_

#include <vector>

namespace VerifyTAPN { namespace AST {
    class NotExpression;

    class OrExpression;

    class AndExpression;

    class AtomicProposition;

    class DeadlockExpression;

    class BoolExpression;

    class NumberExpression;

    class IdentifierExpression;

    class MultiplyExpression;

    class MinusExpression;

    class SubtractExpression;

    class PlusExpression;

    class ArithmeticExpression;


    class Query;

    class Result {
    };

    template<typename T>
    class SpecificResult : public Result {
    public:
        T value;
    };

    typedef SpecificResult<int> IntResult;
    typedef SpecificResult<bool> BoolResult;
    typedef SpecificResult<std::vector<int> > IntVectorResult;

    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visit(NotExpression &expr, Result &context) = 0;

        virtual void visit(OrExpression &expr, Result &context) = 0;

        virtual void visit(AndExpression &expr, Result &context) = 0;

        virtual void visit(AtomicProposition &expr, Result &context) = 0;

        virtual void visit(BoolExpression &expr, Result &context) = 0;

        virtual void visit(Query &query, Result &context) = 0;

        virtual void visit(DeadlockExpression &expr, Result &context) = 0;

        virtual void visit(NumberExpression &expr, Result &context) = 0;

        virtual void visit(IdentifierExpression &expr, Result &context) = 0;

        virtual void visit(MultiplyExpression &expr, Result &context) = 0;

        virtual void visit(MinusExpression &expr, Result &context) = 0;

        virtual void visit(SubtractExpression &expr, Result &context) = 0;

        virtual void visit(PlusExpression &expr, Result &context) = 0;
    };
} }


#endif /* VISITOR_HPP_ */
