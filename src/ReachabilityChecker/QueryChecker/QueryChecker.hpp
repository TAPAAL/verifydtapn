#ifndef QUERYCHECKER_HPP_
#define QUERYCHECKER_HPP_

#include "../../Core/QueryParser/AST.hpp"

namespace VerifyTAPN
{
	class SymbolicMarking;

	class QueryChecker
	{
	public:
		QueryChecker(const AST::Query* query);
		virtual ~QueryChecker();

		bool IsExpressionSatisfied(const SymbolicMarking& marking) const;
		inline bool IsEF() const { return query->GetQuantifier() == VerifyTAPN::AST::EF; };
		inline bool IsAG() const { return query->GetQuantifier() == VerifyTAPN::AST::AG; };
		inline bool IsEG() const { return query->GetQuantifier() == VerifyTAPN::AST::EG; };
		inline bool IsAF() const { return query->GetQuantifier() == VerifyTAPN::AST::AF; };

	private:
		const AST::Query* query;
	};
}

#endif /* QUERYCHECKER_HPP_ */
