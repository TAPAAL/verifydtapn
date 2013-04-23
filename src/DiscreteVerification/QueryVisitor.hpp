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

            QueryVisitor(T& marking) : marking(marking) {
            };

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
        private:
            bool Compare(int numberOfTokensInPlace, const std::string& op, int n) const;

        private:
            const T& marking;
        };
        
        template<typename T>
        void QueryVisitor<T>::visit(const NotExpression& expr, boost::any& context) {
            boost::any c;
            expr.Child().Accept(*this, c);
            context = !boost::any_cast<bool>(c);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const ParExpression& expr, boost::any& context) {
            expr.Child().Accept(*this, context);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const OrExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.Left().Accept(*this, left);
            expr.Right().Accept(*this, right);

            context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const AndExpression& expr, boost::any& context) {
            boost::any left, right;
            expr.Left().Accept(*this, left);
            expr.Right().Accept(*this, right);

            context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right);
        }

        template<typename T>
        void QueryVisitor<T>::visit(const AtomicProposition& expr, boost::any& context) {
            int numberOfTokens = marking.numberOfTokensInPlace(expr.Place());
            context = Compare(numberOfTokens, expr.Operator(), expr.N());
        }

        template<typename T>
        void QueryVisitor<T>::visit(const BoolExpression& expr, boost::any& context) {
            context = expr.GetValue();
        }

        template<typename T>
        void QueryVisitor<T>::visit(const Query& query, boost::any& context) {
            query.Child().Accept(*this, context);
            if (query.GetQuantifier() == AG || query.GetQuantifier() == AF) {
                context = !boost::any_cast<bool>(context);
            }
        }

        template<typename T>
        bool QueryVisitor<T>::Compare(int numberOfTokensInPlace, const std::string& op, int n) const {
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
