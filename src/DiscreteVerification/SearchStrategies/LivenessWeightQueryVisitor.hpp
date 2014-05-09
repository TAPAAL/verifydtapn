/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef LIVEWEIGHTQUERYVISITOR_HPP_
#define LIVEWEIGHTQUERYVISITOR_HPP_

#include "../../Core/QueryParser/Visitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include <exception>
#include <limits.h>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class LivenessWeightQueryVisitor : public Visitor{
	public:
	LivenessWeightQueryVisitor(NonStrictMarkingBase& marking) : marking(marking){};
		virtual ~LivenessWeightQueryVisitor(){};

	public: // visitor methods
		virtual void visit(const NotExpression& expr, Result& context);
		virtual void visit(const OrExpression& expr, Result& context);
		virtual void visit(const AndExpression& expr, Result& context);
		virtual void visit(const AtomicProposition& expr, Result& context);
		virtual void visit(const BoolExpression& expr, Result& context);
		virtual void visit(const Query& query, Result& context);
                virtual void visit(const DeadlockExpression& expr, Result& context);
                virtual void visit(const NumberExpression& expr, Result& context);
                virtual void visit(const IdentifierExpression& expr, Result& context);
                virtual void visit(const MultiplyExpression& expr, Result& context);
                virtual void visit(const MinusExpression& expr, Result& context);
                virtual void visit(const SubtractExpression& expr, Result& context);
                virtual void visit(const PlusExpression& expr, Result& context);
	private:
		int compare(int numberOfTokensInPlace, const std::string& op, int n) const;

	private:
		const NonStrictMarkingBase& marking;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* LIVENESSQUERYVISITOR_HPP_ */
