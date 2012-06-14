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
#include "boost/tuple/tuple_io.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
#define DEBUG 0

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace std;
using namespace TAPN;
using namespace boost;

struct ArcRef{
	TokenList enabledBy;
	virtual ~ArcRef(){};
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

struct ArcAndTokens{
	TokenList enabledBy;
	vector<unsigned int > modificationVector;
	bool isOK;

	ArcAndTokens(TokenList enabledBy, vector<unsigned int > modificationVector)
	: enabledBy(enabledBy), modificationVector(modificationVector){}

	ArcAndTokens(TokenList enabledBy, int weight)
	: enabledBy(enabledBy), modificationVector(vector<unsigned int>(weight)){
		isOK = this->reset();
	}

	virtual ~ArcAndTokens(){}

	virtual void moveToken(Token& token, NonStrictMarking& m) = 0;

	void clearModificationVector(){
		modificationVector.clear();
	}

	bool reset(){
		int weight = modificationVector.size();

		int index = 0;
		// Construct available tokens
		for(vector<Token>::iterator placeTokensIter = enabledBy.begin();
				placeTokensIter != enabledBy.end() && index < weight;
				placeTokensIter++){
			for(int i = 0; i < placeTokensIter->getCount() && index <  weight; i++){
				modificationVector[index] = distance(enabledBy.begin(), placeTokensIter);
				index++;
			}
		}

		if(index < weight)	return false;
		return true;
	}
};

struct InputArcAndTokens : ArcAndTokens{
	boost::weak_ptr<TimedInputArc> arc;

	InputArcAndTokens(boost::weak_ptr<TimedInputArc> arc, TokenList enabledBy, vector<unsigned int > modificationVector)
	: ArcAndTokens(enabledBy, modificationVector), arc(arc){}

	InputArcAndTokens(boost::weak_ptr<TimedInputArc> arc, TokenList enabledBy)
		: ArcAndTokens(enabledBy, arc.lock()->GetWeight()), arc(arc){}

	void moveToken(Token& token, NonStrictMarking& m){
		m.RemoveToken(arc.lock()->InputPlace().GetIndex(), token.getAge());
	}
};

struct TransportArcAndTokens : ArcAndTokens{
	boost::weak_ptr<TransportArc> arc;

	TransportArcAndTokens(boost::weak_ptr<TransportArc> arc, TokenList enabledBy, vector<unsigned int > modificationVector)
	: ArcAndTokens(enabledBy, modificationVector),  arc(arc){}

	TransportArcAndTokens(boost::weak_ptr<TransportArc> arc, TokenList enabledBy)
		: ArcAndTokens(enabledBy, arc.lock()->GetWeight()),  arc(arc){}

	void moveToken(Token& token, NonStrictMarking& m){
		m.RemoveToken(arc.lock()->Source().GetIndex(), token.getAge());
		m.AddTokenInPlace(arc.lock()->Destination(), token);
	}
};

class SuccessorGenerator {
	typedef google::sparse_hash_map<const void*, TokenList> ArcHashMap;
	typedef boost::ptr_vector< ArcAndTokens > ArcAndTokensVector;

public:
	SuccessorGenerator(TAPN::TimedArcPetriNet& tapn);
	~SuccessorGenerator();
	vector< NonStrictMarking > generateSuccessors(const NonStrictMarking& marking) const;
private:
	TokenList getPlaceFromMarking(const NonStrictMarking& marking, int placeID) const;

	void generatePermultations(vector< NonStrictMarking >& result, NonStrictMarking& init_marking, int placeID, TokenList enabledBy, int tokenToProcess, int remainingToRemove, TimedPlace* destinationPlace = NULL) const;
	void generateMarkings(vector<NonStrictMarking >& result, const NonStrictMarking& init_marking, const std::vector< const TimedTransition* >& transitions, ArcHashMap& enabledArcs) const;
	void recursiveGenerateMarking(vector<NonStrictMarking >& result, NonStrictMarking& init_marking, const TimedTransition& transition, ArcHashMap& enabledArcs, unsigned int index) const;

	void addMarking(vector<NonStrictMarking >& result, NonStrictMarking& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const;
	bool incrementModificationVector(vector<unsigned int >& modificationVector, TokenList& enabledTokens) const;

	const TAPN::TimedArcPetriNet& tapn;
	vector<const TAPN::TimedTransition*> allwaysEnabled;
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
