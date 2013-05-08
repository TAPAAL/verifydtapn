#ifndef VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_
#define VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_

#include <iostream>
#include "TimedPlace.hpp"
#include "TimedTransition.hpp"
#include "TimedInputArc.hpp"
#include "TransportArc.hpp"
#include "InhibitorArc.hpp"
#include "OutputArc.hpp"
#include "boost/make_shared.hpp"
#include "google/sparse_hash_map"
#include "boost/functional/hash.hpp"
#include "../QueryParser/AST.hpp"
#include "../../DiscreteVerification/PlaceVisitor.hpp"
#include "../VerificationOptions.hpp"

namespace VerifyTAPN {

	namespace TAPN {

		class TimedArcPetriNet
		{

		public:// construction
			TimedArcPetriNet(const TimedPlace::Vector& places,
				const TimedTransition::Vector& transitions,
				const TimedInputArc::Vector& inputArcs,
				const OutputArc::Vector& outputArcs,
				const TransportArc::Vector& transportArcs,
				const InhibitorArc::Vector& inhibitorArcs)
				: places(places), transitions(transitions), inputArcs(inputArcs), outputArcs(outputArcs), transportArcs(transportArcs), inhibitorArcs(inhibitorArcs), maxConstant(0) { };
			virtual ~TimedArcPetriNet() { /* empty */ }

		public: // inspectors
			void print(std::ostream& out) const;
			inline int getPlaceIndex(const TimedPlace& p) const { return p.getIndex(); };
			int getPlaceIndex(const std::string& placeName) const;
			const TimedPlace& getPlace(const int placeIndex) const { return *places[placeIndex]; }
			const TimedTransition::Vector& getTransitions() const { return transitions; }
			const TimedInputArc::Vector& getInputArcs() const { return inputArcs; }
			const TransportArc::Vector& getTransportArcs() const { return transportArcs; }
			const InhibitorArc::Vector& getInhibitorArcs() const { return inhibitorArcs; }
			const TimedPlace::Vector& getPlaces() const { return places; };
			const int getNumberOfConsumingArcs() const { return inputArcs.size() + transportArcs.size(); }
			const OutputArc::Vector& getOutputArcs() const { return outputArcs; }
			const int getNumberOfOutputArcs() const { return outputArcs.size(); }
			int getNumberOfPlaces() const { return places.size(); };
			inline int getMaxConstant() const { return maxConstant; };
			inline const bool isPlaceAtIndexUntimed(int index) const { return places[index]->isUntimed(); }
			bool isNonStrict() const;
			void calculateCausality(TimedPlace& p, std::vector< TimedPlace* >* result) const;
			void updatePlaceTypes(const AST::Query* query, VerificationOptions options);
		public: // modifiers
			void initialize(bool useGlobalMaxConstant);
			void removeOrphantedTransitions();


		private: // modifiers
			void makeTAPNConservative();
			void updateMaxConstant(const TimeInterval& interval);
			void updateMaxConstant(const TimeInvariant& invariant);
			void markUntimedPlaces();
			void findMaxConstants();

		private: // data
			const TimedPlace::Vector places;
			TimedTransition::Vector transitions;
			const TimedInputArc::Vector inputArcs;
			const OutputArc::Vector outputArcs;
			const TransportArc::Vector transportArcs;
			const InhibitorArc::Vector inhibitorArcs;
			int maxConstant;
		};

		inline std::ostream& operator<<(std::ostream& out, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
		{
			tapn.print( out );
			return out;
		}

		inline std::size_t getHashValue(const TimedTransition& transition)
		{
			boost::hash<std::string> hasher;
			return hasher(transition.GetName());
		}
	}
}

#endif /* VERIFYTAPN_TAPN_TimedArcPetriNet_HPP_ */
