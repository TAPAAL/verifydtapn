#ifndef VISITOR_HPP_
#define VISITOR_HPP_

#include <vector>

namespace VerifyTAPN
{
	namespace AST
	{
		class NotExpression;
		class ParExpression;
		class OrExpression;
		class AndExpression;
		class AtomicProposition;
                class DeadlockExpression;
		class BoolExpression;
		class Query;
                
                class Result {};
                template<typename T>
                class SpecificResult : public Result { public: T value; };
                typedef SpecificResult<int> IntResult;
                typedef SpecificResult<bool> BoolResult;
                typedef SpecificResult<std::vector<int> > IntVectorResult;
                
		class Visitor
		{
		public:
			virtual ~Visitor() { };
			virtual void visit(const NotExpression& expr, Result& context) = 0;
			virtual void visit(const ParExpression& expr, Result& context) = 0;
			virtual void visit(const OrExpression& expr, Result& context) = 0;
			virtual void visit(const AndExpression& expr, Result& context) = 0;
			virtual void visit(const AtomicProposition& expr, Result& context) = 0;
			virtual void visit(const BoolExpression& expr, Result& context) = 0;
			virtual void visit(const Query& query, Result& context) = 0;
                        virtual void visit(const DeadlockExpression& expr, Result& context) = 0;
		};
	}
}


#endif /* VISITOR_HPP_ */
