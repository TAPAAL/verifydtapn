#ifndef VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_
#define VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_

#include <iostream>
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"
#include "boost/make_shared.hpp"

namespace VerifyTAPN {
	namespace TAPN {

		class TimedArcPetriNet
		{
		public:// construction
			TimedArcPetriNet() : places(), transitions(), inputArcs(), outputArcs() {  };
//			TimedArcPetriNet(const boost::shared_ptr<TimedTransition::Vector>& transitions,
//				const boost::shared_ptr<TimedInputArc::Vector>& inputArcs,
//				const boost::shared_ptr<OutputArc::Vector>& outputArcs)
//				: places(), transitions(transitions), inputArcs(inputArcs), outputArcs(outputArcs) { };
			virtual ~TimedArcPetriNet() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;
			const TimedPlace& FindPlaceByName(const std::string& name) const;
			const TimedTransition& FindTransitionByName(const std::string& name) const;
			bool ContainsPlaceByName(const std::string& name) const;

		public: // modifiers
			void Initialize();
			void AddPlace(TimedPlace* place);
			void AddTransition(TimedTransition* transition);
			void AddInputArc(TimedInputArc* arc);
			void AddOutputArc(OutputArc* arc);

		private: // data
			TimedPlace::Vector places;
			TimedTransition::Vector transitions;
			TimedInputArc::Vector inputArcs;
			OutputArc::Vector outputArcs;
		};

		inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
		{
			tapn.Print( out );
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_ */
