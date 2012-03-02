#include "NormalizationVisitor.hpp"
#include "AST.hpp"

namespace VerifyTAPN
{
	namespace AST
	{
		struct Tuple{
			bool negate;
			Expression* returnExpr;

		public:
			Tuple(bool negate, Expression* expr) : negate(negate), returnExpr(expr) {};
		};

		void NormalizationVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any any = Tuple(!tuple.negate, NULL);
			expr.Child().Accept(*this, any);
			tuple.returnExpr = boost::any_cast<Tuple&>(any).returnExpr;
		}

		void NormalizationVisitor::Visit(const ParExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any any = Tuple(tuple.negate, NULL);
			expr.Child().Accept(*this, any);
			tuple.returnExpr = new ParExpression(boost::any_cast<Tuple&>(any).returnExpr);
		}

		void NormalizationVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any left = Tuple(tuple.negate, NULL), right = Tuple(tuple.negate, NULL);
			if(tuple.negate){
				expr.Left().Accept(*this, left);
				expr.Right().Accept(*this, right);
				tuple.returnExpr = new AndExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}else{
				expr.Left().Accept(*this, left);
				expr.Right().Accept(*this, right);
				tuple.returnExpr = new OrExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}
		}

		void NormalizationVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any left = Tuple(tuple.negate, NULL), right = Tuple(tuple.negate, NULL);
			if(tuple.negate){
				expr.Left().Accept(*this, left);
				expr.Right().Accept(*this, right);
				tuple.returnExpr = new OrExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}else{
				expr.Left().Accept(*this, left);
				expr.Right().Accept(*this, right);
				tuple.returnExpr = new AndExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}
		}

		void NormalizationVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			std::string op;
			if(tuple.negate){
				op = NegateOperator(expr.Operator());
			}else{
				op = expr.Operator();
			}
			tuple.returnExpr = new AtomicProposition(expr.Place(), &op, expr.N());
		}

		void NormalizationVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			if(tuple.negate){
				tuple.returnExpr = new BoolExpression(!expr.GetValue());
			}else{
				tuple.returnExpr = new BoolExpression(expr.GetValue());
			}
		}

		void NormalizationVisitor::Visit(const Query& query, boost::any& context)
		{
			boost::any any = Tuple(false, NULL);
			query.Child().Accept(*this, any);

			normalizedQuery = new AST::Query(query.GetQuantifier(), boost::any_cast<Tuple&>(any).returnExpr);
		}

		std::string NormalizationVisitor::NegateOperator(const std::string& op) const{
			if(op == "=" || op == "=="){
				return "!=";
			}else if(op == ">"){
				return "<=";
			}else if(op == "<"){
				return ">=";
			}else if(op == ">="){
				return "<";
			}else if(op == "<="){
				return ">";
			}else{
				std::cout << "Unknown operator";
				throw new std::exception();
			}
		}
	}
}
