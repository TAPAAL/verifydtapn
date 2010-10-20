#include "SuccessorGenerator.hpp"
#include "../TAPN/TimedInputArc.hpp"
#include "../Core/SymMarking.hpp"
#include "../Core/Pairing.hpp"
#include "dbm/print.h"

namespace VerifyTAPN {
	void SuccessorGenerator::GenerateDiscreteTransitionsSuccessors(const SymMarking* marking, std::vector<SymMarking*>& succ)
	{
		const TAPN::TimedTransition::Vector& transitions = tapn->GetTransitions();

		CollectArcsAndAppropriateTokens(transitions, marking);

		GenerateSuccessors(transitions, marking, succ);
	}

	void SuccessorGenerator::CollectArcsAndAppropriateTokens(const TAPN::TimedTransition::Vector& transitions, const SymMarking* marking)
	{
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
				const TAPN::TimeInterval& ti = ia->Interval();
				const TokenMapping& map = marking->GetTokenMapping();

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
	}

	void SuccessorGenerator::GenerateSuccessors(const TAPN::TimedTransition::Vector& transitions, const SymMarking* marking, std::vector<SymMarking*>& succ)
	{
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
				std::vector<int> tokensToRemove;


				unsigned int indices[presetSize];
				for(unsigned int i = 0; i < presetSize; ++i)
					indices[i] = 0;

				bool done = false;
				while(true)
				{
					bool skipCurrentPermutation = false;
					SymMarking* next = marking->clone();

					for(unsigned int i = 0; i < presetSize; ++i)
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
							int outputPlaceIndex = tapn->GetPlaceIndex(*opIter);

							if(outputPlaceIndex == TAPN::TimedPlace::BottomIndex())
								tokensToRemove.push_back(tokenMappingIdx);

							next->MoveToken(tokenIndex, outputPlaceIndex);

							// constrain dbm with lower bound and upper bound in guard
							next->Constrain(tokenMappingIdx, ti);

//							std::cout << "-------------------------------\n";
//							std::cout << "Next DBM constrained on mapping index: " << tokenMappingIdx << " with time interval: " << ti << "\n";
//							std::cout << "\n------------------------------------\n";
//							std::cout << next->Zone() << "\n-----------------------------------------\n\n";

							if(next->Zone().isEmpty())
							{
								skipCurrentPermutation = true;
								break;
							}

						}
						if(skipCurrentPermutation)
							break;
					}

					for (unsigned int i = 0; i < presetSize; ++i) {
						int tokenMappingIdx = tokenIndices->at_element(currTransitionIdx+i, indices[i]);
						// reset age to 0
						next->ResetClock(tokenMappingIdx);
					}

//					std::cout << "-------------------------------\n";
//					std::cout << "Next DBM with clocks reset:\n";
//					std::cout << "\n------------------------------------\n";
//					std::cout << next->Zone() << "\n-----------------------------------------\n\n";

					if(!skipCurrentPermutation)
					{
						// check if we need to add or remove tokens in the successor marking
						int diff = presetSize - (*iter)->GetPostsetSize();
						if(diff > 0) // preset bigger than postset, i.e. more tokens consumed than produced
						{
							assert(tokensToRemove.size() == std::abs(diff));

							// remove diff active tokens, placement already fixed
							next->RemoveInactiveTokensFromDBM(tokensToRemove);
						}
						else if(diff < 0) // postset bigger than preset, i.e. more tokens produced than consumed
						{
							int dim = next->Zone().getDimension();

							// add diff active tokens and move diff tokens from BOTTOM to paired places
							next->AddActiveTokensToDBM(std::abs(diff));

							std::list<TAPN::TimedPlace> outputPlaces = pairing.GetOutputPlacesFor(TAPN::TimedPlace::Bottom());
							int i = 0;
							for(std::list<TAPN::TimedPlace>::const_iterator bottomIter = outputPlaces.begin(); bottomIter != outputPlaces.end(); ++bottomIter)
							{
								// change token placement
								int tokenIndex = next->MoveFirstTokenAtBottomTo(tapn->GetPlaceIndex(*bottomIter));

								assert(tokenIndex > 0); // if this assertion fails a token should have been moved from bottom but there are currently no tokens in bottom

								next->AddTokenToMapping(tokenIndex);

								// reset clock to 0
								next->ResetClock(dim+i);
								i++;

							}
						}

						succ.push_back(next);
						tokensToRemove.clear();
					}


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
