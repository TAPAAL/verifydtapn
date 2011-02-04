#ifndef VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_
#define VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_

#include <iostream>
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"
#include "TimedInputArc.hpp"
#include "OutputArc.hpp"
#include "boost/make_shared.hpp"
#include "google/sparse_hash_map"
#include "boost/functional/hash.hpp"
#include "Pairing.hpp"

namespace VerifyTAPN {

	namespace TAPN {

		class TimedArcPetriNet
		{
		public: // typedefs
			typedef google::sparse_hash_map<TimedTransition, VerifyTAPN::Pairing, boost::hash<TAPN::TimedTransition> > HashMap;
		public:// construction
			TimedArcPetriNet(const TimedPlace::Vector& places,
				const TimedTransition::Vector& transitions,
				const TimedInputArc::Vector& inputArcs,
				const OutputArc::Vector& outputArcs)
				: places(places), transitions(transitions), inputArcs(inputArcs), outputArcs(outputArcs), maxConstant(0) { };
			virtual ~TimedArcPetriNet() { /* empty */ }

		public: // inspectors
			void Print(std::ostream& out) const;
			inline int GetPlaceIndex(const TimedPlace& p) const { return p.GetIndex(); };
			int GetPlaceIndex(const std::string& placeName) const;
			const TimedPlace& GetPlace(const int placeIndex) const { return *places[placeIndex]; }
			const TimedTransition::Vector& GetTransitions() const { return transitions; }
			const TimedInputArc::Vector& GetInputArcs() const { return inputArcs; }
			const int GetNumberOfInputArcs() const { return inputArcs.size(); }
			const OutputArc::Vector& GetOutputArcs() const { return outputArcs; }
			const int GetNumberOfOutputArcs() const { return outputArcs.size(); }
			const Pairing& GetPairing(const TimedTransition& t) const { return pairings.find(t)->second; }
			inline int MaxConstant() const { return maxConstant; };
			inline const bool IsPlaceInfinityPlace(int index) const { return places[index]->IsInfinityPlace(); }
		public: // modifiers
			void Initialize(bool useInfinityPlaces);


		private: // modifiers
			void MakeTAPNConservative();
			void GeneratePairings();
			void UpdateMaxConstant(const TimeInterval& interval);
			void MarkInfinityPlaces();
			void FindMaxConstants();

		private: // data
			const TimedPlace::Vector places;
			const TimedTransition::Vector transitions;
			const TimedInputArc::Vector inputArcs;
			const OutputArc::Vector outputArcs;
			mutable HashMap pairings;
			int maxConstant;
		};

		inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
		{
			tapn.Print( out );
			return out;
		}

		inline std::size_t hash_value(const TimedTransition& transition)
		{
			boost::hash<std::string> hasher;
			return hasher(transition.GetName());
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_ */
