/*
 * SuccessorGenerator.hpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#ifndef TIMEDARTSUCCESSORGENERATOR_HPP_
#define TIMEDARTSUCCESSORGENERATOR_HPP_

#include <google/sparse_hash_map>
#include <limits>
#include "../Core/TAPN/TAPN.hpp"
#include "VerificationTypes/Verification.hpp"
#include "DataStructures/NonStrictMarkingBase.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace std;
using namespace TAPN;

struct ArcRef {
    TokenList enabledBy;

    virtual ~ArcRef() {
    };
};

struct InputArcRef : ArcRef {

    InputArcRef(const TimedInputArc& arc) : arc(arc) {
    }
    const TimedInputArc& arc;
};

struct InhibitorArcRef : ArcRef {

    InhibitorArcRef(const InhibitorArc& arc) : arc(arc) {
    }
    const InhibitorArc& arc;
};

struct TransportArcRef : ArcRef {

    TransportArcRef(const TransportArc& arc) : arc(arc) {
    }
    const TransportArc& arc;
};

template<typename T>
class ArcAndTokens {
public:
    TokenList enabledBy;
    vector<unsigned int > modificationVector;
    bool isOK;

    ArcAndTokens(TokenList enabledBy, vector<unsigned int > modificationVector)
    : enabledBy(enabledBy), modificationVector(modificationVector) {
    }

    ArcAndTokens(TokenList enabledBy, int weight)
    : enabledBy(enabledBy), modificationVector(vector<unsigned int>(weight)) {
        isOK = this->reset();
    }

    virtual ~ArcAndTokens() {
    }

    virtual void moveToken(Token& token, T& m) = 0;

    void clearModificationVector() {
        modificationVector.clear();
    }

    bool reset() {
        int weight = modificationVector.size();

        int index = 0;
        // Construct available tokens
        for (vector<Token>::iterator placeTokensIter = enabledBy.begin();
                placeTokensIter != enabledBy.end() && index < weight;
                placeTokensIter++) {
            for (int i = 0; i < placeTokensIter->getCount() && index < weight; i++) {
                modificationVector[index] = distance(enabledBy.begin(), placeTokensIter);
                index++;
            }
        }

        if (index < weight) return false;
        return true;
    }
};

template<typename T>
class InputArcAndTokens : public ArcAndTokens<T> {
public:
    const TimedInputArc& arc;

    InputArcAndTokens(const TimedInputArc& arc, TokenList enabledBy, vector<unsigned int > modificationVector)
    : ArcAndTokens<T>(enabledBy, modificationVector), arc(arc) {
    }

    InputArcAndTokens(const TimedInputArc& arc, TokenList enabledBy)
    : ArcAndTokens<T>(enabledBy, arc.getWeight()), arc(arc) {
    }

    void moveToken(Token& token, T& m) {
        m.removeToken(arc.getInputPlace().getIndex(), token.getAge());
    }
};

template<typename T>
class TransportArcAndTokens : public ArcAndTokens<T> {
public:
    const TransportArc& arc;

    TransportArcAndTokens(const TransportArc& arc, TokenList enabledBy, vector<unsigned int > modificationVector)
    : ArcAndTokens<T>(enabledBy, modificationVector), arc(arc) {
    }

    TransportArcAndTokens(const TransportArc& arc, TokenList enabledBy)
    : ArcAndTokens<T>(enabledBy, arc.getWeight()), arc(arc) {
    }

    void moveToken(Token& token, T& m) {
        m.removeToken(arc.getSource().getIndex(), token.getAge());
        m.addTokenInPlace(arc.getDestination(), token);
    }
};

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
