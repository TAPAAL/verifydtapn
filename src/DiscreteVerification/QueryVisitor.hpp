/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef QUERYVISITOR_HPP_
#define QUERYVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "DataStructures/NonStrictMarking.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>

namespace VerifyTAPN {
    namespace DiscreteVerification {

        using namespace AST;

        template<typename T>
        class QueryVisitor : public Visitor {
        public:
            
            QueryVisitor(T& marking, const TAPN::TimedArcPetriNet& tapn, int maxDelay) : marking(marking), tapn(tapn), maxDelay(maxDelay) {
                deadlockChecked = false;
                deadlocked = false;
            };

            QueryVisitor(T& marking, const TAPN::TimedArcPetriNet& tapn) : marking(marking), tapn(tapn), maxDelay(0) {
                deadlockChecked = false;
                deadlocked = false;
            }
            
            virtual ~QueryVisitor() {
            };
 
        public: // visitor methods

            virtual void visit(NotExpression& expr, AST::Result& context);
            virtual void visit(OrExpression& expr, AST::Result& context);
            virtual void visit(AndExpression& expr, AST::Result& context);
            virtual void visit(AtomicProposition& expr, AST::Result& context);
            virtual void visit(BoolExpression& expr, AST::Result& context);
            virtual void visit(Query& query, AST::Result& context);
            virtual void visit(DeadlockExpression& expr, AST::Result& context);
            virtual void visit(NumberExpression& expr,  AST::Result& context);
            virtual void visit(IdentifierExpression& expr,  AST::Result& context);
            virtual void visit(MultiplyExpression& expr,  AST::Result& context);
            virtual void visit(MinusExpression& expr,  AST::Result& context);
            virtual void visit(SubtractExpression& expr,  AST::Result& context);
            virtual void visit(PlusExpression& expr,  AST::Result& context);
        private:
            bool compare(int numberOfTokensInPlace, const std::string& op, int n) const;

        private:
            const T& marking;
            const TAPN::TimedArcPetriNet& tapn;
            bool deadlockChecked;
            bool deadlocked;
            const int maxDelay;
        };
        
        template<typename T>
        void QueryVisitor<T>::visit(NotExpression& expr, AST::Result& context) {
            BoolResult c;
            expr.getChild().accept(*this, c);
            expr.eval = !c.value;
            static_cast<BoolResult&>(context).value = expr.getChild().eval;
        }

        template<typename T>
        void QueryVisitor<T>::visit(OrExpression& expr, AST::Result& context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            // use lazy evaluation
            if(left.value){
                static_cast<BoolResult&>(context).value = true;
            } else {
                expr.getRight().accept(*this, right);
                static_cast<BoolResult&>(context).value = right.value;
            }
            expr.eval = static_cast<BoolResult&>(context).value;
        }

        template<typename T>
        void QueryVisitor<T>::visit(AndExpression& expr, AST::Result& context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            
            // use lazy evaluation
            if(!left.value){
                static_cast<BoolResult&>(context).value = false;
            } else {
                expr.getRight().accept(*this, right);
                static_cast<BoolResult&>(context).value = right.value;
            }
            expr.eval = static_cast<BoolResult&>(context).value;
        }

        template<typename T>
        void QueryVisitor<T>::visit(AtomicProposition& expr, AST::Result& context) {
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getRight().accept(*this, right);

            static_cast<BoolResult&>(context).value 
                    = compare(left.value, expr.getOperator(), right.value);
            expr.eval = static_cast<BoolResult&>(context).value;
        }

        template<typename T>
        void QueryVisitor<T>::visit(BoolExpression& expr, AST::Result& context) {
            static_cast<BoolResult&>(context).value 
                    = expr.getValue();
            expr.eval = expr.getValue();
        }

        template<typename T>
        void QueryVisitor<T>::visit(NumberExpression& expr,  AST::Result& context){
            ((IntResult&)context).value = expr.getValue();
            expr.eval = static_cast<IntResult&>(context).value;
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(IdentifierExpression& expr, AST::Result& context){
            ((IntResult&)context).value = marking.numberOfTokensInPlace(expr.getPlace());
            expr.eval = static_cast<IntResult&>(context).value;
        }

        template<typename T>
        void QueryVisitor<T>::visit(MultiplyExpression& expr,  AST::Result& context){
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getRight().accept(*this, right);
            ((IntResult&)context).value = left.value * right.value;
            expr.eval = static_cast<IntResult&>(context).value;
        }

        template<typename T>
        void QueryVisitor<T>::visit(MinusExpression& expr,  AST::Result& context){
            IntResult value;
            expr.getValue().accept(*this, value);
            ((IntResult&)context).value = -value.value;
            expr.eval = static_cast<IntResult&>(context).value;
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(SubtractExpression& expr,  AST::Result& context){
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getRight().accept(*this, right);
            ((IntResult&)context).value = left.value - right.value;
            expr.eval = static_cast<IntResult&>(context).value;
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(PlusExpression& expr,  AST::Result& context){
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getRight().accept(*this, right);
            ((IntResult&)context).value = left.value + right.value;
            expr.eval = static_cast<IntResult&>(context).value;
        }
        
        
        template<typename T>
        void QueryVisitor<T>::visit(Query& query,  AST::Result& context) {
            query.getChild()->accept(*this, context);
            if (query.getQuantifier() == AG || query.getQuantifier() == AF) {
                static_cast<BoolResult&>(context).value
                        = !static_cast<BoolResult&>(context).value;
            }
            query.eval = static_cast<IntResult&>(context).value;
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(DeadlockExpression& expr, AST::Result& context) {
            if(!deadlockChecked){
                deadlockChecked = true;
                deadlocked = marking.canDeadlock(tapn, maxDelay);
            }
            static_cast<BoolResult&>(context).value 
                    = deadlocked;
            expr.eval = static_cast<BoolResult&>(context).value;
        }

        template<typename T>
        bool QueryVisitor<T>::compare(int numberOfTokensInPlace, const std::string& op, int n) const {
            if (op == "<") return numberOfTokensInPlace < n;
            else if (op == "<=") return numberOfTokensInPlace <= n;
            else if (op == "=" || op == "==") return numberOfTokensInPlace == n;
            else if (op == ">=") return numberOfTokensInPlace >= n;
            else if (op == ">") return numberOfTokensInPlace > n;
            else if (op == "!=") return numberOfTokensInPlace != n;
            assert(false);
            return false;
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */
