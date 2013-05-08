#ifndef PAIRING_HPP_
#define PAIRING_HPP_
#include "google/sparse_hash_map"
#include "../TAPN/TimedPlace.hpp"
#include "boost/functional/hash.hpp"
#include <string>
#include <list>

namespace VerifyTAPN {
	namespace TAPN {
		class TimedTransition;
		class TimedArcPetriNet;
	}

	class Pairing {
		public:
			typedef google::sparse_hash_map<int, std::list<int>, boost::hash<int> > HashMap;
		public: // construction
			Pairing(const TAPN::TimedArcPetriNet& tapn, const TAPN::TimedTransition& t) : pairing() { generatePairingFor(tapn, t); };
			Pairing() : pairing() { };

		public: // inspectors
			const std::list<int>& getOutputPlacesFor(int inputPlace) const;
			inline bool isPairingEmpty() const { return pairing.empty(); }
			void print(std::ostream& out) const;

		private: // initializers
			void generatePairingFor(const TAPN::TimedArcPetriNet& tapn, const TAPN::TimedTransition& t);
			void add(int inputPlace, int outputPlace);

		private: // data
			mutable HashMap pairing;

	};

	inline std::ostream& operator<<(std::ostream& out, const Pairing& pairing)
	{
		pairing.print(out);
		return out;
	}

}

#endif /* PAIRING_HPP_ */
