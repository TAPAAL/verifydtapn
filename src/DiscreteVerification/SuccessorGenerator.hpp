/*
 * SuccessorGenerator.hpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#ifndef SUCCESSORGENERATOR_HPP_
#define SUCCESSORGENERATOR_HPP_

#include "../Core/TAPN/TAPN.hpp"
#include "NonStrictMarking.hpp"
#include "google/sparse_hash_map"
#include <limits>
#define DEBUG 0

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace std;
using namespace TAPN;

struct ArcRef{
	TokenList enabledBy;
};

struct InputArcRef : ArcRef{
	InputArcRef(boost::weak_ptr<TimedInputArc> arc) : arc(arc) {}
	boost::weak_ptr<TimedInputArc> arc;
};

struct InhibitorArcRef : ArcRef{
	InhibitorArcRef(boost::weak_ptr<InhibitorArc> arc) : arc(arc) {}
	boost::weak_ptr<InhibitorArc> arc;
};

struct TransportArcRef : ArcRef{
	TransportArcRef(boost::weak_ptr<TransportArc> arc) : arc(arc) {}
	boost::weak_ptr<TransportArc> arc;
};

class SuccessorGenerator {
	typedef google::sparse_hash_map<const void*, TokenList> ArcHashMap;

public:
	SuccessorGenerator(TAPN::TimedArcPetriNet& tapn) : tapn(tapn) {};
	~SuccessorGenerator();
	vector< NonStrictMarking > generateSuccessors(const NonStrictMarking& marking) const;
private:
	TokenList getPlaceFromMarking(const NonStrictMarking& marking, int placeID) const;
	void generateMarkings(vector<NonStrictMarking >& result, const NonStrictMarking& init_marking, const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs) const;
	void recursiveGenerateMarking(vector<NonStrictMarking >& result, NonStrictMarking& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) const;
	const TAPN::TimedArcPetriNet& tapn;
	void processArc(
			ArcHashMap& enabledArcs,
			std::vector<unsigned int>& enabledTransitionArcs,
			std::vector<const TAPN::TimedTransition*>& enabledTransitions,
			const Place& place,
			const TAPN::TimeInterval& interval,
			const void* arcAddress,
			const TimedTransition& transition,
			int bound = std::numeric_limits<int>().max(),
			bool isInhib = false
	) const;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* SUCCESSORGENERATOR_HPP_ */
