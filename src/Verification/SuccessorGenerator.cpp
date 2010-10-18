#include "SuccessorGenerator.hpp"
#include "../TAPN/TimedInputArc.hpp"
#include "../Core/SymMarking.hpp"
#include "../Core/Pairing.hpp"

namespace VerifyTAPN {
	void SuccessorGenerator::GenerateDiscreteTransitionsSuccessors(const SymMarking* marking, std::vector<SymMarking*>& succ)
	{
		const TAPN::TimedTransition::Vector& transitions = tapn->GetTransitions();
		unsigned int currInputArcIdx = 0;


		// for all input arcs
		//		collect Arcs - should be sorted by transition in arcsArray
		//		mark transitions
		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			const TAPN::TimedInputArc::WeakPtrVector& preset = (*iter)->GetPreset();
			for(TAPN::TimedInputArc::WeakPtrVector::const_iterator presetIter = preset.begin(); presetIter != preset.end(); ++presetIter)
			{
				boost::shared_ptr<TAPN::TimedInputArc> ia = (*presetIter).lock();
				const TokenMapping& map = marking->GetTokenMapping();
				const TAPN::TimeInterval& ti = ia->Interval();

				unsigned int nTokensFromCurrInputPlace = 0;
				for(unsigned int i = 0; i < map.size(); i++)
				{
					int tokenIndex = map.GetMapping(i);
					int placeIndex = marking->GetTokenPlacement(tokenIndex);

					if(placeIndex >= 0 && placeIndex == tapn->GetPlaceIndex(ia->InputPlace()))
					{
						// check lower bound
						bool isLowerBoundSat = marking->Zone().satisfies(0,i,ti.LowerBoundToDBMRaw());

						// check upper bound
						bool isUpperBoundSat = marking->Zone().satisfies(i,0, ti.UpperBoundToDBMRaw());

						bool notAppropriateAge = !isLowerBoundSat || !isUpperBoundSat;

						if(!notAppropriateAge) // token satisfies guard
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

		// for all marked transitions
		// 		try to take them
		int currTransitionIdx = 0;
		for(TAPN::TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			unsigned int presetSize = (*iter)->GetPresetSize();
			bool isEnabled = true;
			for(unsigned int i = currTransitionIdx; i < currTransitionIdx + presetSize; ++i)
			{
				if(arcsArray[i] <= 0)
				{
					isEnabled = false;
					break;
				}
			}



			if(isEnabled)
			{
				// generate permutations of successors
				Pairing pairing(*(*iter));
				const TAPN::TimedInputArc::WeakPtrVector& preset = (*iter)->GetPreset();



				unsigned int indices[presetSize];
				for(int i = 0; i < presetSize; ++i)
					indices[i] = 0;

				bool done = false;
				while(true)
				{
					bool skipCurrentPermutation = false;
					SymMarking* next = marking->clone();

					for(int i = 0; i < presetSize; ++i)
					{
						boost::shared_ptr<TAPN::TimedInputArc> ia = preset[i].lock();
						const TAPN::TimedPlace& inputPlace = ia->InputPlace();
						const TokenMapping& map = marking->GetTokenMapping();
						const TAPN::TimeInterval& ti = ia->Interval();
						std::list<TAPN::TimedPlace> outputPlaces = pairing.GetOutputPlacesFor(inputPlace);

						assert(outputPlaces.size() <= 1);

						for(std::list<TAPN::TimedPlace>::const_iterator opIter = outputPlaces.begin(); opIter != outputPlaces.end(); ++opIter)
						{
							// change placement
							int tokenMappingIdx = tokenIndices->at_element(currTransitionIdx+i, indices[i]);
							int tokenIndex = map.GetMapping(tokenMappingIdx);
							next->MoveToken(tokenIndex,tapn->GetPlaceIndex(*opIter));

							// constrain dbm with lower bound and upper bound in guard
							next->Constrain(tokenMappingIdx, ti);

							if(next->Zone().isEmpty())
							{
								skipCurrentPermutation = true;
								break;
							}

							// reset age to 0
							next->ResetClock(tokenMappingIdx);
						}
						if(skipCurrentPermutation)
							break;
					}


					if(!skipCurrentPermutation)
					{
						// check if we need to add or remove tokens in the successor marking
						int diff = presetSize - (*iter)->GetPostsetSize();
						if(diff > 0) // preset bigger than postset, i.e. more tokens consumed than produced
						{
							// remove diff active tokens, placement already fixed
							next->RemoveInactiveToken(diff);
						}
						else if(diff < 0) // postset bigger than preset, i.e. more tokens produced than consumed
						{
							// add diff active tokens and move diff tokens from BOTTOM to paired places
							next->AddActiveToken(std::abs(diff));

							std::list<TAPN::TimedPlace> outputPlaces = pairing.GetOutputPlacesFor(TAPN::TimedPlace::Bottom());
							for(std::list<TAPN::TimedPlace>::const_iterator bottomIter = outputPlaces.begin(); bottomIter != outputPlaces.end(); ++bottomIter)
							{
								// change token placement
								next->MoveFirstTokenAtBottomTo(tapn->GetPlaceIndex(*bottomIter));
							}

						}

						succ.push_back(next);
					}



//					for(int i = 0; i < presetSize; ++i)
//					{
//						// Generate concrete successor
//
//
//						std::cout << tokenIndices->at_element(currTransitionIdx+i, indices[i]) << " ";
//					}
					std::cout << "\n";

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

//				int i = 0;
//				while(true)
//				{
//
//					int j = i;
//					for(int k = 0; k < presetSize; ++k)
//					{
//						std::cout << tokenIndices->at_element(currTransitionIdx+k, j % arcsArray[currTransitionIdx+k]) << " ";
//						j /= arcsArray[currTransitionIdx+k];
//					}
//					std::cout << "\n";
//					if(j > 0)
//						break;
//
//					i += 1;
//				}

			}

			currTransitionIdx += presetSize;
		}


	}


	void SuccessorGenerator::ClearAll()
	{
		ClearArcsArray();
		ClearTokenIndices();
	}

	void SuccessorGenerator::ClearArcsArray()
	{
		for(unsigned int i = 0; i < nInputArcs; i++)
			arcsArray[i] = 0;
	}

	void SuccessorGenerator::ClearTokenIndices()
	{
		for (unsigned i = 0; i < tokenIndices->size1(); ++i)
			for (unsigned j = 0; j < tokenIndices->size2(); ++j)
				tokenIndices->insert_element(i, j, -1);
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
