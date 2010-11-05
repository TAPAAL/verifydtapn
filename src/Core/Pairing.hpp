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
			Pairing(const TAPN::TimedArcPetriNet& tapn, const TAPN::TimedTransition& t) : pairing() { GeneratePairingFor(tapn, t); };
			Pairing() : pairing() { };

		public: // inspectors
			const std::list<int>& GetOutputPlacesFor(int inputPlace) const;
			inline bool IsPairingEmpty() const { return pairing.empty(); }
			void Print(std::ostream& out) const;

		private: // initializers
			void GeneratePairingFor(const TAPN::TimedArcPetriNet& tapn, const TAPN::TimedTransition& t);
			void Add(int inputPlace, int outputPlace);

		private: // data
			mutable HashMap pairing;

	};

	inline std::ostream& operator<<(std::ostream& out, const Pairing& pairing)
	{
		pairing.Print(out);
		return out;
	}

}

#endif /* PAIRING_HPP_ */
