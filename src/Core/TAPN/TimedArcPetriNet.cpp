#include "TimedArcPetriNet.hpp"

#include "TimeInterval.hpp"
#include <limits>

namespace VerifyTAPN {
	namespace TAPN {
		void TimedArcPetriNet::Initialize(bool useGlobalMaxConstant)
		{
			for(unsigned int i = 0; i < places.size(); i++){
				places[i]->SetIndex(i);
				UpdateMaxConstant(places[i]->GetInvariant());
			}

			for(unsigned int i = 0; i < transitions.size(); i++){
				transitions[i]->SetIndex(i);
			}

			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); ++iter)
			{
				const boost::shared_ptr<TimedInputArc>& arc = *iter;
				arc->OutputTransition().AddToPreset(arc);
				arc->InputPlace().AddInputArc(arc);
				UpdateMaxConstant(arc->Interval());
			}

			for(TransportArc::Vector::const_iterator iter = transportArcs.begin(); iter != transportArcs.end(); ++iter)
			{
				const boost::shared_ptr<TransportArc>& arc = *iter;
				arc->Transition().AddTransportArcGoingThrough(arc);
				arc->Source().AddTransportArc(arc);
				UpdateMaxConstant(arc->Interval());
			}

			for(InhibitorArc::Vector::const_iterator iter = inhibitorArcs.begin(); iter != inhibitorArcs.end(); ++iter) {
				const boost::shared_ptr<InhibitorArc>& arc = *iter;
				arc->getOutputTransition().AddIncomingInhibitorArc(arc);
				arc->getInputPlace().AddInhibitorArc(arc);
				arc->getInputPlace().SetHasInhibitorArcs(true);
			}

			for(OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); ++iter)
			{
				const boost::shared_ptr<OutputArc>& arc = *iter;
				arc->getInputTransition().AddToPostset(arc);
				arc->getOutputPlace().AddOutputArc(arc);
			}

			GeneratePairings();
			FindMaxConstants();

			if(useGlobalMaxConstant){
				for(TimedPlace::Vector::const_iterator place_iter = places.begin(); place_iter != places.end(); ++place_iter)
				{
					(*place_iter)->SetMaxConstant(MaxConstant()==0? -1:MaxConstant());
				}
			}

			MarkUntimedPlaces();
		}

		void TimedArcPetriNet::removeOrphantedTransitions(){
			// CAUTION: This if statement removes orphan transitions.
			//			This changes answers for e.g. DEADLOCK queries if
			//			support for such queries are added later.
			if(transitions.size() > 0)
			{
				bool hasShownMessage = false;
				TimedTransition::Vector::iterator iter = transitions.begin();
				while(iter != transitions.end())
				{
					if((*iter)->GetPresetSize() == 0 && (*iter)->GetPostsetSize() == 0){
						iter = transitions.erase(iter);
						if(!hasShownMessage){
							std::cout << "Orphaned transitions have been removed." << std::endl << std::endl;
							hasShownMessage = true;
						}
					}else{
						iter++;
					}
				}
			}
		}

		void TimedArcPetriNet::MarkUntimedPlaces()
		{
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter)
			{
				bool isUntimedPlace = (*iter)->GetInvariant() == TimeInvariant::LS_INF;

				for(TransportArc::Vector::const_iterator arcIter = transportArcs.begin(); arcIter != transportArcs.end(); ++arcIter)
				{
					isUntimedPlace = isUntimedPlace && (*arcIter)->Source() != **iter;
				}

				if(isUntimedPlace)
				{
					for(TimedInputArc::Vector::const_iterator arcIter = inputArcs.begin(); arcIter != inputArcs.end(); ++arcIter)
					{
						if((*arcIter)->InputPlace() == **iter)
						{
							isUntimedPlace = isUntimedPlace && (*arcIter)->Interval().IsZeroInfinity();
						}
					}
				}

				if(isUntimedPlace) (*iter)->MarkPlaceAsUntimed();
			}
		}

		void TimedArcPetriNet::FindMaxConstants()
		{
			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter)
			{
				int maxConstant = -1;
				if((*iter)->GetInvariant() != TimeInvariant::LS_INF){
					maxConstant = (*iter)->GetInvariant().GetBound();
					(*iter)->SetMaxConstant(maxConstant);
					(*iter)->SetType(Inv);
				}
				else {
					(*iter)->SetType(Dead);
					for(TimedInputArc::Vector::const_iterator arcIter = inputArcs.begin(); arcIter != inputArcs.end(); ++arcIter)
					{
						if((*arcIter)->InputPlace() == **iter)
						{
							boost::shared_ptr<TimedInputArc> ia = *arcIter;
							const TAPN::TimeInterval& interval = ia->Interval();

							const int lowerBound = interval.GetLowerBound();
							const int upperBound = interval.GetUpperBound();

							if(upperBound != std::numeric_limits<int>().max() || lowerBound != 0){
								if(upperBound == std::numeric_limits<int>().max()){
									maxConstant = (maxConstant < lowerBound ? lowerBound : maxConstant);
									(*iter)->SetType(Std);
								} else {
									maxConstant = (maxConstant < upperBound ? upperBound : maxConstant);
								}
							} else {
								(*iter)->SetType(Std);
							}
						}
					}
					for(TransportArc::Vector::const_iterator transport_iter = transportArcs.begin(); transport_iter != transportArcs.end(); transport_iter++)
					{
						if((*transport_iter)->Source() == **iter)
						{
							int maxArc = -1;
							boost::shared_ptr<TransportArc> ta = *transport_iter;
							const TAPN::TimeInterval& interval = ta->Interval();
							const int lowerBound = interval.GetLowerBound();
							const int upperBound = interval.GetUpperBound();

							if(upperBound != std::numeric_limits<int>().max() || lowerBound != 0){
								if(upperBound == std::numeric_limits<int>().max()){
									maxArc = lowerBound;
									(*iter)->SetType(Std);
								} else {
									maxArc = upperBound;
								}
							} else {
								(*iter)->SetType(Std);
							}
							int destinationInvariant = ta->Destination().GetInvariant().GetBound();
							if(destinationInvariant != std::numeric_limits<int>().max()){
								maxArc = maxArc < destinationInvariant ? maxArc : destinationInvariant;
							}
							maxConstant = maxConstant < maxArc ? maxArc : maxConstant;
						}
					}
					(*iter)->SetMaxConstant(maxConstant);

					for(InhibitorArc::Vector::const_iterator inhib_iter = inhibitorArcs.begin(); inhib_iter != inhibitorArcs.end(); inhib_iter++){
						if((*inhib_iter)->getInputPlace().GetIndex() == (*iter)->GetIndex() && (*iter)->GetType() == Dead){
							(*iter)->SetType(Std);
						}
					}
				}
			}

			for(TimedPlace::Vector::const_iterator place_iter = places.begin(); place_iter != places.end(); ++place_iter)
			{
				std::vector< TimedPlace* > causalitySet;
				calculateCausality(**place_iter, &causalitySet);
				for(std::vector< TimedPlace* >::const_iterator cau_iter = causalitySet.begin(); cau_iter != causalitySet.end(); cau_iter++){
					if((*cau_iter)->GetMaxConstant() > (*place_iter)->GetMaxConstant()){
						(*place_iter)->SetMaxConstant((*cau_iter)->GetMaxConstant());
					}
				}
			}

			for(TimedTransition::Vector::iterator iter = transitions.begin(); iter != transitions.end(); iter++){
				for(OutputArc::WeakPtrVector::const_iterator place_iter = iter->get()->GetPostset().begin(); place_iter != iter->get()->GetPostset().end(); place_iter++){
					if(place_iter->lock()->getOutputPlace().GetMaxConstant() > -1){
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
			for(TransportArc::Vector::const_iterator iter = this->GetTransportArcs().begin(); iter != this->GetTransportArcs().end(); iter++){
				if((*iter)->Source() == p){
					if((*iter)->Interval().GetUpperBound() == std::numeric_limits<int>().max()){
						calculateCausality((*iter)->Destination(), result);
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
				if(options.getKeepDeadTokens() && (*iter)->GetType() == Dead){
					(*iter)->SetType(Std);
					continue;
				}
				for(std::vector<int>::const_iterator id_iter = placeNumbers.begin(); id_iter != placeNumbers.end(); id_iter++){
					if((*id_iter) == (*iter)->GetIndex() && (*iter)->GetType() == Dead){
						(*iter)->SetType(Std);
						break;
					}
				}
			}
		}

		void TimedArcPetriNet::UpdateMaxConstant(const TimeInterval& interval)
		{
			int lowerBound = interval.GetLowerBound();
			int upperBound = interval.GetUpperBound();
			if(lowerBound < std::numeric_limits<int>().max() && lowerBound > maxConstant)
			{
				maxConstant = lowerBound;
			}
			if(upperBound < std::numeric_limits<int>().max() && upperBound > maxConstant)
			{
				maxConstant = upperBound;
			}
		}

		void TimedArcPetriNet::UpdateMaxConstant(const TimeInvariant& invariant)
		{
			int bound = invariant.GetBound();
			if(bound < std::numeric_limits<int>().max() && bound > maxConstant)
			{
				maxConstant = bound;
			}
		}

		int TimedArcPetriNet::GetPlaceIndex(const std::string& placeName) const
		{
				int idx = TimedPlace::BottomIndex();
				for(unsigned int i = 0; i < places.size(); ++i)
				{
					if(places[i]->GetName() == placeName)
					{
						idx = i;
						break;
					}
				}

				return idx;
		}

		void TimedArcPetriNet::Print(std::ostream & out) const
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


		void TimedArcPetriNet::GeneratePairings()
		{
			for(TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
			{
				const TimedTransition& t = *(*iter);
				Pairing p(*this, t);
				pairings.insert(std::pair<TimedTransition, Pairing>(t, p));
			}
		}

		bool TimedArcPetriNet::IsNonStrict() const{

			for(TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); iter++){
				TimedInputArc& ia = *(*iter);
				if(ia.Interval().IsLowerBoundStrict() || (ia.Interval().IsUpperBoundStrict() && ia.Interval().GetUpperBound() != std::numeric_limits<int>().max())){
					return false;
				}
			}


			for(TransportArc::Vector::const_iterator iter = transportArcs.begin(); iter != transportArcs.end(); iter++){
				TransportArc& ta = *(*iter);
				if(ta.Interval().IsLowerBoundStrict() || (ta.Interval().IsUpperBoundStrict() && ta.Interval().GetUpperBound() != std::numeric_limits<int>().max())){
					return false;
				}
			}

			for(TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); iter++){
				const TimedPlace& p = *(*iter);
				if(p.GetInvariant().IsBoundStrict() && p.GetInvariant().GetBound() != std::numeric_limits<int>().max()){
					return false;
				}
			}

			return true;
		}
	}


}


