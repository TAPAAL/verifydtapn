/*
 * SuccessorGenerator.hpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTSUCCESSORGENERATOR_HPP_
#define TIMEDARTSUCCESSORGENERATOR_HPP_

#include "../Core/TAPN/TAPN.hpp"
#include "DataStructures/NonStrictMarkingBase.hpp"
#include "google/sparse_hash_map"
#include <limits>
#include "boost/tuple/tuple_io.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
#include "SuccessorGenerator.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace std;
using namespace TAPN;
using namespace boost;

class TimeDartSuccessorGenerator {
	typedef google::sparse_hash_map<const void*, TokenList> ArcHashMap;
	typedef boost::ptr_vector< ArcAndTokens<NonStrictMarkingBase> > ArcAndTokensVector;

public:
	TimeDartSuccessorGenerator(TAPN::TimedArcPetriNet& tapn);
	~TimeDartSuccessorGenerator();
	bool generateSuccessors(const NonStrictMarkingBase& marking, const TimedTransition& transition) const;
	void PrintTransitionStatistics(std::ostream & out) const;
        Verification<NonStrictMarkingBase>* verifier;
private:
	TokenList getPlaceFromMarking(const NonStrictMarkingBase& marking, int placeID) const;

	bool generateMarkings(const NonStrictMarkingBase& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs) const;
	bool recursiveGenerateMarking(NonStrictMarkingBase& init_marking, const TimedTransition& transition, unsigned int index, ArcHashMap& enabledArcs) const;

	bool addMarking(NonStrictMarkingBase& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const;
	bool incrementModificationVector(vector<unsigned int >& modificationVector, TokenList& enabledTokens) const;

	const TAPN::TimedArcPetriNet& tapn;
	vector<const TAPN::TimedTransition*> allwaysEnabled;
	void processArc(
			ArcHashMap& enabledArcs,
					const TokenList& place,
					const TAPN::TimeInterval& interval,
					const void* arcAddress,
					const TimedTransition& transition,
					int bound = INT_MAX
			) const;

    inline void ClearTransitionsArray() {
    	memset(transitionStatistics, 0, numberoftransitions * sizeof (transitionStatistics[0]));
    }

    unsigned int numberoftransitions;
	unsigned int* transitionStatistics;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* SUCCESSORGENERATOR_HPP_ */
