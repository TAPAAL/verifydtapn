/* 
 * File:   ReducingGenerator.hpp
 * Author: Peter G. Jensen
 *
 * Created on December 14, 2017, 8:44 PM
 */

#ifndef REDUCINGGENERATOR_HPP
#define REDUCINGGENERATOR_HPP

#include "Generator.h"
#include "DataStructures/light_deque.h"
namespace VerifyTAPN {
namespace DiscreteVerification {
    
    struct IncDecr : public Result
    {
        IncDecr(bool i, bool d)
                : incr(i), decr(d) {};
                
        bool incr, decr;
    };
    
    class ReducingGenerator;
    
    class InterestingVisitor : public Visitor
    {
    public: // visitor methods
        friend class ReducingGenerator;
        InterestingVisitor(TAPN::TimedArcPetriNet& tapn) 
        : _incr(tapn.getPlaces().size()), _decr(tapn.getPlaces().size()) 
        {
            _incr.shrink_to_fit();
            _decr.shrink_to_fit();
        };
        virtual void visit(NotExpression& expr, Result& context);
        virtual void visit(OrExpression& expr, Result& context);
        virtual void visit(AndExpression& expr, Result& context);
        virtual void visit(AtomicProposition& expr, Result& context);
        virtual void visit(BoolExpression& expr, Result& context);
        virtual void visit(Query& query, Result& context);
        virtual void visit(DeadlockExpression& expr, Result& context);
        virtual void visit(NumberExpression& expr, Result& context);
        virtual void visit(IdentifierExpression& expr, Result& context);
        virtual void visit(MultiplyExpression& expr, Result& context);
        virtual void visit(MinusExpression& expr, Result& context);
        virtual void visit(SubtractExpression& expr, Result& context);
        virtual void visit(PlusExpression& expr, Result& context);
        bool isNegated() { return negated; }
        void negate() { negated = !negated; }
        void clear();
    private:
        bool negated = false;
        bool deadlock = false;
        std::vector<bool> _incr;
        std::vector<bool> _decr;
    };
    
    class ReducingGenerator : public Generator {
    protected:
        InterestingVisitor interesting;
        std::vector<bool> _enabled, _stubborn;
        size_t ecnt = 0;
        bool can_reduce = false;
        light_deque<uint32_t> _unprocessed, _ordering;

        bool preSetOf(size_t i);
        bool postSetOf(size_t i, bool check_age);
        bool inhibPostSetOf(size_t i);
        void zero_time_set(int32_t max_age, const TAPN::TimedPlace*, const TAPN::TimedTransition*);
        bool ample_set();
        bool compute_closure(bool added_zt);
    public:
        ReducingGenerator(TAPN::TimedArcPetriNet& tapn, AST::Query* query) 
        : Generator(tapn, query), interesting(tapn), _enabled(tapn.getTransitions().size()), _stubborn(tapn.getTransitions().size()) {};

        virtual void from_marking(NonStrictMarkingBase* parent, Mode mode = ALL, bool urgent = false);
        virtual NonStrictMarkingBase* next(bool do_delay = true);
    };

}
}

#endif /* REDUCINGGENERATOR_HPP */

