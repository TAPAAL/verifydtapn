/*
 * SuccessorGenerator.cpp
 *
 *  Created on: 22/03/2012
 *      Author: MathiasGS
 */

#include "SuccessorGenerator.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {

	SuccessorGenerator::~SuccessorGenerator(){

	}

	vector< NonStrictMarking > SuccessorGenerator::generateSuccessors(const NonStrictMarking& marking) const{
		std::cout << std::endl << "Successor generator:" << std::endl << "------------------------" << std::endl;
		vector< NonStrictMarking > result;

		for(PlaceList::const_iterator place_iter = marking.places.begin(); place_iter != marking.places.end(); place_iter++){

			/* Generate vectors with local transitions */
			vector< TimedInputArc* > inputArcs;
			vector< TransportArc* > transportArcs;
			vector< InhibitorArc* > inhibitorArcs;

			// Input arcs
			std::cout << "Place " << place_iter->id << std::endl << " inputArcs: " << std::endl;
			for(TimedInputArc::Vector::const_iterator it = tapn.GetInputArcs().begin(); it != tapn.GetInputArcs().end(); it++){
				if(it->get()->InputPlace().GetIndex() == place_iter->id){
					inputArcs.push_back( it->get() );
					it->get()->Print(std::cout);
					std::cout << std::endl;
				}
			}
			std::cout << "EO inputArcs - size: " << inputArcs.size() << std::endl;

			// Transport arcs
			std::cout << " transport arcs: " << std::endl;
			for(TransportArc::Vector::const_iterator it = tapn.GetTransportArcs().begin(); it != tapn.GetTransportArcs().end(); it++){
				if(it->get()->Source().GetIndex() == place_iter->id){
					transportArcs.push_back( it->get() );
					it->get()->Print(std::cout);
					std::cout << std::endl;
				}
			}
			std::cout << "EO transport arcs - size: " << transportArcs.size() << std::endl;

			// Transport arcs
			std::cout << " inhibitor arcs: " << std::endl;
			for(InhibitorArc::Vector::const_iterator it = tapn.GetInhibitorArcs().begin(); it != tapn.GetInhibitorArcs().end(); it++){
				if(it->get()->InputPlace().GetIndex() == place_iter->id){
					inhibitorArcs.push_back( it->get() );
					it->get()->Print(std::cout);
					std::cout << std::endl;
				}
			}
			std::cout << "EO inhibitor arcs - size: " << inhibitorArcs.size() << std::endl;

			/* EO Generate vectors with local transitions */

			/* Check enabled transitions */
			for(TimedTransition::Vector::const_iterator trans_iter = tapn.GetTransitions().begin(); trans_iter != tapn.GetTransitions().end(); trans_iter++){

			}
			/* EO Check enabled transitions */

			for(TokenList::const_iterator token_iter = place_iter->tokens.begin(); token_iter != place_iter->tokens.end(); token_iter++){
				/* Check if token activates arcs */


				/* EO Check if token activates arcs */
			}
		}
		std::cout << "------------------------" << std::endl << std::endl;
		return result;
	}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
