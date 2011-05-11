#ifndef AST_HPP_
#define AST_HPP_

#include <string>
#include <iostream>
#include "Visitor.hpp"
#include "boost/any.hpp"

namespace VerifyTAPN{
	namespace AST {

		class Visitable
		{
		public:
			virtual void Accept(Visitor& visitor, boost::any& context) const = 0;
		};

		class Expression : public Visitable
		{
		public:
			virtual ~Expression() { };
			//virtual void Accept(Visitor& visitor, boost::any& context) const = 0;
			virtual Expression* clone() const = 0;
		};

		class BoolExpression : public Expression
		{
		public:
			explicit BoolExpression(bool value) : value(value) {};
			virtual ~BoolExpression() { };

			virtual BoolExpression* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			bool GetValue() const { return value; };
		private:
			bool value;
		};

		class AtomicProposition : public Expression
		{
		public:
			AtomicProposition(int placeIndex, std::string* op, int n) : place(placeIndex), op(op->begin(), op->end()), n(n){};
			AtomicProposition(const AtomicProposition& other) : place(other.place), op(other.op), n(other.n) { };
			AtomicProposition& operator=(const AtomicProposition& other)
			{
				if(&other != this){
					place = other.place;
					op = other.op;
					n = other.n;
				}
				return *this;
			}

			virtual ~AtomicProposition() { };

			virtual AtomicProposition* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			const int Place() const { return place; }
			const std::string& Operator() const { return op; }
			int N() const { return n; }
		private:
			int place;
			std::string op;
			int n;
		};

		class AndExpression : public Expression
		{
		public:
			AndExpression(Expression* left, Expression* right) : left(left), right(right) { };
			AndExpression(const AndExpression& other) : left(other.left->clone()), right(other.right->clone()) {};
			AndExpression& operator=(const AndExpression& other)
			{
				if(&other != this){
					delete left;
					delete right;

					left = other.left->clone();
					right = other.right->clone();
				}
				return *this;
			}

			virtual ~AndExpression() {
				if( left ) delete left;
				if( right ) delete right;
			}

			virtual AndExpression* clone() const;
			void Accept(Visitor& visitor, boost::any& context) const;

			const Expression& Left() const { return *left; }
			const Expression& Right() const { return *right; }
		private:
			Expression* left;
			Expression* right;
		};

		class OrExpression : public Expression
		{
		public:
			OrExpression(Expression* left, Expression* right) : left(left), right(right) { };
			OrExpression(const OrExpression& other) : left(other.left->clone()), right(other.right->clone()) { };
			OrExpression& operator=(const OrExpression& other)
			{
				if(&other != this){
					delete left;
					delete right;

					left = other.left->clone();
					right = other.right->clone();
				}
				return *this;
			}


			virtual ~OrExpression(){
				if( left ) delete left;
				if( right ) delete right;
			};



			virtual OrExpression* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			const Expression& Left() const { return *left; }
			const Expression& Right() const { return *right; }
		private:
			Expression* left;
			Expression* right;
		};

		class ParExpression : public Expression
		{
		public:
			explicit ParExpression(Expression* expr) : expr(expr) { };
			ParExpression(const ParExpression& other) : expr(other.expr->clone()) { };
			ParExpression& operator=(const ParExpression& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}

				return *this;
			}

			virtual ~ParExpression(){
				if( expr ) delete expr;
			};

			virtual ParExpression* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			const Expression& Child() const { return *expr; }
		private:
			Expression* expr;
		};

		enum Quantifier { EF, AG };

		class Query : public Visitable
		{
		public:
			Query(Quantifier quantifier, Expression* expr) : quantifier(quantifier), expr(expr) { };
			Query(const Query& other) : quantifier(other.quantifier), expr(other.expr->clone()) { };
			Query& operator=(const Query& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}
				return *this;
			}

			virtual ~Query() { if( expr ) delete expr; }

			virtual Query* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			Quantifier GetQuantifier() const { return quantifier; }
			const Expression& Child() const { return *expr; }
		private:
			Quantifier quantifier;
			Expression* expr;
		};
	}
}

#endif /* AST_HPP_ */
