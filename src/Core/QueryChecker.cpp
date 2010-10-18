#include "QueryChecker.hpp"
#include "QueryParser/AST.hpp"
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

	bool QueryChecker::IsEF() const
	{
		return query->GetQuantifier() == VerifyTAPN::AST::EF;
	}

	bool QueryChecker::IsAG() const
	{
		return query->GetQuantifier() == VerifyTAPN::AST::AG;
	}
}
