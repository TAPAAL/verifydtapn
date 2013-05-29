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

		void NormalizationVisitor::visit(const NotExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any any = Tuple(!tuple.negate, NULL);
			expr.getChild().accept(*this, any);
			tuple.returnExpr = boost::any_cast<Tuple&>(any).returnExpr;
		}

		void NormalizationVisitor::visit(const ParExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any any = Tuple(tuple.negate, NULL);
			expr.getChild().accept(*this, any);
			tuple.returnExpr = new ParExpression(boost::any_cast<Tuple&>(any).returnExpr);
		}

		void NormalizationVisitor::visit(const OrExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any left = Tuple(tuple.negate, NULL), right = Tuple(tuple.negate, NULL);
			if(tuple.negate){
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new AndExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}else{
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new OrExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}
		}

		void NormalizationVisitor::visit(const AndExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			boost::any left = Tuple(tuple.negate, NULL), right = Tuple(tuple.negate, NULL);
			if(tuple.negate){
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new OrExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}else{
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new AndExpression(boost::any_cast<Tuple&>(left).returnExpr, boost::any_cast<Tuple&>(right).returnExpr);
			}
		}

		void NormalizationVisitor::visit(const AtomicProposition& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			std::string op;
			if(tuple.negate){
				op = negateOperator(expr.getOperator());
			}else{
				op = expr.getOperator();
			}
			tuple.returnExpr = new AtomicProposition(expr.getPlace(), &op, expr.getNumberOfTokens());
		}

		void NormalizationVisitor::visit(const BoolExpression& expr, boost::any& context)
		{
			Tuple& tuple = boost::any_cast<Tuple&>(context);
			if(tuple.negate){
				tuple.returnExpr = new BoolExpression(!expr.getValue());
			}else{
				tuple.returnExpr = new BoolExpression(expr.getValue());
			}
		}

		void NormalizationVisitor::visit(const Query& query, boost::any& context)
		{
			boost::any any = Tuple(false, NULL);
			query.getChild().accept(*this, any);

			normalizedQuery = new AST::Query(query.getQuantifier(), boost::any_cast<Tuple&>(any).returnExpr);
		}

		std::string NormalizationVisitor::negateOperator(const std::string& op) const{
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
