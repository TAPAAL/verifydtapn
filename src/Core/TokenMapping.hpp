#ifndef TOKENMAPPING_HPP_
#define TOKENMAPPING_HPP_

#include <vector>
#include <limits>

namespace VerifyTAPN {
	// A token mapping maps a token index 0 <= i <= dbm dimension in the DBM to
	// a new token index in the current marking. Needed for active clock reduction.
	class TokenMapping {
		public: // Constructors
			TokenMapping() : mapping() {};
			TokenMapping(const std::vector<int>& mapping) : mapping(mapping) {};
			virtual ~TokenMapping() {};

		public: // inspectors
			inline const int GetMapping(int index) const { return mapping[index]; }
			const std::vector<int>& GetMappingVector() const { return mapping; }
			const unsigned int size() const { return mapping.size(); }

		public: // modifiers
			void AddTokenToMapping(int tokenIndex);
			void SetMapping(int index, int dbmIndex) { mapping[index] = dbmIndex; }
			void RemoveToken(int index) { mapping.erase(mapping.begin() + index); }
		private:
			std::vector<int> mapping;
	};

}
#endif /* TOKENMAPPING_HPP_ */
