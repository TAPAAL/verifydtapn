#ifndef QUERYCHECKER_HPP_
#define QUERYCHECKER_HPP_

namespace VerifyTAPN
{
	class SymMarking;
	namespace AST{
		class Query;
	}

	class QueryChecker
	{
	public:
		QueryChecker(const AST::Query* query);
		virtual ~QueryChecker();

		bool IsExpressionSatisfied(const SymMarking& marking) const;
		bool IsEF() const;
		bool IsAG() const;

	private:
		const AST::Query* query;
	};
}

#endif /* QUERYCHECKER_HPP_ */
