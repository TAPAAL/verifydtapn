#ifndef TOKENMAPPING_HPP_
#define TOKENMAPPING_HPP_

#include <vector>
#include <limits>

namespace VerifyTAPN {
	// A token mapping maps a token index 0 <= i <= k to
	// a new token index in the current marking. Needed for active clock reduction.
	class TokenMapping {
		public: // Constructors
			TokenMapping(const std::vector<int>& mapping) : mapping(mapping) {};
			virtual ~TokenMapping() {};

		public: // inspectors
			const int GetMapping(int index) const;
		private:
			std::vector<int> mapping;
	};

}
#endif /* TOKENMAPPING_HPP_ */
