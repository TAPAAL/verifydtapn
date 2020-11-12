#ifndef VERIFYTAPN_TAPN_OUTPUTARC_HPP_
#define VERIFYTAPN_TAPN_OUTPUTARC_HPP_

#include <vector>
#include <memory>

namespace VerifyTAPN {
	namespace TAPN {
		class TimedPlace;
		class TimedTransition;

		class OutputArc
		{
		public: // typedefs
			typedef std::vector< std::shared_ptr<OutputArc> > Vector;
			typedef std::vector< std::weak_ptr<OutputArc> > WeakPtrVector;
		public:
			OutputArc(const std::shared_ptr<TimedTransition>& transition, const std::shared_ptr<TimedPlace>& place)
				: transition(transition), place(place) { };
			virtual ~OutputArc() { /* empty */ }

		public: // modifiers
			TimedPlace& OutputPlace();
			TimedTransition& InputTransition();

		public: // inspectors
			void Print(std::ostream& out) const;
		private:
			const std::shared_ptr<TimedTransition> transition;
			const std::shared_ptr<TimedPlace> place;
		};

		inline std::ostream& operator<<(std::ostream& out, const OutputArc& arc)
		{
			arc.Print(out);
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_OUTPUTARC_HPP_ */
