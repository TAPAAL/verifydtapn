#include "SuccessorGenerator.hpp"
#include "../Core/TAPN/TimedInputArc.hpp"
#include "../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "../Core/TAPN/Pairing.hpp"
#include <algorithm>

namespace VerifyTAPN {
	void SuccessorGenerator::GenerateDiscreteTransitionsSuccessors(const SymbolicMarking& marking, std::vector<Successor>& succ)
	{
		ClearAll();

		const TAPN::TimedTransition::Vector& transitions = tapn.GetTransitions();

		CollectArcsAndAppropriateTokens(transitions, &marking);
		GenerateSuccessors(transitions, &marking, succ);
	}

    void SuccessorGenerator::UpdateArcInfo(const SymbolicMarking *marking, int currInputPlaceIndex, const TAPN::TimeInterval & ti, unsigned int & currInputArcIdx)
    {
        unsigned int nTokensFromCurrInputPlace = 0;
        for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
		{
			int placeIndex = marking->GetTokenPlacement(i);


			if(placeIndex == currInputPlaceIndex)
			{
				bool potentiallyUsable = marking->PotentiallySatisfies(i, ti);

				if(potentiallyUsable)
				{
					assert(currInputArcIdx <= nInputArcs);
					assert(nTokensFromCurrInputPlace <= static_cast<unsigned int>(options.GetKBound()));

					arcsArray[currInputArcIdx] = arcsArray[currInputArcIdx] + 1;
					tokenIndices->insert_element(currInputArcIdx,nTokensFromCurrInputPlace, i);
					nTokensFromCurrInputPlace++;
				}
			}
		}
    }

	// Collects the number of tokens of potentially appropriate age for each input into arcsArray.
	// Note that arcs array should be sorted by transition. Further, if a token is potentially of
	// appropriate age we add the token index to the tokenIndices matrix for use when generating successors.
	void SuccessorGenerator::CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector& transitions, const SymbolicMarking* marking)
	{
		unsigned int currInputArcIdx = 0;

		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			const TAPN::TransportArc::WeakPtrVector& transportArcs = (*iter)->GetTransportArcs();
			for(TAPN::TransportArc::WeakPtrVector::const_iterator presetIter = transportArcs.begin(); presetIter != transportArcs.end(); ++presetIter)
			{
				boost::shared_ptr<TAPN::TransportArc> ta = (*presetIter).lock();
				const TAPN::TimeInterval& ti = ta->Interval();
				int currInputPlaceIndex = tapn.GetPlaceIndex(ta->Source());

				UpdateArcInfo(marking, currInputPlaceIndex, ti, currInputArcIdx);
				currInputArcIdx++;
			}

			const TAPN::TimedInputArc::WeakPtrVector& preset = (*iter)->GetPreset();
			for(TAPN::TimedInputArc::WeakPtrVector::const_iterator presetIter = preset.begin(); presetIter != preset.end(); ++presetIter)
			{
				boost::shared_ptr<TAPN::TimedInputArc> ia = (*presetIter).lock();
				const TAPN::TimeInterval& ti = ia->Interval();
				int currInputPlaceIndex = tapn.GetPlaceIndex(ia->InputPlace());

				UpdateArcInfo(marking, currInputPlaceIndex, ti, currInputArcIdx);
				currInputArcIdx++;
			}
		}
	}

	// Generate for each enabled transition a successor for each
	// permutation of tokens of appropriate age from the token matrix.
	void SuccessorGenerator::GenerateSuccessors(const TAPN::TimedTransition::Vector& transitions, const SymbolicMarking* marking, std::vector<Successor>& succ)
	{
		int currentTransitionIndex = 0;
		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			unsigned int presetSize = (*iter)->GetPresetSize();
			if(IsTransitionEnabled(*(*iter),marking,currentTransitionIndex, presetSize))
			{
				// The indicesOfCurrentPermutation array stores the column indices
				// into the token matrix for the current permutation of input tokens.
				// I.e. it is used to select which tokens to consume when firing the transition.
				unsigned int indicesOfCurrentPermutation[presetSize];
				for(unsigned int i = 0; i < presetSize; ++i)
					indicesOfCurrentPermutation[i] = 0;

				bool done = false;
				while(true)
				{
					GenerateSuccessorForCurrentPermutation(*(*iter), indicesOfCurrentPermutation, currentTransitionIndex, presetSize, marking, succ);

					// Generate next permutation of input tokens
					int j = presetSize - 1;
					while(true)
					{
						indicesOfCurrentPermutation[j] += 1;
						if(indicesOfCurrentPermutation[j] < arcsArray[currentTransitionIndex+j])
							break;
						indicesOfCurrentPermutation[j] = 0;
						j -= 1;
						if(j < 0)
						{
							done = true;
							break;
						}
					}
					if(done)
						break;
				}
			}

			currentTransitionIndex += presetSize; // jump to next start of next transition in arcsArray
		}
	}

	// After the input arcs and tokens have been collected
	// you can check the enabledness of a transition by specifying
	// the index of the first input arc of the transition
	// along with the size of the preset of the transition
	bool SuccessorGenerator::IsTransitionEnabled(const TAPN::TimedTransition& transition, const SymbolicMarking* marking, unsigned int currTransitionIndex, unsigned int presetSize) const
	{
		const TAPN::InhibitorArc::WeakPtrVector& inhibitorArcs = transition.GetInhibitorArcs();
		for(TAPN::InhibitorArc::WeakPtrVector::const_iterator iter = inhibitorArcs.begin(); iter != inhibitorArcs.end(); ++iter)
		{
			boost::shared_ptr<TAPN::InhibitorArc> ia = iter->lock();
			int sourcePlaceIndex = ia->InputPlace().GetIndex();

			for(unsigned int i = 0; i < marking->NumberOfTokens(); i++)
			{
				if(marking->GetTokenPlacement(i) == sourcePlaceIndex)
					return false;
			}
		}

		for(unsigned int i = currTransitionIndex; i < currTransitionIndex + presetSize; ++i)
		{
			if(arcsArray[i] <= 0)
				return false;
		}
		return true;
	}

	// Generates a successor node for the current permutation of input tokens
	void SuccessorGenerator::GenerateSuccessorForCurrentPermutation(const TAPN::TimedTransition& transition, const unsigned int* currentPermutationindices, const unsigned int currentTransitionIndex, const unsigned int presetSize, const SymbolicMarking* marking, std::vector<Successor>& succ)
	{
		unsigned int kBound = options.GetKBound();
		bool trace = options.GetTrace() != NONE;

		const Pairing& pairing = tapn.GetPairing(transition);
		const TAPN::TimedInputArc::WeakPtrVector& preset = transition.GetPreset();
		std::vector<int> tokensToRemove;
		SymbolicMarking* next = factory.Clone(*marking);

		for(unsigned int i = 0; i < transition.NumberOfTransportArcs(); ++i)
		{
			boost::shared_ptr<TAPN::TransportArc> ta = transition.GetTransportArcs()[i].lock();
			const TAPN::TimeInterval& ti = ta->Interval();

			int tokenIndex = tokenIndices->at_element(currentTransitionIndex+i, currentPermutationindices[i]);
			int outputPlaceIndex = tapn.GetPlaceIndex(ta->Destination());

			// constrain dbm with the guard of the input arc
			next->Constrain(tokenIndex, ti);
			next->MoveToken(tokenIndex, outputPlaceIndex);

			if(next->IsEmpty())
			{
				delete next;
				return;
			}

		}

		int offset = transition.NumberOfTransportArcs();
		// move all tokens that are currently in the net
		for(unsigned int i = 0; i < transition.NumberOfInputArcs(); ++i)
		{
			boost::shared_ptr<TAPN::TimedInputArc> inputArc = preset[i].lock();
			int inputPlace = tapn.GetPlaceIndex(inputArc->InputPlace());
			const TAPN::TimeInterval& ti = inputArc->Interval();
			const std::list<int>& outputPlaces = pairing.GetOutputPlacesFor(inputPlace);

			// only BOTTOM is allowed to have more than 1 associated output place
			assert(outputPlaces.size() <= 1);

			for(std::list<int>::const_iterator opIter = outputPlaces.begin(); opIter != outputPlaces.end(); ++opIter)
			{
				// change placement
				int tokenIndex = tokenIndices->at_element(currentTransitionIndex+offset+i, currentPermutationindices[offset+i]);
				int outputPlaceIndex = *opIter;

				// constrain dbm with the guard of the input arc
				next->Constrain(tokenIndex, ti);

				if(outputPlaceIndex == TAPN::TimedPlace::BottomIndex())
					tokensToRemove.push_back(tokenIndex);
				else
					next->MoveToken(tokenIndex, outputPlaceIndex);

				if(next->IsEmpty())
				{
					delete next;
					return;
				}
			}
		}

		// reset clocks of moved tokens
		for (unsigned int i = transition.NumberOfTransportArcs(); i < presetSize; ++i) {
			int tokenIndex = tokenIndices->at_element(currentTransitionIndex+i, currentPermutationindices[i]);

			next->Reset(tokenIndex);
		}

		// check if we need to add or remove tokens in the successor marking
		int diff = presetSize - transition.GetPostsetSize();
		if(diff > 0) // preset bigger than postset, i.e. more tokens consumed than produced
		{
			assert(tokensToRemove.size() == static_cast<unsigned int>(std::abs(diff)));

			// remove tokens in placement and dbm
			next->RemoveTokens(tokensToRemove);
		}
		else if(diff < 0) // postset bigger than preset, i.e. more tokens produced than consumed
		{
			const std::list<int>& outputPlaces = pairing.GetOutputPlacesFor(TAPN::TimedPlace::BottomIndex());

			// Perform under-approximation in case the net is not k-bounded.
			// I.e. only allow up to k tokens in a given marking.
			if(next->NumberOfTokens() + outputPlaces.size() > kBound) {
				delete next;
				return;
			}

			next->AddTokens(outputPlaces);
		}

		for(unsigned int i = 0; i < next->NumberOfTokens(); i++)
		{
			const TAPN::TimeInvariant& invariant = tapn.GetPlace(next->GetTokenPlacement(i)).GetInvariant();
			next->Constrain(i, invariant);

			if(next->IsEmpty())
			{
				delete next;
				return;
			}
		}


		//next->DBMIntern(); // TODO: Handle interning internally in the marking

		// Store trace information
		if(trace){
			TraceInfo traceInfo(marking->UniqueId(), transition.GetIndex(), next->UniqueId());
			IndirectionTable mapping;
			MakeIdentity(mapping, options.GetKBound());

			for(unsigned int i = 0; i < presetSize; ++i)
			{
				int tokenIndex = tokenIndices->at_element(currentTransitionIndex+i, currentPermutationindices[i]);
				boost::shared_ptr<TAPN::TimedInputArc> ia = preset[i].lock();
				const TAPN::TimeInterval& ti = ia->Interval();
				int indexAfterFiring = tokenIndex;
				for(std::vector<int>::iterator iter = tokensToRemove.begin(); iter != tokensToRemove.end(); ++iter)
				{
					if(*iter < tokenIndex) indexAfterFiring--;
					else if(*iter == tokenIndex){
						indexAfterFiring = -1;
						UpdateTraceMapping(mapping, tokenIndex);
						break;
					}
				}

				assert(tokenIndex < static_cast<int>(marking->NumberOfTokens()));
				assert(indexAfterFiring < static_cast<int>(next->NumberOfTokens()));
				int placement =  indexAfterFiring == -1 ? -1 : next->GetTokenPlacement(indexAfterFiring);
				Participant participant(tokenIndex, ti,placement);
				traceInfo.AddParticipant(participant);
			}

			if(diff < 0){
				TAPN::TimeInterval inf;
				for(int i = diff; i < 0; i++){
					int indexAfterFiring = (marking->NumberOfTokens() - diff + i);
					Participant participant(indexAfterFiring,inf, next->GetTokenPlacement(indexAfterFiring));
					traceInfo.AddParticipant(participant);
				}
			}

			traceInfo.SetTransitionFiringMapping(mapping);
			traceInfo.setMarking(next);
			succ.push_back(Successor(next, traceInfo));
		}else{
			succ.push_back(Successor(next));
		}
		tokensToRemove.clear();
	}

	void SuccessorGenerator::MakeIdentity(IndirectionTable& mapping, unsigned int size) const
	{
		for(unsigned int i = 0; i < size; ++i)
		{
			mapping.AddMapping(i,i);
		}
	}

    void SuccessorGenerator::UpdateTraceMapping(IndirectionTable& mapping, unsigned int tokenToRemove) const
    {
    	IndirectionTable table;
    	for(unsigned int i = 0; i < tokenToRemove; i++)
    	{
    		table.AddMapping(i, mapping.MapForward(i));
    	}
    	table.AddMapping(tokenToRemove, mapping.Size() - 1);
    	for(unsigned int i = tokenToRemove+1; i < mapping.Size(); ++i)
    	{
    		table.AddMapping(i, mapping.MapForward(i)-1);
    	}
    	mapping.Swap(table);
    }

//    void SuccessorGenerator::InvertMapping(std::vector<int>& mapping) const
//    {
//    	std::vector<int> inverted(mapping.size(),-1);
//    	unsigned int count = 0;
//    	for(unsigned int i = 0; i < mapping.size(); ++i)
//    	{
//    		int index = mapping[i];
//    		if(index >= 0)
//    		{
//    			inverted[index] = i;
//    			count++;
//    		}
//    	}
//    	inverted.resize(count);
//    	mapping.swap(inverted);
//    }


	void SuccessorGenerator::Print(std::ostream& out) const
	{
		out << "\nArcs Array:\n";
		out << "------------------\n";

		for(unsigned int i = 0; i < nInputArcs; i++)
		{
			out << i << ": " << arcsArray[i] << "\n";
		}

		out << "\n\nToken Indices:\n";
		out << "----------------------\n";

		out << *tokenIndices << "\n";
	}
}
