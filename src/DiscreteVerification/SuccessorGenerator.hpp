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
public:
	SuccessorGenerator(TAPN::TimedArcPetriNet& tapn) : tapn(tapn) {};
	~SuccessorGenerator();
	vector< NonStrictMarking > generateSuccessors(const NonStrictMarking& marking) const;
private:
	TokenList getPlaceFromMarking(const NonStrictMarking& marking, int placeID) const;
	void generateMarkings(vector<NonStrictMarking>& result, const NonStrictMarking& init_marking, const TimedTransition& transition, vector<InputArcRef> inputArcs, vector<TransportArcRef> transportArcs) const;
	void recursiveGenerateMarking(vector<NonStrictMarking>& result, NonStrictMarking& init_marking, const TimedTransition& transition, vector<InputArcRef> inputArcs, vector<TransportArcRef> transportArcs, int index) const;
	const TAPN::TimedArcPetriNet& tapn;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* SUCCESSORGENERATOR_HPP_ */
