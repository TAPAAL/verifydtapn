#ifndef SYMMARKING_HPP_
#define SYMMARKING_HPP_

#include "DiscretePart.hpp"

namespace VerifyTAPN {

// Class representing a symbolic marking.
class SymMarking {
public:
	public:// construction
		explicit SymMarking(const DiscretePart& dp) : dp(dp) { };
		virtual ~SymMarking() { };


	public: // inspectors

	public: // modifiers
		const DiscretePart* GetDiscretePart() const;

	private: // data
		DiscretePart dp;
		// DBM dbm;
};

}

#endif /* SYMMARKING_HPP_ */
