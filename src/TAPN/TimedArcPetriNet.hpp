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
			TimedArcPetriNet(const TimedPlace::Vector& places,
				const TimedTransition::Vector& transitions,
				const TimedInputArc::Vector& inputArcs,
				const OutputArc::Vector& outputArcs)
				: places(places), transitions(transitions), inputArcs(inputArcs), outputArcs(outputArcs) { };
			virtual ~TimedArcPetriNet() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;
			int GetPlaceIndex(const TimedPlace& p) const;
			int GetPlaceIndex(const std::string& placeName) const;
			const TimedTransition::Vector& GetTransitions() const { return transitions; }
			const TimedInputArc::Vector& GetInputArcs() const { return inputArcs; }
			const int GetNumberOfInputArcs() const { return inputArcs.size(); }
			const OutputArc::Vector& GetOutputArcs() const { return outputArcs; }
			const int GetNumberOfOutputArcs() const { return outputArcs.size(); }

		public: // modifiers
			void Initialize();

		private: // modifiers
			void MakeTAPNConservative();

		private: // data
			const TimedPlace::Vector places;
			const TimedTransition::Vector transitions;
			const TimedInputArc::Vector inputArcs;
			const OutputArc::Vector outputArcs;
		};

		inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
		{
			tapn.Print( out );
			return out;
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_ */
