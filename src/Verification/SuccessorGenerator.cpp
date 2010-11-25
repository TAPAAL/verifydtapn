#include "SuccessorGenerator.hpp"
#include "../TAPN/TimedInputArc.hpp"
#include "../Core/SymMarking.hpp"
#include "../Core/Pairing.hpp"
#include "dbm/print.h"

namespace VerifyTAPN {
	void SuccessorGenerator::GenerateDiscreteTransitionsSuccessors(const SymMarking* marking, std::vector<Successor>& succ)
	{
		ClearAll();

		const TAPN::TimedTransition::Vector& transitions = tapn.GetTransitions();

		CollectArcsAndAppropriateTokens(transitions, marking);
		GenerateSuccessors(transitions, marking, succ);
	}

	// Collects the number of tokens of potentially appropriate age for each input into arcsArray.
	// Note that arcs array should be sorted by transition. Further, if a token is potentially of
	// appropriate age we add the token index to the tokenIndices matrix for use when generating successors.
	void SuccessorGenerator::CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector& transitions, const SymMarking* marking)
	{
		unsigned int kBound = options.GetKBound();
		unsigned int currInputArcIdx = 0;

		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			const TAPN::TimedInputArc::WeakPtrVector& preset = (*iter)->GetPreset();
			for(TAPN::TimedInputArc::WeakPtrVector::const_iterator presetIter = preset.begin(); presetIter != preset.end(); ++presetIter)
			{
				boost::shared_ptr<TAPN::TimedInputArc> ia = (*presetIter).lock();
				const TAPN::TimeInterval& ti = ia->Interval();
				unsigned int nTokensFromCurrInputPlace = 0;
				int currInputPlaceIndex = tapn.GetPlaceIndex(ia->InputPlace());

				for(unsigned int i = 0; i < marking->GetNumberOfTokens(); i++)
				{
					int placeIndex = marking->GetTokenPlacement(i);


					if(placeIndex == currInputPlaceIndex)
					{
//						bool inappropriateAge = true;
//						if(useInfinityPlaces && tapn.IsPlaceInfinityPlace(placeIndex))
//							inappropriateAge = false;
//						else
							bool inappropriateAge = marking->IsTokenOfInappropriateAge(i, ti);

						if(!inappropriateAge) // token potentially satisfies guard
						{
							assert(currInputArcIdx <= nInputArcs);
							assert(nTokensFromCurrInputPlace <= kBound);

							arcsArray[currInputArcIdx] = arcsArray[currInputArcIdx] + 1;
							tokenIndices->insert_element(currInputArcIdx,nTokensFromCurrInputPlace, i);
							nTokensFromCurrInputPlace++;
						}
					}
				}

				currInputArcIdx++;
			}
		}
	}

	void SuccessorGenerator::GenerateSuccessors(const TAPN::TimedTransition::Vector& transitions, const SymMarking* marking, std::vector<Successor>& succ)
	{
		int currTransitionIdx = 0;
		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			unsigned int presetSize = (*iter)->GetPresetSize();

			if(IsTransitionEnabled(currTransitionIdx, presetSize))
			{
				// generate permutations of successors
				unsigned int indices[presetSize];
				for(unsigned int i = 0; i < presetSize; ++i)
					indices[i] = 0;

				bool done = false;
				while(true)
				{
					GenerateSuccessorForCurrentPermutation(*(*iter), indices, currTransitionIdx, presetSize, marking, succ);

					// Generate next permutation of input tokens
					int j = presetSize - 1;
					while(true)
					{
						indices[j] += 1;
						if(indices[j] < arcsArray[currTransitionIdx+j])
							break;
						indices[j] = 0;
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

			currTransitionIdx += presetSize;
		}
	}

	// After the input arcs and tokens have been collected
	// you can check the enabledness of a transition by specifying
	// the index of the first input arc of the transition
	// along with the size of the preset of the transition
	bool SuccessorGenerator::IsTransitionEnabled(unsigned int currTransitionIndex, unsigned int presetSize) const
	{
		for(unsigned int i = currTransitionIndex; i < currTransitionIndex + presetSize; ++i)
		{
			if(arcsArray[i] <= 0)
				return false;
		}
		return true;
	}

	// Generates a successor node for the current permutation of input tokens
	void SuccessorGenerator::GenerateSuccessorForCurrentPermutation(const TAPN::TimedTransition& transition, const unsigned int* currPermutationindices, const unsigned int currTransitionIndex, const unsigned int presetSize, const SymMarking* marking, std::vector<Successor>& succ)
	{
		unsigned int kBound = options.GetKBound();
		bool trace = options.GetTrace() != NONE;

		const Pairing& pairing = tapn.GetPairing(transition);
		const TAPN::TimedInputArc::WeakPtrVector& preset = transition.GetPreset();
		std::vector<int> tokensToRemove;
		SymMarking* next = marking->clone();

		// move all tokens that are currently in the net
		for(unsigned int i = 0; i < presetSize; ++i)
		{
			boost::shared_ptr<TAPN::TimedInputArc> ia = preset[i].lock();
			int inputPlace = tapn.GetPlaceIndex(ia->InputPlace());
			const TAPN::TimeInterval& ti = ia->Interval();
			const std::list<int>& outputPlaces = pairing.GetOutputPlacesFor(inputPlace);

			assert(outputPlaces.size() <= 1);

			for(std::list<int>::const_iterator opIter = outputPlaces.begin(); opIter != outputPlaces.end(); ++opIter)
			{
				// change placement
				int tokenIndex = tokenIndices->at_element(currTransitionIndex+i, currPermutationindices[i]);
				int outputPlaceIndex = *opIter;
				//int originalPlaceIndex = next->GetTokenPlacement(tokenIndex);

				// constrain dbm with lower bound and upper bound in guard
				//if(!(useInfinityPlaces && tapn.IsPlaceInfinityPlace(originalPlaceIndex)))
					next->Constrain(tokenIndex, ti);

				if(outputPlaceIndex == TAPN::TimedPlace::BottomIndex())
					tokensToRemove.push_back(tokenIndex);
				else
					next->MoveToken(tokenIndex, outputPlaceIndex);

				if(next->Zone().isEmpty())
				{
					delete next;
					return;
				}
			}
		}

		// reset clocks of moved tokens
		for (unsigned int i = 0; i < presetSize; ++i) {
			int tokenIndex = tokenIndices->at_element(currTransitionIndex+i, currPermutationindices[i]);

			next->ResetClock(tokenIndex);
		}

		// check if we need to add or remove tokens in the successor marking
		int diff = presetSize - transition.GetPostsetSize();
		if(diff > 0) // preset bigger than postset, i.e. more tokens consumed than produced
		{
			assert(tokensToRemove.size() == std::abs(diff));

			// remove tokens in placement and dbm
			next->RemoveTokens(tokensToRemove);
		}
		else if(diff < 0) // postset bigger than preset, i.e. more tokens produced than consumed
		{
			const std::list<int>& outputPlaces = pairing.GetOutputPlacesFor(TAPN::TimedPlace::BottomIndex());

			if(next->GetNumberOfTokens() + outputPlaces.size() > kBound) {
				delete next;
				return;
			}

			next->AddTokens(outputPlaces,tapn);
		}

		next->DBMIntern();
		if(trace){
			TraceInfo traceInfo(marking->Id(), transition.GetIndex(), next->Id());

			for(unsigned int i = 0; i < presetSize; ++i)
			{
				int tokenIndex = tokenIndices->at_element(currTransitionIndex+i, currPermutationindices[i]);
				boost::shared_ptr<TAPN::TimedInputArc> ia = preset[i].lock();
				const TAPN::TimeInterval& ti = ia->Interval();
				int indexAfterFiring = tokenIndex;
				for(std::vector<int>::iterator iter = tokensToRemove.begin(); iter != tokensToRemove.end(); ++iter){
					if(*iter < tokenIndex) indexAfterFiring--;
					else if(*iter == tokenIndex){
						indexAfterFiring = -1;
						break;
					}
				}

				Participant participant(tokenIndex, ti, indexAfterFiring);
				traceInfo.AddParticipant(participant);
			}

			if(diff < 0){
				TAPN::TimeInterval inf;
				for(int i = diff; i < 0; i++){
					Participant participant(-1, inf, (marking->GetNumberOfTokens() - diff + i));
					traceInfo.AddParticipant(participant);
				}
			}
			traceInfo.setMarking(next);
			succ.push_back(Successor(next, traceInfo));
		}else{
			succ.push_back(Successor(next));
		}

		tokensToRemove.clear();
	}


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
