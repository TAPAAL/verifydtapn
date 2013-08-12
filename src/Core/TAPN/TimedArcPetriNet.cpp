#include "TimedArcPetriNet.hpp"

#include "TimeInterval.hpp"
#include <limits>

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::initialize(bool useGlobalMaxConstant)
		{
			for(unsigned int i = 0; i < places.size(); i++){
				places[i]->setIndex(i);
				updateMaxConstant(places[i]->getInvariant());
			}

			for(unsigned int i = 0; i < transitions.size(); i++){
				transitions[i]->setIndex(i);
			}

			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); ++iter)
			{
				const boost::shared_ptr<TimedInputArc>& arc = *iter;
				arc->getOutputTransition().addToPreset(arc);
				arc->getInputPlace().addInputArc(arc);
				updateMaxConstant(arc->getInterval());
			}

			for(TransportArc::Vector::const_iterator iter = transportArcs.begin(); iter != transportArcs.end(); ++iter)
			{
				const boost::shared_ptr<TransportArc>& arc = *iter;
				arc->getTransition().addTransportArcGoingThrough(arc);
				arc->getSource().addTransportArc(arc);
				updateMaxConstant(arc->getInterval());
			}

			for(InhibitorArc::Vector::const_iterator iter = inhibitorArcs.begin(); iter != inhibitorArcs.end(); ++iter) {
				const boost::shared_ptr<InhibitorArc>& arc = *iter;
				arc->getOutputTransition().addIncomingInhibitorArc(arc);
				arc->getInputPlace().addInhibitorArc(arc);
				arc->getInputPlace().setHasInhibitorArcs(true);
			}

			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); ++iter)
			{
				const boost::shared_ptr<OutputArc>& arc = *iter;
				arc->getInputTransition().addToPostset(arc);
				arc->getOutputPlace().addOutputArc(arc);
			}


			findMaxConstants();

			if(useGlobalMaxConstant){
				for(TimedPlace::Vector::const_iterator place_iter = places.begin(); place_iter != places.end(); ++place_iter)
				{
					(*place_iter)->setMaxConstant(getMaxConstant()==0? -1:getMaxConstant());
				}
			}

			markUntimedPlaces();
		}

		bool TimedArcPetriNet::removeOrphantedTransitions(){
			// CAUTION: This if statement removes orphan transitions.
			//			This changes answers for e.g. DEADLOCK queries if
			//			support for such queries are added later.
                        bool anyRemoved = false;
			if(transitions.size() > 0)
			{
				bool hasShownMessage = false;
				TimedTransition::Vector::iterator iter = transitions.begin();
				while(iter != transitions.end())
				{
					if((*iter)->getPresetSize() == 0 && (*iter)->getPostsetSize() == 0){
						iter = transitions.erase(iter);
						if(!hasShownMessage){
							std::cout << "Orphaned transitions have been removed." << std::endl << std::endl;
							hasShownMessage = true;
						}
                                                anyRemoved = true;
					}else{
						iter++;
					}
				}
			}
                        return anyRemoved;
		}

		void TimedArcPetriNet::markUntimedPlaces()
		{
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter)
			{
				bool isUntimedPlace = (*iter)->getInvariant() == TimeInvariant::LS_INF;

				for(TransportArc::Vector::const_iterator arcIter = transportArcs.begin(); arcIter != transportArcs.end(); ++arcIter)
				{
					isUntimedPlace = isUntimedPlace && (*arcIter)->getSource() != **iter;
				}

				if(isUntimedPlace)
				{
					for(TimedInputArc::Vector::const_iterator arcIter = inputArcs.begin(); arcIter != inputArcs.end(); ++arcIter)
					{
						if((*arcIter)->getInputPlace() == **iter)
						{
							isUntimedPlace = isUntimedPlace && (*arcIter)->getInterval().isZeroInfinity();
						}
					}
				}

				if(isUntimedPlace) (*iter)->markPlaceAsUntimed();
			}
		}

		void TimedArcPetriNet::findMaxConstants()
		{
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter)
			{
				int maxConstant = -1;
				if((*iter)->getInvariant() != TimeInvariant::LS_INF){
					maxConstant = (*iter)->getInvariant().getBound();
					(*iter)->setMaxConstant(maxConstant);
					(*iter)->setType(Inv);
				}
				else {
					(*iter)->setType(Dead);
					for(TimedInputArc::Vector::const_iterator arcIter = inputArcs.begin(); arcIter != inputArcs.end(); ++arcIter)
					{
						if((*arcIter)->getInputPlace() == **iter)
						{
							boost::shared_ptr<TimedInputArc> ia = *arcIter;
							const TAPN::TimeInterval& interval = ia->getInterval();

							const int lowerBound = interval.getLowerBound();
							const int upperBound = interval.getUpperBound();

							if(upperBound != std::numeric_limits<int>().max() || lowerBound != 0){
								if(upperBound == std::numeric_limits<int>().max()){
									maxConstant = (maxConstant < lowerBound ? lowerBound : maxConstant);
									(*iter)->setType(Std);
								} else {
									maxConstant = (maxConstant < upperBound ? upperBound : maxConstant);
								}
							} else {
								(*iter)->setType(Std);
							}
						}
					}
					for(TransportArc::Vector::const_iterator transport_iter = transportArcs.begin(); transport_iter != transportArcs.end(); transport_iter++)
					{
						if((*transport_iter)->getSource() == **iter)
						{
							int maxArc = -1;
							boost::shared_ptr<TransportArc> ta = *transport_iter;
							const TAPN::TimeInterval& interval = ta->getInterval();
							const int lowerBound = interval.getLowerBound();
							const int upperBound = interval.getUpperBound();

							if(upperBound != std::numeric_limits<int>().max() || lowerBound != 0){
								if(upperBound == std::numeric_limits<int>().max()){
									maxArc = lowerBound;
									(*iter)->setType(Std);
								} else {
									maxArc = upperBound;
								}
							} else {
								(*iter)->setType(Std);
							}
							int destinationInvariant = ta->getDestination().getInvariant().getBound();
							if(destinationInvariant != std::numeric_limits<int>().max()){
								maxArc = maxArc < destinationInvariant ? maxArc : destinationInvariant;
							}
							maxConstant = maxConstant < maxArc ? maxArc : maxConstant;
						}
					}
					(*iter)->setMaxConstant(maxConstant);

					for(InhibitorArc::Vector::const_iterator inhib_iter = inhibitorArcs.begin(); inhib_iter != inhibitorArcs.end(); inhib_iter++){
						if((*inhib_iter)->getInputPlace().getIndex() == (*iter)->getIndex() && (*iter)->getType() == Dead){
							(*iter)->setType(Std);
						}
					}
				}
			}

			for(TimedPlace::Vector::const_iterator place_iter = places.begin(); place_iter != places.end(); ++place_iter)
			{
				std::vector< TimedPlace* > causalitySet;
				calculateCausality(**place_iter, &causalitySet);
				for(std::vector< TimedPlace* >::const_iterator cau_iter = causalitySet.begin(); cau_iter != causalitySet.end(); cau_iter++){
					if((*cau_iter)->getMaxConstant() > (*place_iter)->getMaxConstant()){
						(*place_iter)->setMaxConstant((*cau_iter)->getMaxConstant());
					}
				}
			}

			for(TimedTransition::Vector::iterator iter = transitions.begin(); iter != transitions.end(); iter++){
				for(OutputArc::WeakPtrVector::const_iterator place_iter = iter->get()->getPostset().begin(); place_iter != iter->get()->getPostset().end(); place_iter++){
					if(place_iter->lock()->getOutputPlace().getMaxConstant() > -1){
						iter->get()->setUntimedPostset(false);
						break;
					}
				}
			}
		}

		void TimedArcPetriNet::calculateCausality(TimedPlace& p, std::vector< TimedPlace* >* result) const
		{
			for(std::vector< TimedPlace* >::const_iterator iter = result->begin(); iter != result->end(); iter++){
				if(**iter == p) return;
			}
			result->push_back(&p);
			for(TransportArc::Vector::const_iterator iter = this->getTransportArcs().begin(); iter != this->getTransportArcs().end(); iter++){
				if((*iter)->getSource() == p){
					if((*iter)->getInterval().getUpperBound() == std::numeric_limits<int>().max()){
						calculateCausality((*iter)->getDestination(), result);
					}
				}
			}
		}

		void TimedArcPetriNet::updatePlaceTypes(const AST::Query* query, VerificationOptions options){
			std::vector< int > placeNumbers;
			boost::any context = placeNumbers;
			DiscreteVerification::PlaceVisitor visitor;
			query->accept(visitor, context);
			placeNumbers = boost::any_cast< std::vector< int > >(context);

			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter){
				if(options.getKeepDeadTokens() && (*iter)->getType() == Dead){
					(*iter)->setType(Std);
					continue;
				}
				for(std::vector<int>::const_iterator id_iter = placeNumbers.begin(); id_iter != placeNumbers.end(); id_iter++){
					if((*id_iter) == (*iter)->getIndex() && (*iter)->getType() == Dead){
						(*iter)->setType(Std);
						break;
					}
				}
			}
		}

		void TimedArcPetriNet::updateMaxConstant(const TimeInterval& interval)
		{
			int lowerBound = interval.getLowerBound();
			int upperBound = interval.getUpperBound();
			if(lowerBound < std::numeric_limits<int>().max() && lowerBound > maxConstant)
			{
				maxConstant = lowerBound;
			}
			if(upperBound < std::numeric_limits<int>().max() && upperBound > maxConstant)
			{
				maxConstant = upperBound;
			}
		}

		void TimedArcPetriNet::updateMaxConstant(const TimeInvariant& invariant)
		{
			int bound = invariant.getBound();
			if(bound < std::numeric_limits<int>().max() && bound > maxConstant)
			{
				maxConstant = bound;
			}
		}

		int TimedArcPetriNet::getPlaceIndex(const std::string& placeName) const
		{
				int idx = TimedPlace::BottomIndex();
				for(unsigned int i = 0; i < places.size(); ++i)
				{
					if(places[i]->getName() == placeName)
					{
						idx = i;
						break;
					}
				}

				return idx;
		}

		void TimedArcPetriNet::print(std::ostream & out) const
		{
			out << "TAPN:" << std::endl << "  Places: ";
			for(TimedPlace::Vector::const_iterator iter = places.begin();iter != places.end();iter++)
			{
				out << *(*iter);
				out << std::endl;
			}

			out << std::endl << "  Transitions: ";
			for(TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++)
			{
				out << *(*iter);
				out << std::endl;
			}

			out << std::endl << "  Input Arcs: ";
			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); iter++)
			{
				out << *(*iter);
				out << ", ";
			}

			if(transportArcs.size() > 0) {
				out << std::endl << "  Transport Arcs: ";
				for(TransportArc::Vector::const_iterator iter = transportArcs.begin(); iter != transportArcs.end(); iter++)
				{
					out << *(*iter);
					out << ", ";
				}
			}

			if(inhibitorArcs.size() > 0) {
				out << std::endl << "  Inhibitor Arcs: ";
				for(InhibitorArc::Vector::const_iterator iter = inhibitorArcs.begin(); iter != inhibitorArcs.end(); iter++)
				{
					out << *(*iter);
					out << ", ";
				}
			}


			out << std::endl << "  Output Arcs: ";
			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); iter++)
			{
				out << *(*iter);
				out << ", ";
			}

			out << std::endl;
		}

		bool TimedArcPetriNet::isNonStrict() const{

			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); iter++){
				TimedInputArc& ia = *(*iter);
				if(ia.getInterval().isLowerBoundStrict() || (ia.getInterval().isUpperBoundStrict() && ia.getInterval().getUpperBound() != std::numeric_limits<int>().max())){
					return false;
				}
			}


			for(TransportArc::Vector::const_iterator iter = transportArcs.begin(); iter != transportArcs.end(); iter++){
				TransportArc& ta = *(*iter);
				if(ta.getInterval().isLowerBoundStrict() || (ta.getInterval().isUpperBoundStrict() && ta.getInterval().getUpperBound() != std::numeric_limits<int>().max())){
					return false;
				}
			}

			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); iter++){
				const TimedPlace& p = *(*iter);
				if(p.getInvariant().isBoundStrict() && p.getInvariant().getBound() != std::numeric_limits<int>().max()){
					return false;
				}
			}

			return true;
		}
	}


}


