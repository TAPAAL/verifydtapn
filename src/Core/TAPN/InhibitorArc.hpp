#ifndef INHIBITORARC_HPP_
#define INHIBITORARC_HPP_

#include <vector>
#include "TimeInterval.hpp"
#include "boost/smart_ptr.hpp"

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedPlace;

		class InhibitorArc {
			public: // typedefs
				typedef std::vector< boost::shared_ptr<InhibitorArc> > Vector;
				typedef std::vector< boost::weak_ptr<InhibitorArc> > WeakPtrVector;
			public:
				InhibitorArc(const boost::shared_ptr<TimedPlace>& place, const boost::shared_ptr<TimedTransition>& transition, const int weight) : place(place), transition(transition), weight(weight) { };
				virtual ~InhibitorArc() { /* empty */ }

			public: // modifiers
				inline TimedPlace& getInputPlace() { return *place; }
				inline TimedTransition& getOutputTransition() { return *transition; }

			public: // Inspectors
				void print(std::ostream& out) const;
				inline const int getWeight() const { return weight; }
			private:
				const boost::shared_ptr<TimedPlace> place;
				const boost::shared_ptr<TimedTransition> transition;
				const int weight;
		};

		inline std::ostream& operator<<(std::ostream& out, const InhibitorArc& arc)
		{
			arc.print(out);
			return out;
		}
	}

}

#endif /* INHIBITORARC_HPP_ */
