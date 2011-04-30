#include "TraceStore.hpp"
#include "EntrySolver.hpp"
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"

namespace VerifyTAPN
{
	void UpdateMarking(ConcreteMarking& marking, const TraceInfo& traceInfo, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
	{
		const std::vector<Participant>& participants = traceInfo.Participants();
		for(std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); ++iter)
		{
			unsigned int index = iter->TokenIndex();
			unsigned int indexAfterFiring = traceInfo.GetTransitionFiringMapping().MapForward(index);
			unsigned int symmetric_index = traceInfo.GetSymmetricMapping().MapForward(indexAfterFiring);
			unsigned int mapped_token_index = traceInfo.GetOriginalMapping()[symmetric_index];

			const std::string& place = iter->PlacementAfterFiring() == -1 ? TAPN::TimedPlace::BOTTOM_NAME : tapn.GetPlace(iter->PlacementAfterFiring()).GetName();
			Token& token = marking[mapped_token_index];
			token.SetPlace(place);
			if(iter->GetArcType() == NORMAL_ARC) token.Reset();
		}
	}

	void dumpTraceInfo(const TraceInfo& traceInfo, const TAPN::TimedArcPetriNet& tapn)
	{
		std::cout << tapn.GetTransitions()[traceInfo.TransitionIndex()]->GetName() << " (table size: " << traceInfo.GetSymmetricMapping().Size() << ")\n";
		std::cout << "  Participants:\n";
		for(std::vector<Participant>::const_iterator it = traceInfo.Participants().begin(); it != traceInfo.Participants().end(); it++)
		{
			std::cout << "  " << it->TokenIndex() <<  ", (" << (it->GetArcType() == NORMAL_ARC ? "normal" : "transport") << ")\n";
		}
		std::cout << "\n";
		std::cout << "  Invariants:\n";
		for(std::vector<TraceInfo::Invariant>::const_iterator it = traceInfo.GetInvariants().begin(); it != traceInfo.GetInvariants().end(); it++)
		{
			std::cout << "  c" << it->first << it->second << "\n";
		}
		std::cout << "\n";
		std::cout << "\n  Transition firing indirection table:\n";
		for(unsigned int i = 0; i < traceInfo.GetTransitionFiringMapping().Size(); i++)
		{
			std::cout << "  " << i << " <--> " << traceInfo.GetTransitionFiringMapping().MapForward(i) << ", ";
		}
		std::cout << "\n";
		std::cout <<  "\n  Symmetry indirection table:\n";
		for(unsigned int i = 0; i < traceInfo.GetSymmetricMapping().Size(); i++)
		{
			std::cout << "  " << i << " <--> " << traceInfo.GetSymmetricMapping().MapForward(i) << ", ";
		}

		std::cout << "\n";
		std::cout << "\n  OriginalMapping:\n";
		for(unsigned int i = 0; i < traceInfo.GetOriginalMapping().size(); i++)
		{
			std::cout << "  " << i << ":" << traceInfo.GetOriginalMapping()[i] << ", ";
		}
		std::cout << "\n\n";
	}

	void TraceStore::AugmentSymmetricMappings(std::deque<TraceInfo>& traceInfos) const
	{
		for(std::deque<TraceInfo>::iterator it = traceInfos.begin(); it != traceInfos.end(); ++it)
		{
			IndirectionTable& table = it->GetSymmetricMapping();
			for(unsigned int i = table.Size(); i < static_cast<unsigned int>(options.GetKBound()); i++)
			{
				table.AddMapping(i,i);
			}
		}
	}

	// This function trasitively applies the symmetric and transition firing mappings, to aid
	// in the construction of an equivalent non-symmetric trace.
	// See TraceInfo.hpp for more info.
	void TraceStore::ComputeIndexMappings(std::deque<TraceInfo>& traceInfos) const
	{
		std::vector<unsigned int> map(identity_map);
		TraceInfo& first = traceInfos[0];
		const IndirectionTable& table = first.GetSymmetricMapping();
		for(unsigned int symmetric_index = 0; symmetric_index < static_cast<unsigned int>(options.GetKBound()); symmetric_index++)
		{
			assert(symmetric_index < table.Size());
			map[symmetric_index] = table.MapBackwards(symmetric_index);
		}
		first.SetOriginalMapping(map);

		for(unsigned int i = 1; i < traceInfos.size(); i++)
		{
			const TraceInfo& previous = traceInfos[i-1];
			TraceInfo& current = traceInfos[i];

			const std::vector<unsigned int>& previousTable = previous.GetOriginalMapping();
			const IndirectionTable& currentTable = current.GetSymmetricMapping();
			const IndirectionTable& transitionMapping = current.GetTransitionFiringMapping();

			std::vector<unsigned int> mapping(identity_map);
			for(unsigned int token = 0; token < static_cast<unsigned int>(options.GetKBound()); token++)
			{
				assert(token < currentTable.Size());

				unsigned int remapped_index = currentTable.MapBackwards(token);
				int indexBeforeTransitionFiring = transitionMapping.MapBackwards(remapped_index);
				assert(indexBeforeTransitionFiring != -1);
				int originalIndex = previousTable[indexBeforeTransitionFiring];
				assert(originalIndex != -1);
				mapping[token] = originalIndex;
			}

			current.SetOriginalMapping(mapping);
		}
	}

	void TraceStore::OutputTraceTo(const TAPN::TimedArcPetriNet& tapn) const
	{
		id_type currentId = finalMarkingId;
		std::deque<TraceInfo> traceInfos;
		while(currentId != 0){
			const TraceInfo& info = store.find(currentId)->second;
			traceInfos.push_front(info);
			currentId = info.PreviousStateId();
		}

		AugmentSymmetricMappings(traceInfos);
		ComputeIndexMappings(traceInfos);

		std::vector<decimal> delays;
		CalculateDelays(traceInfos, delays);


		TAPN::TimedTransition::Vector transitions = tapn.GetTransitions();

		std::cerr << std::endl << "Trace: " << std::endl;
		ConcreteMarking marking(initialMarking);
		std::cerr << "\t" << marking;
		for(unsigned int i = 0; i < traceInfos.size(); ++i)
		{
			const TraceInfo& traceInfo = traceInfos[i];

			int index = traceInfo.TransitionIndex();
			const TAPN::TimedTransition& transition = *transitions[index];

			if(delays[i] > decimal(0)){
				std::cerr << "\tDelay: " << delays[i] << std::endl;
				marking.Delay(delays[i]);
				std::cerr << "\t" << marking;
			}
			std::cerr << "\tTransition: " << transition.GetName()  << std::endl;
			UpdateMarking(marking, traceInfo, tapn);
			std::cerr << "\t" << marking;
		}
	}

	void TraceStore::CalculateDelays(const std::deque<TraceInfo>& traceInfos, std::vector<decimal>& delays) const
	{
		EntrySolver solver(options.GetKBound(), traceInfos);
		std::vector<decimal> calculatedDelays = solver.CalculateDelays();
		delays.swap(calculatedDelays);
	}
}
