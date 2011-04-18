#include "SearchStrategy.hpp"
#include "../Core/TAPN/TimedArcPetriNet.hpp"
#include "Successor.hpp"
#include "../typedefs.hpp"

//TODO: delete
#include "../Core/SymbolicMarking/DBMMarking.hpp"
#include "dbm/print.h"

namespace VerifyTAPN
{
	DefaultSearchStrategy::DefaultSearchStrategy(
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
		SymbolicMarking* initialMarking,
		const AST::Query* query,
		const VerificationOptions& options,
		MarkingFactory* factory
	) : tapn(tapn), initialMarking(initialMarking), checker(query), options(options), succGen(tapn, *factory, options), factory(factory), traceStore(options, initialMarking, tapn), identity_map()
	{
		if(options.GetSearchType() == DEPTHFIRST)
			pwList = new PWList(new StackWaitingList, factory);
		else
			pwList = new PWList(new QueueWaitingList, factory);

		maxConstantsArray = new int[options.GetKBound()+1];
		for(int i = 0; i < options.GetKBound()+1; ++i)
		{
			maxConstantsArray[i] = tapn.MaxConstant();
		}

		for(unsigned int i = 0; i < options.GetKBound(); i++)
		{
			identity_map.insert(BiMap::value_type(i,i));
		}
	};


	bool DefaultSearchStrategy::Verify()
	{
		initialMarking->Delay();
		UpdateMaxConstantsArray(*initialMarking);
		initialMarking->Extrapolate(maxConstantsArray);

		if(options.GetSymmetryEnabled()){
			BiMap bimap;
			initialMarking->MakeSymmetric(bimap);
		}

		pwList->Add(*initialMarking);
		if(CheckQuery(*initialMarking)){
			if(options.GetTrace() != NONE) traceStore.SetFinalMarkingId(initialMarking->UniqueId());
			return checker.IsEF(); // return true if EF query (proof found), or false if AG query (counter example found)
		}

		while(pwList->HasWaitingStates())
		{
			SymbolicMarking* next = pwList->GetNextUnexplored();

			typedef std::vector<Successor> SuccessorVector;
			SuccessorVector successors;
			std::vector<TraceInfo> traceInfos;

			succGen.GenerateDiscreteTransitionsSuccessors(*next, successors);

			for(SuccessorVector::iterator iter = successors.begin(); iter != successors.end(); ++iter)
			{
				SymbolicMarking& succ = *(*iter).Marking();
				succ.Delay();

				UpdateMaxConstantsArray(succ);

				succ.Extrapolate(maxConstantsArray);

				BiMap bimap;
				if(options.GetSymmetryEnabled())
				{
					succ.MakeSymmetric(bimap);
				}
				if(options.GetTrace() != NONE){

					TraceInfo traceInfo = iter->GetTraceInfo();
					traceInfo.SetSymmetricMapping(IndirectionTable(bimap));
					traceStore.Save(succ.UniqueId(), traceInfo);
				}

				bool added = pwList->Add(succ);

				if(added && CheckQuery(succ)){
					factory->Release(iter->Marking());
					if(options.GetTrace() != NONE) traceStore.SetFinalMarkingId(iter->Marking()->UniqueId());

					return checker.IsEF();
				}
				factory->Release(iter->Marking());
			}
			factory->Release(next);

			//PrintDiagnostics(successors.size());
		}
		return checker.IsAG(); // return true if AG query (no counter example found), false if EF query (no proof found)
	}

	// Finds the local max constants for each token to be used for extrapolation.
	// If infinity place optimization is enabled, tokens in such a place are "marked"
	// as inactive by supplying a max constant of -dbm_INFINITY.
	void DefaultSearchStrategy::UpdateMaxConstantsArray(const SymbolicMarking& marking)
	{
		for(unsigned int tokenIndex = 0; tokenIndex < marking.NumberOfTokens(); ++tokenIndex)
		{
			int placeIndex = marking.GetTokenPlacement(tokenIndex);

			if(options.GetUntimedPlacesEnabled() && tapn.IsPlaceUntimed(placeIndex))
				maxConstantsArray[marking.GetClockIndex(tokenIndex)] = -INF;
			else if(!options.GetGlobalMaxConstantsEnabled())
			{
				const TAPN::TimedPlace& p = tapn.GetPlace(placeIndex);
				maxConstantsArray[marking.GetClockIndex(tokenIndex)] = p.GetMaxConstant();
			}else{
				maxConstantsArray[marking.GetClockIndex(tokenIndex)] = tapn.MaxConstant();
			}
		}
	}

	bool DefaultSearchStrategy::CheckQuery(const SymbolicMarking& marking) const
	{
		bool satisfied = checker.IsExpressionSatisfied(marking);
		return (satisfied && checker.IsEF()) || (!satisfied && checker.IsAG());
	}

	Stats DefaultSearchStrategy::GetStats() const
	{
		return pwList->GetStats();
	}

	void DefaultSearchStrategy::PrintDiagnostics(size_t successors) const
	{
		pwList->Print();
		std::cout << ", successors: " << successors;
		std::cout << std::endl;
	}

	void DefaultSearchStrategy::PrintTraceIfAny(bool result) const
	{
		if(options.GetTrace() != NONE){
			if((checker.IsAG() && result) || (checker.IsEF() && !result))
				std::cout << "A trace could not be generated due to the query result." << std::endl;
			else{
				traceStore.OutputTraceTo(tapn);
			}
		}
	}
}
