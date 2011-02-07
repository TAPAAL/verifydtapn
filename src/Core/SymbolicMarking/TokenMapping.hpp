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
			explicit TokenMapping(const std::vector<unsigned int>& mapping) : mapping(mapping) {};
			virtual ~TokenMapping() {};

		public: // inspectors
			inline const unsigned int GetMapping(unsigned int index) const { return mapping[index]; }
			const std::vector<unsigned int>& GetMappingVector() const { return mapping; }
			const unsigned int size() const { return mapping.size(); }

		public: // modifiers
			void AddTokenToMapping(unsigned int tokenIndex);
			void SetMapping(unsigned int index, unsigned int dbmIndex) { mapping[index] = dbmIndex; }
			void RemoveToken(unsigned int index) { mapping.erase(mapping.begin() + index); }
		private:
			std::vector<unsigned int> mapping;
	};

}
#endif /* TOKENMAPPING_HPP_ */
