#ifndef UPWARDCLOSEDVISITOR_HPP_
#define UPWARDCLOSEDVISITOR_HPP_

#include "Visitor.hpp"

namespace VerifyTAPN {
namespace AST {
	class UpwardClosedVisitor: public VerifyTAPN::AST::Visitor {
	public:
		UpwardClosedVisitor() : propShouldBeUpward(true) { };
		virtual ~UpwardClosedVisitor() {};
		virtual void Visit(const ParExpression& expr, boost::any& context);
		virtual void Visit(const OrExpression& expr, boost::any& context);
		virtual void Visit(const AndExpression& expr, boost::any& context);
		virtual void Visit(const AtomicProposition& expr, boost::any& context);
		virtual void Visit(const Query& query, boost::any& context);
		bool IsUpwardClosed(const Query& query);

	private:
		bool propShouldBeUpward;
	};
}
}

#endif /* UPWARDCLOSEDVISITOR_HPP_ */
