/*
 * SuccessorGenerator.cpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartSuccessorGenerator.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

TimeDartSuccessorGenerator::~TimeDartSuccessorGenerator(){

}

TimeDartSuccessorGenerator::TimeDartSuccessorGenerator(TAPN::TimedArcPetriNet& tapn, Verification<NonStrictMarkingBase>& verifier)  : tapn(tapn), allwaysEnabled(), numberoftransitions(tapn.getTransitions().size()), transitionStatistics(), verifier(verifier){
	//Find the transitions which don't have input arcs
	transitionStatistics = new unsigned int [numberoftransitions];
	clearTransitionsArray();
	for(TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin(); iter != tapn.getTransitions().end(); iter++){
		if((*iter)->GetPreset().size() + (*iter)->GetTransportArcs().size() == 0){
			allwaysEnabled.push_back(iter->get());
		}
	}
}

bool TimeDartSuccessorGenerator::generateAndInsertSuccessors(const NonStrictMarkingBase& marking, const TimedTransition& transition) const{

	ArcHashMap enabledArcs(transition.GetPresetSize() + transition.GetTransportArcs().size());

	// Calculate enabling tokens
	for(TAPN::TimedInputArc::WeakPtrVector::const_iterator arc_iter = transition.GetPreset().begin();
			arc_iter != transition.GetPreset().end(); arc_iter++){
			processArc(enabledArcs,	marking.getTokenList( arc_iter->lock()->getInputPlace().GetIndex() ), arc_iter->lock()->getInterval(), arc_iter->lock().get(), transition);
	}

	for(TAPN::TransportArc::WeakPtrVector::const_iterator arc_iter = transition.GetTransportArcs().begin();
			arc_iter != transition.GetTransportArcs().end(); arc_iter++){
			processArc(enabledArcs,	marking.getTokenList( arc_iter->lock()->Source().GetIndex() ), arc_iter->lock()->Interval(), arc_iter->lock().get(), transition, arc_iter->lock()->Destination().GetInvariant().getBound());
	}

	return generateMarkings(marking, transition, enabledArcs);

}

void TimeDartSuccessorGenerator::processArc(
		ArcHashMap& enabledArcs,
		const TokenList& tokens,
		const TAPN::TimeInterval& interval,
		const void* arcAddress,
		const TimedTransition& transition,
		int bound
) const{
	for(TokenList::const_iterator token_iter = tokens.begin();	token_iter != tokens.end(); token_iter++){
		if(interval.getLowerBound() <= token_iter->getAge() && token_iter->getAge() <= interval.getUpperBound() && token_iter->getAge() <= bound){
			enabledArcs[arcAddress].push_back(*token_iter);
		}
	}
}

bool TimeDartSuccessorGenerator::generateMarkings( const NonStrictMarkingBase& init_marking,
		const TimedTransition& transition, ArcHashMap& enabledArcs) const {

		bool inhibited = false;
		//Check that no inhibitors is enabled;

		for(TAPN::InhibitorArc::WeakPtrVector::const_iterator inhib_iter = transition.GetInhibitorArcs().begin(); inhib_iter != transition.GetInhibitorArcs().end(); inhib_iter++){
			// Maybe this could be done more efficiently using ArcHashMap? Dunno exactly how it works
			if(init_marking.numberOfTokensInPlace(inhib_iter->lock().get()->getInputPlace().GetIndex()) >= inhib_iter->lock().get()->getWeight()){
				inhibited = true;
				break;
			}
		}
		if (inhibited) return false;

		NonStrictMarkingBase m(init_marking);
		m.setGeneratedBy(&transition);
		//Generate markings for transition
		return generatePermutations( m, transition, 0, enabledArcs);
}


bool TimeDartSuccessorGenerator::generatePermutations(NonStrictMarkingBase& init_marking, const TimedTransition& transition, unsigned int index, ArcHashMap& enabledArcs) const{

	// Initialize vectors
	ArcAndTokensVector indicesOfCurrentPermutation;
	for(TimedInputArc::WeakPtrVector::const_iterator iter = transition.GetPreset().begin(); iter != transition.GetPreset().end(); iter++){
		InputArcAndTokens<NonStrictMarkingBase>* arcAndTokens = new InputArcAndTokens<NonStrictMarkingBase>(*iter, enabledArcs[iter->lock().get()]);
		if(arcAndTokens->isOK){
			indicesOfCurrentPermutation.push_back(arcAndTokens);
		}else{
			return false;
		}
	}
	// Transport arcs
	for(TransportArc::WeakPtrVector::const_iterator iter = transition.GetTransportArcs().begin(); iter != transition.GetTransportArcs().end(); iter++){
		TransportArcAndTokens<NonStrictMarkingBase>* arcAndTokens = new TransportArcAndTokens<NonStrictMarkingBase>(*iter, enabledArcs[iter->lock().get()]);
		if(arcAndTokens->isOK){
			indicesOfCurrentPermutation.push_back(arcAndTokens);
		}else{
			return false;
		}
	}

	// Write statistics
	transitionStatistics[transition.GetIndex()]++;

	// Generate permutations
	bool changedSomething = true;
	while(changedSomething){
		changedSomething = false;
		if(insertMarking(init_marking, transition, indicesOfCurrentPermutation)){
                    return true;
                }

		//Loop through arc indexes from the back
		for(int arcAndTokenIndex = indicesOfCurrentPermutation.size()-1; arcAndTokenIndex >= 0; arcAndTokenIndex--){
			TokenList& enabledTokens = indicesOfCurrentPermutation.at(arcAndTokenIndex).enabledBy;
			vector<unsigned int >& modificationVector = indicesOfCurrentPermutation.at(arcAndTokenIndex).modificationVector;
			if(incrementModificationVector(modificationVector, enabledTokens)){
				changedSomething = true;
				break;
			}
		}
	}
        return false;
}       

bool TimeDartSuccessorGenerator::incrementModificationVector(vector<unsigned int >& modificationVector, TokenList& enabledTokens) const{
	unsigned int numOfTokenIndices = enabledTokens.size();

	unsigned int* refrences = new unsigned int[numOfTokenIndices];
	unsigned int* org_refrences = new unsigned int[numOfTokenIndices];

	for(unsigned int i = 0; i < enabledTokens.size(); i++){
		refrences[i] = enabledTokens[i].getCount();
	}

	for(unsigned int i = 0; i < modificationVector.size(); i++){
		refrences[modificationVector[i]]--;
	}

	memcpy(org_refrences, refrences, sizeof(unsigned int)*numOfTokenIndices);

	int modificationVectorSize = modificationVector.size();

	vector<unsigned int> tmp = modificationVector;
	// Loop through modification vector from the back
	for(int i = modificationVectorSize-1; i >= 0; i--){

		//Possible to increment index
		if(modificationVector[i] < numOfTokenIndices-1 && org_refrences[modificationVector.at(i)+1] > 0){
			//Increment index
			refrences[modificationVector.at(i)]++;
			modificationVector.at(i)++;
			refrences[modificationVector.at(i)]--;

			// Fix following indexes
			if(i < modificationVectorSize-1){
				unsigned int toSet = modificationVector.at(i);

				for(i++; i < modificationVectorSize; i++){
					//Find next index to set (die if not possible)
					while(refrences[toSet] == 0){
						toSet++;
						if(toSet >= numOfTokenIndices){
							modificationVector = tmp;
							delete [] refrences;
							delete [] org_refrences;
							return false;
						}
					}

					//Set index
					refrences[toSet]--;
					modificationVector[i] = toSet;
				}
			}
			delete [] refrences;
			delete [] org_refrences;
			return true;
		}else{
			// Free index
			refrences[modificationVector[i]]++;
		}
	}
	modificationVector = tmp;
	delete [] refrences;
	delete [] org_refrences;
	return false;
}

bool TimeDartSuccessorGenerator::insertMarking(NonStrictMarkingBase& init_marking, const TimedTransition& transition, ArcAndTokensVector& indicesOfCurrentPermutation) const{
	NonStrictMarkingBase* m = new NonStrictMarkingBase(init_marking);
	for(ArcAndTokensVector::iterator iter = indicesOfCurrentPermutation.begin(); iter != indicesOfCurrentPermutation.end(); iter++){
		vector<unsigned int>& tokens = iter->modificationVector;

		for(vector< unsigned int >::const_iterator tokenIter = tokens.begin(); tokenIter < tokens.end(); tokenIter++){
			Token t((iter->enabledBy)[*tokenIter].getAge(), 1);
			iter->moveToken(t, *m);
		}
	}

	for(OutputArc::WeakPtrVector::const_iterator postsetIter = transition.GetPostset().begin(); postsetIter != transition.GetPostset().end(); postsetIter++){
		Token t(0, postsetIter->lock()->getWeight());
		m->addTokenInPlace(postsetIter->lock()->getOutputPlace(), t);
	}
        return this->verifier.addToPW(m);
	
}

void TimeDartSuccessorGenerator::printTransitionStatistics(std::ostream& out) const {
		out << std::endl << "TRANSITION STATISTICS";
		for (unsigned int i=0;i<numberoftransitions;i++) {
			if ((i) % 6 == 0) {
				out << std::endl;
				out << "<" << tapn.getTransitions()[i]->GetName() << ":" << transitionStatistics[i] << ">";
			}
			else {
				out << " <"  <<tapn.getTransitions()[i]->GetName() << ":" << transitionStatistics[i] << ">";
			}
		}
		out << std::endl;
		out << std::endl;
	}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
