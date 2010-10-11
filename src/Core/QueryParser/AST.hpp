#ifndef AST_HPP_
#define AST_HPP_

#include <string>
#include "boost/variant.hpp"
#include "boost/fusion/include/adapt_struct.hpp"

namespace VerifyTAPN
{
	namespace AST
	{
		struct ParExpression;

		struct AtomicProposition
		{
			std::string place;
			std::string op;
			int n;
		};

		typedef boost::variant<
			boost::recursive_wrapper<ParExpression>,
			AtomicProposition
		> atom_prop;

		struct AndExpression{
			atom_prop left;
			atom_prop right;
		};

		struct OrExpression{
			AndExpression left;
			AndExpression right;
		};

		struct ParExpression{
			OrExpression expr;
		};

		struct Query{
			std::string quantifier;
			ParExpression parExpr;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
		VerifyTAPN::AST::AtomicProposition,
		(std::string, place)
		(std::string, op)
		(int, n)
)

BOOST_FUSION_ADAPT_STRUCT(
		VerifyTAPN::AST::AndExpression,
		(VerifyTAPN::AST::atom_prop, left)
		(VerifyTAPN::AST::atom_prop, right)
)

BOOST_FUSION_ADAPT_STRUCT(
		VerifyTAPN::AST::OrExpression,
		(VerifyTAPN::AST::AndExpression, left)
		(VerifyTAPN::AST::AndExpression, right)
)

BOOST_FUSION_ADAPT_STRUCT(
		VerifyTAPN::AST::ParExpression,
		(VerifyTAPN::AST::OrExpression, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
		VerifyTAPN::AST::Query,
		(std::string, quantifier)
		(VerifyTAPN::AST::ParExpression, parExpr)
)



//BOOST_FUSION_ADAPT_STRUCT(
//		VerifyTAPN::Query::Query,
//		(std::string, quantifier)
//		(VerifyTAPN::Query::ParExpression, parExpr)
//)

#endif /* AST_HPP_ */
