#ifndef SYMMARKING_HPP_
#define SYMMARKING_HPP_

#include "../TAPN/TimedPlace.hpp"


namespace VerifyTAPN {

class SymMarking {
public:
	public:// construction
		SymMarking(const TAPN::TimedPlace::Vector& placement) : placement(placement) { };
		virtual ~SymMarking() { };


	public: // inspectors

	public: // modifiers


	private: // data
		TAPN::TimedPlace::Vector placement;
		// DBM dbm;
};

}

#endif /* SYMMARKING_HPP_ */
