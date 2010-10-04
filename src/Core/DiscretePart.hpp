#ifndef DISCRETEPART_HPP_
#define DISCRETEPART_HPP_

#include "../TAPN/TimedPlace.hpp"

namespace VerifyTAPN {

class DiscretePart {
	public: // construction
		DiscretePart(const VerifyTAPN::TAPN::TimedPlace::Vector& placement) : placement(placement) { };
		virtual ~DiscretePart() { };

	private: // data
		VerifyTAPN::TAPN::TimedPlace::Vector placement;
};

}

#endif /* DISCRETEPART_HPP_ */
