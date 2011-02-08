#include "QueryChecker.hpp"
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "../../Core/QueryParser/ExpressionSatisfiedVisitor.hpp"
#include "boost/any.hpp"
#include "../../Core/QueryParser/AST.hpp"

namespace VerifyTAPN
{
	QueryChecker::QueryChecker(const AST::Query* query) : query(query)
	{
	}

	QueryChecker::~QueryChecker()
	{
		if( query ) delete query;
	}

	bool QueryChecker::IsExpressionSatisfied(const SymbolicMarking& marking) const
	{
		AST::ExpressionSatisfiedVisitor visitor(marking);
		boost::any any;
		query->Accept(visitor, any);

		return boost::any_cast<bool>(any);
	}
}
