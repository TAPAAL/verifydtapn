#include "QueryChecker.hpp"
#include "SymMarking.hpp"
#include "QueryParser/ExpressionSatisfiedVisitor.hpp"
#include "boost/any.hpp"
#include "QueryParser/AST.hpp"

namespace VerifyTAPN
{
	QueryChecker::QueryChecker(const AST::Query* query) : query(query)
	{
	}

	QueryChecker::~QueryChecker()
	{
		if( query ) delete query;
	}

	bool QueryChecker::IsExpressionSatisfied(const SymMarking& marking) const
	{
		AST::ExpressionSatisfiedVisitor visitor(marking);
		boost::any any;
		query->Accept(visitor, any);

		return boost::any_cast<bool>(any);
	}
}
