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

            virtual void visit(const NotExpression& expr, boost::any& context);
            virtual void visit(const ParExpression& expr, boost::any& context);
            virtual void visit(const OrExpression& expr, boost::any& context);
            virtual void visit(const AndExpression& expr, boost::any& context);
            virtual void visit(const AtomicProposition& expr, boost::any& context);
            virtual void visit(const BoolExpression& expr, boost::any& context);
            virtual void visit(const Query& query, boost::any& context);
            virtual void visit(const DeadlockExpression& expr, boost::any& context);
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
        void QueryVisitor<T>::visit(const NotExpression& expr, boost::any& context) {
            boost::any c;
            expr.getChild().accept(*this, c);
            context = !boost::any_cast<bool>(c);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const ParExpression& expr, boost::any& context) {
            expr.getChild().accept(*this, context);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const OrExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const AndExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);

            context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right);
        }

        template<typename T>

        void QueryVisitor<T>::visit(const AtomicProposition& expr, boost::any& context) {
            int numberOfTokens = marking.numberOfTokensInPlace(expr.getPlace());
            context = compare(numberOfTokens, expr.getOperator(), expr.getNumberOfTokens());
        }

        template<typename T>
        void QueryVisitor<T>::visit(const BoolExpression& expr, boost::any& context) {
            context = expr.getValue();
        }

        template<typename T>
        void QueryVisitor<T>::visit(const Query& query, boost::any& context) {
            query.getChild().accept(*this, context);
            if (query.getQuantifier() == AG || query.getQuantifier() == AF) {
                context = !boost::any_cast<bool>(context);
            }
        }
        
        template<typename T>
        void QueryVisitor<T>::visit(const DeadlockExpression& expr, boost::any& context) {
            if(!deadlockChecked){
                deadlockChecked = true;
                deadlocked = marking.canDeadlock(tapn, maxDelay);
            }
            context = deadlocked;
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
