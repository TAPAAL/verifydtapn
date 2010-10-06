#ifndef DISCRETEPART_HPP_
#define DISCRETEPART_HPP_

#include "../TAPN/TimedPlace.hpp"

namespace VerifyTAPN {

class DiscretePart {
	public: // construction
		DiscretePart(const std::vector<int>& placement) : placement(placement) { };
		explicit DiscretePart(const DiscretePart& dp) : placement(dp.placement) { };

		virtual ~DiscretePart() { };

	public: // inspectors

	private: // data
		std::vector<int> placement;

};

}

#endif /* DISCRETEPART_HPP_ */
