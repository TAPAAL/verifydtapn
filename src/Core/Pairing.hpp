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

		inline std::size_t hash_value(TAPN::TimedPlace const& inputPlace)
		{
			boost::hash<std::string> hasher;
			return hasher(inputPlace.GetName());
		}

	}

	class Pairing {
		public:
			typedef google::sparse_hash_map<TAPN::TimedPlace, std::list<TAPN::TimedPlace>, boost::hash<TAPN::TimedPlace> > HashMap;
		public: // construction
			Pairing(const TAPN::TimedTransition& t) : pairing() { GeneratePairingFor(t); };
			Pairing() : pairing() { };



		public: // inspectors
			std::list<TAPN::TimedPlace>& GetOutputPlacesFor(const TAPN::TimedPlace& inputPlace);
			bool IsPairingEmpty() const { return pairing.empty(); }
			void Print(std::ostream& out) const;

		public: // modifiers
			void Add(const TAPN::TimedPlace& inputPlace, const TAPN::TimedPlace& outputPlace);


		private: // initializers
			void GeneratePairingFor(const TAPN::TimedTransition& t);

		private: // data
			HashMap pairing;

	};

	inline std::ostream& operator<<(std::ostream& out, const Pairing& pairing)
	{
		pairing.Print(out);
		return out;
	}

}

#endif /* PAIRING_HPP_ */
