/*
 * PlaceVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef PLACEVISITOR_HPP_
#define PLACEVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class PlaceVisitor : public Visitor{
	public:
		PlaceVisitor(){};
		virtual ~PlaceVisitor(){};

	public: // visitor methods
		virtual void Visit(const NotExpression& expr, boost::any& context);
		virtual void Visit(const ParExpression& expr, boost::any& context);
		virtual void Visit(const OrExpression& expr, boost::any& context);
		virtual void Visit(const AndExpression& expr, boost::any& context);
		virtual void Visit(const AtomicProposition& expr, boost::any& context);
		virtual void Visit(const BoolExpression& expr, boost::any& context);
		virtual void Visit(const Query& query, boost::any& context);
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PLACEVISITOR_HPP_ */
