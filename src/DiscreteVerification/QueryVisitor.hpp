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
            
            QueryVisitor(T& marking, TAPN::TimedArcPetriNet& tapn, int maxDelay) : marking(marking), tapn(tapn), maxDelay(maxDelay) {
                deadlockChecked = false;
                deadlocked = false;
            };

            QueryVisitor(T& marking, TAPN::TimedArcPetriNet& tapn) : marking(marking), tapn(tapn), maxDelay(0) {
                deadlockChecked = false;
                deadlocked = false;
            }
            
            virtual ~QueryVisitor() {
            };
 
        public: // visitor methods

            virtual void visit(const NotExpression& expr, AST::Result& context);
            virtual void visit(const OrExpression& expr, AST::Result& context);
            virtual void visit(const AndExpression& expr, AST::Result& context);
            virtual void visit(const AtomicProposition& expr, AST::Result& context);
            virtual void visit(const BoolExpression& expr, AST::Result& context);
            virtual void visit(const Query& query, AST::Result& context);
            virtual void visit(const DeadlockExpression& expr, AST::Result& context);
            virtual void visit(const NumberExpression& expr, Result& context);
            virtual void visit(const IdentifierExpression& expr, Result& context);
            virtual void visit(const MultiplyExpression& expr, Result& context);
            virtual void visit(const MinusExpression& expr, Result& context);
            virtual void visit(const SubtractExpression& expr, Result& context);
            virtual void visit(const PlusExpression& expr, Result& context);
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
        void QueryVisitor<T>::visit(const NotExpression& expr, AST::Result& context) {
            BoolResult c;
            expr.getChild().accept(*this, c);
            static_cast<BoolResult&>(context).value = !c.value;
        }

        template<typename T>
        void QueryVisitor<T>::visit(const OrExpression& expr, AST::Result& context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            // use lazy evaluation
            if(left.value){
                static_cast<BoolResult&>(context).value = true;
            } else {
                expr.getRight().accept(*this, right);
                static_cast<BoolResult&>(context).value = right.value;
            }
        }

        template<typename T>
        void QueryVisitor<T>::visit(const AndExpression& expr, AST::Result& context) {
            BoolResult left, right;
            expr.getLeft().accept(*this, left);
            
            // use lazy evaluation
            if(!left.value){
                static_cast<BoolResult&>(context).value = false;
            } else {
                expr.getRight().accept(*this, right);
                static_cast<BoolResult&>(context).value = right.value;
            }
        }

        template<typename T>

        void QueryVisitor<T>::visit(const AtomicProposition& expr, AST::Result& context) {
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getLeft().accept(*this, right);

            static_cast<BoolResult&>(context).value 
                    = compare(left.value, expr.getOperator(), right.value);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const BoolExpression& expr, AST::Result& context) {
            static_cast<BoolResult&>(context).value 
                    = expr.getValue();
        }

        template<typename T>
        void QueryVisitor<T>::visit(const NumberExpression& expr, Result& context){
            ((IntResult&)context).value = expr.getValue();
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(const IdentifierExpression& expr, Result& context){
            ((IntResult&)context).value = marking.numberOfTokensInPlace(expr.getPlace());
        }
        template<typename T>
        void QueryVisitor<T>::visit(const MultiplyExpression& expr, Result& context){
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getLeft().accept(*this, right);
            ((IntResult&)context).value = left.value * right.value;
        }
        template<typename T>
        void QueryVisitor<T>::visit(const MinusExpression& expr, Result& context){
            IntResult value;
            expr.getValue().accept(*this, value);
            ((IntResult&)context).value = -value.value;
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(const SubtractExpression& expr, Result& context){
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getLeft().accept(*this, right);
            ((IntResult&)context).value = left.value - right.value;
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(const PlusExpression& expr, Result& context){
            IntResult left;
            expr.getLeft().accept(*this, left);
            IntResult right;
            expr.getLeft().accept(*this, right);
            ((IntResult&)context).value = left.value + right.value;
        }
        
        
        template<typename T>
        void QueryVisitor<T>::visit(const Query& query, AST::Result& context) {
            query.getChild().accept(*this, context);
            if (query.getQuantifier() == AG || query.getQuantifier() == AF) {
                static_cast<BoolResult&>(context).value
                        = !static_cast<BoolResult&>(context).value;
            }
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(const DeadlockExpression& expr, AST::Result& context) {
            if(!deadlockChecked){
                deadlockChecked = true;
                deadlocked = marking.canDeadlock(tapn, maxDelay);
            }
            static_cast<BoolResult&>(context).value 
                    = deadlocked;
        }

        template<typename T>
        bool QueryVisitor<T>::compare(int numberOfTokensInPlace, const std::string& op, int n) const {
            if (op == "<") return numberOfTokensInPlace < n;
            else if (op == "<=") return numberOfTokensInPlace <= n;
            else if (op == "=" || op == "==") return numberOfTokensInPlace == n;
            else if (op == ">=") return numberOfTokensInPlace >= n;
            else if (op == ">") return numberOfTokensInPlace > n;
            else if (op == "!=") return numberOfTokensInPlace != n;
            else
                throw std::exception();
        }

    } /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */
