#ifndef TRANSPORTARC_HPP_
#define TRANSPORTARC_HPP_

#include <vector>
#include "TimeInterval.hpp"
#include "boost/smart_ptr.hpp"

namespace VerifyTAPN
{
	namespace TAPN
	{
		class TimedTransition;
		class TimedPlace;

		class TransportArc
		{
		public:
			typedef std::vector< boost::shared_ptr<TransportArc> > Vector;
			typedef std::vector< boost::weak_ptr<TransportArc> > WeakPtrVector;
		public:
			TransportArc(
					const boost::shared_ptr<TimedPlace>& source,
					const boost::shared_ptr<TimedTransition>& transition,
					const boost::shared_ptr<TimedPlace>& destination,
					const TAPN::TimeInterval& interval,
					const int weight
			) : interval(interval), source(source), transition(transition), destination(destination), weight(weight) {};

			virtual ~TransportArc() {};
		public:
			inline TimedPlace& getSource() { return *source; }
			inline TimedTransition& getTransition() { return *transition; }
			inline TimedPlace& getDestination() { return *destination; }
			inline const TimeInterval& getInterval() { return interval; }

		public: // Inspectors
				void print(std::ostream& out) const;
				inline const int getWeight() const { return weight; }
		private:
			const TAPN::TimeInterval interval;
			const boost::shared_ptr<TimedPlace> source;
			const boost::shared_ptr<TimedTransition> transition;
			const boost::shared_ptr<TimedPlace> destination;
			const int weight;
		};

		inline std::ostream& operator<<(std::ostream& out, const TransportArc& arc)
		{
			arc.print(out);
			return out;
		}
	}
}

#endif /* TRANSPORTARC_HPP_ */
