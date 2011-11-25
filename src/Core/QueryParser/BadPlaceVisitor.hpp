#ifndef BADPLACEVISITOR_HPP_
#define BADPLACEVISITOR_HPP_

#include <vector>
#include "Visitor.hpp"
#include "AST.hpp"

namespace VerifyTAPN {
	namespace AST {

		class BadPlaceVisitor : public Visitor
		{
		public:
			BadPlaceVisitor() : badPlaces() { };
			virtual ~BadPlaceVisitor() {};
			virtual void Visit(const NotExpression& expr, boost::any& context);
			virtual void Visit(const ParExpression& expr, boost::any& context);
			virtual void Visit(const OrExpression& expr, boost::any& context);
			virtual void Visit(const AndExpression& expr, boost::any& context);
			virtual void Visit(const AtomicProposition& expr, boost::any& context);
			virtual void Visit(const BoolExpression& expr, boost::any& context);
			virtual void Visit(const Query& query, boost::any& context);

			void FindBadPlaces(const Query& query){ boost::any any; query.Accept(*this, any); };
			inline std::vector<int>& GetBadPlaces(){ return badPlaces; };
		private:
			std::vector<int> badPlaces;
		};

	}
}

#endif /* BADPLACEVISITOR_HPP_ */
