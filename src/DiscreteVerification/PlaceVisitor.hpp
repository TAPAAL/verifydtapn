/*
 * PlaceVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef PLACEVISITOR_HPP_
#define PLACEVISITOR_HPP_

#include "../Core/QueryParser/Visitor.hpp"
#include "../Core/QueryParser/AST.hpp"
#include <exception>
#include <vector>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class PlaceVisitor : public Visitor{
	public:
		PlaceVisitor(){};
		virtual ~PlaceVisitor(){};

	public: // visitor methods
		virtual void visit(const NotExpression& expr, boost::any& context);
		virtual void visit(const ParExpression& expr, boost::any& context);
		virtual void visit(const OrExpression& expr, boost::any& context);
		virtual void visit(const AndExpression& expr, boost::any& context);
		virtual void visit(const AtomicProposition& expr, boost::any& context);
		virtual void visit(const BoolExpression& expr, boost::any& context);
		virtual void visit(const Query& query, boost::any& context);
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* PLACEVISITOR_HPP_ */
