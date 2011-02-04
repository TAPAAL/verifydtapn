#include "SearchStrategy.hpp"
#include "../Core/TAPN/TimedArcPetriNet.hpp"
#include "Successor.hpp"

namespace VerifyTAPN
{
	DFS::DFS(
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
		SymMarking* initialMarking,
		const AST::Query* query,
		const VerificationOptions& options
	) : tapn(tapn), initialMarking(initialMarking), checker(query), options(options), traceStore(options, initialMarking)
	{
		if(options.GetSearchType() == DEPTHFIRST)
			pwList = new PWList(new StackWaitingList);
		else
			pwList = new PWList(new QueueWaitingList);

		maxConstantsArray = new int[options.GetKBound()+1];
		for(int i = 0; i < options.GetKBound()+1; ++i)
		{
			maxConstantsArray[i] = tapn.MaxConstant();
		}
	};

	bool DFS::Verify()
	{
		initialMarking->Delay();
		UpdateMaxConstantsArray(*initialMarking);
		initialMarking->Extrapolate(maxConstantsArray);

		if(options.GetSymmetryEnabled())
			initialMarking->Canonicalize();

		pwList->Add(*initialMarking);
		if(CheckQuery(*initialMarking)){
			if(options.GetTrace() != NONE) traceStore.SetFinalMarking(initialMarking);
			return checker.IsEF(); // return true if EF query (proof found), or false if AG query (counter example found)
		}

		while(pwList->HasWaitingStates()){
			SymMarking& next = pwList->GetNextUnexplored();

			typedef std::vector<Successor> SuccessorVector;
			SuccessorVector successors;
			std::vector<TraceInfo> traceInfos;

			next.GenerateDiscreteTransitionSuccessors(tapn, options, successors);

			for(SuccessorVector::iterator iter = successors.begin(); iter != successors.end(); ++iter)
			{
				SymMarking& succ = *(*iter).Marking();
				succ.Delay();

				UpdateMaxConstantsArray(succ);

				succ.Extrapolate(maxConstantsArray);

				if(options.GetSymmetryEnabled()) succ.Canonicalize();
				if(options.GetTrace() != NONE) traceStore.Save(succ.Id(), iter->GetTraceInfo());

				bool added = pwList->Add(succ);


				if(!added) delete (*iter).Marking();
				else if(CheckQuery(succ)){
					if(options.GetTrace() != NONE) traceStore.SetFinalMarking(iter->Marking());
					return checker.IsEF();
				}
			}

			//PrintDiagnostics(successors.size());
		}
		return checker.IsAG(); // return true if AG query (no counter example found), false if EF query (no proof found)
	}

	// Finds the local max constants for each token to be used for extrapolation.
	// If infinity place optimization is enabled, tokens in such a place are "marked"
	// as inactive by supplying a max constant of -dbm_INFINITY.
	void DFS::UpdateMaxConstantsArray(const SymMarking& marking)
	{
		for(unsigned int tokenIndex = 0; tokenIndex < marking.GetNumberOfTokens(); ++tokenIndex)
		{
			int placeIndex = marking.GetTokenPlacement(tokenIndex);

			if(options.GetInfinityPlacesEnabled() && tapn.IsPlaceInfinityPlace(placeIndex))
				maxConstantsArray[marking.GetDBMIndex(tokenIndex)] = -dbm_INFINITY;
			else if(!options.GetGlobalMaxConstantsEnabled())
			{
				const TAPN::TimedPlace& p = tapn.GetPlace(placeIndex);
				maxConstantsArray[marking.GetDBMIndex(tokenIndex)] = p.GetMaxConstant();
			}else{
				maxConstantsArray[marking.GetDBMIndex(tokenIndex)] = tapn.MaxConstant();
			}
		}
	}

	bool DFS::CheckQuery(const SymMarking& marking) const
	{
		bool satisfied = checker.IsExpressionSatisfied(marking);
		return (satisfied && checker.IsEF()) || (!satisfied && checker.IsAG());
	}

	Stats DFS::GetStats() const
	{
		return pwList->GetStats();
	}

	void DFS::PrintDiagnostics(size_t successors) const
	{
		pwList->Print();
		std::cout << ", successors: " << successors;
		std::cout << std::endl;
	}

	void DFS::PrintTraceIfAny() const
	{
		if(options.GetTrace() != NONE){
			traceStore.OutputTraceTo(tapn);
		}
	}
}
