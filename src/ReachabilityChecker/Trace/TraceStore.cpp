#include "TraceStore.hpp"
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "../../../lib/rapidxml-1.13/rapidxml.hpp"
#include "../../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "../../Core/TAPNParser/util.hpp"

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
		for(unsigned int i = 0; i < traceInfos.size(); i++)
		{
			//const TraceInfo& previous = traceInfos[i-1];
			TraceInfo& current = traceInfos[i];

			const std::vector<unsigned int>& previousTable = i > 0 ? traceInfos[i-1].GetOriginalMapping() : identity_map;
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
}
