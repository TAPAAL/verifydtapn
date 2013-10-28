/*
 * NonStrictSearch.hpp
 *
 *  Created on: 26/04/2012
 *      Author: MathiasGS
 */

#ifndef WORKFLOW_HPP_
#define WORKFLOW_HPP_

#include "../DataStructures/WorkflowPWList.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../../Core/TAPN/TimedPlace.hpp"
#include "../../Core/TAPN/TimedTransition.hpp"
#include "../../Core/TAPN/TimedInputArc.hpp"
#include "../../Core/TAPN/TransportArc.hpp"
#include "../../Core/TAPN/InhibitorArc.hpp"
#include "../../Core/TAPN/OutputArc.hpp"
#include "../SuccessorGenerator.hpp"
#include "../QueryVisitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include <stack>
#include "ReachabilitySearch.hpp"
#include "../DataStructures/WaitingList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

class Workflow : public Verification<NonStrictMarking> {
public:
	Workflow(TAPN::TimedArcPetriNet& tapn, NonStrictMarking& initialMarking, AST::Query* query, VerificationOptions options, WaitingList<NonStrictMarking>* waiting_list) :
		tapn(tapn), initialMarking(initialMarking), query(query), options(options), in(NULL), out(NULL), modelType(calculateModelType()), pwList(new WorkflowPWList(waiting_list)), successorGenerator( tapn, *this ){
		for(TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++){
			if((*iter)->getType() == Dead){
				(*iter)->setType(Std);
			}
		}
	}

	enum ModelType{
		MTAWFN, ETAWFN, NOTTAWFN
	};

	inline const ModelType getModelType() const{ return modelType; }
        void printTransitionStatistics() const { successorGenerator.printTransitionStatistics(std::cout); }
        bool isDelayPossible(NonStrictMarking& marking);
	ModelType calculateModelType(){
		bool isin, isout;
		bool hasInvariant = false;
		for(TimedPlace::Vector::const_iterator iter = tapn.getPlaces().begin(); iter != tapn.getPlaces().end(); iter++){
			isin = isout = true;
			TimedPlace* p = (*iter);
			if(p->getInputArcs().empty() && p->getOutputArcs().empty() && p->getTransportArcs().empty()){
				bool continueOuter = true;
				// Test if really orphan place or if out place
				for(TransportArc::Vector::const_iterator trans_i = tapn.getTransportArcs().begin(); trans_i != tapn.getTransportArcs().end(); ++trans_i){
					if(&((*trans_i)->getDestination()) == p){
						continueOuter = false;
						break;
					}
				}
				if(continueOuter)	continue;	// Fix orphan places
			}

			if(!hasInvariant && p->getInvariant() != p->getInvariant().LS_INF){
				hasInvariant = true;
			}

			if(p->getInputArcs().size() > 0){
				isout = false;
			}

			if(p->getOutputArcs().size() > 0){
				isin = false;
			}

			if(isout){
				for(TransportArc::Vector::const_iterator iter = p->getTransportArcs().begin(); iter != p->getTransportArcs().end(); iter++){
					if(&(*iter)->getSource() == p){
						isout = false;
						break;
					}
				}
			}

			if(isin){
				for(TransportArc::Vector::const_iterator iter = tapn.getTransportArcs().begin(); iter != tapn.getTransportArcs().end(); ++iter){		// TODO maybe transportArcs should contain both incoming and outgoing? Might break something though.
					if(&(*iter)->getDestination() == p){
						isin = false;
						break;
					}
				}
			}

			if(isin){
				if(in == NULL){
					in = p;
				}else{
					return NOTTAWFN;
				}
			}

			if(isout){
				if(out == NULL){
					out = p;
				}else{
					return NOTTAWFN;
				}
			}

		}

		if(in == NULL || out == NULL || in == out){
			return NOTTAWFN;
		}

		if(initialMarking.size() != 1 || initialMarking.numberOfTokensInPlace(in->getIndex()) != 1){
			return NOTTAWFN;
		}

		bool hasUrgent = false;
		bool hasInhibitor = false;
		// All transitions must have preset
		for(TimedTransition::Vector::const_iterator iter = tapn.getTransitions().begin(); iter != tapn.getTransitions().end(); iter++){
			if((*iter)->getPresetSize() == 0 && (*iter)->getNumberOfTransportArcs() == 0){
				return NOTTAWFN;
			}

			if(!hasUrgent && (*iter)->isUrgent()){
				hasUrgent = true;
			}

			if(!hasInhibitor && !(*iter)->getInhibitorArcs().empty()){
				hasInhibitor = true;
			}
		}

		if(hasUrgent || hasInvariant || hasInhibitor){
			return ETAWFN;
		}

		return MTAWFN;
	}

	virtual void printExecutionTime(ostream& stream){}
	virtual void printMessages(ostream& stream){}

protected:
    	TAPN::TimedArcPetriNet& tapn;
	NonStrictMarking& initialMarking;
	AST::Query* query;
	VerificationOptions options;
	TimedPlace* in;
	TimedPlace* out;
	ModelType modelType;
        WorkflowPWList* pwList;
        SuccessorGenerator<NonStrictMarking> successorGenerator;
	NonStrictMarking* lastMarking;
        NonStrictMarking* tmpParent; 
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* NONSTRICTSEARCH_HPP_ */
