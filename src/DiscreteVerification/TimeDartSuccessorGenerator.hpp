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
#include "SuccessorGenerator.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace std;
using namespace TAPN;

class TimeDartSuccessorGenerator {
	typedef google::sparse_hash_map<const void*, TokenList> ArcHashMap;
	typedef std::vector< ArcAndTokens<NonStrictMarkingBase>*> ArcAndTokensVector;

public:
	TimeDartSuccessorGenerator(TAPN::TimedArcPetriNet& tapn, Verification<NonStrictMarkingBase>& verifier);
	~TimeDartSuccessorGenerator();
	bool generateAndInsertSuccessors(const NonStrictMarkingBase& marking, const TimedTransition& transition) const;
	void printTransitionStatistics(std::ostream & out) const;
        
private:

	bool generateMarkings(const NonStrictMarkingBase& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs) const;
	bool generatePermutations(NonStrictMarkingBase& init_marking, const TimedTransition& transition, unsigned int index, ArcHashMap& enabledArcs) const;

	bool insertMarking(NonStrictMarkingBase& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const;
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

    inline void clearTransitionsArray() {
    	memset(transitionStatistics, 0, numberoftransitions * sizeof (transitionStatistics[0]));
    }

    unsigned int numberoftransitions;
        unsigned int* transitionStatistics;
        Verification<NonStrictMarkingBase>& verifier;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* SUCCESSORGENERATOR_HPP_ */
