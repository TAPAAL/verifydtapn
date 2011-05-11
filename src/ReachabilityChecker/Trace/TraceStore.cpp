#include "TraceStore.hpp"
#include "EntrySolver.hpp"
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

    void TraceStore::OutputTraceTo(const TAPN::TimedArcPetriNet & tapn) const
    {
        id_type currentId = finalMarkingId;
        std::deque<TraceInfo> traceInfos;
        while(currentId != 0){
            TraceInfo* info = store.find(currentId)->second;
            traceInfos.push_front(*info);
            currentId = info->PreviousStateId();
        }

        std::vector<decimal> delays;
        if(traceInfos.size() > 0)
        {
        	AugmentSymmetricMappings(traceInfos);
        	ComputeIndexMappings(traceInfos);
        	CalculateDelays(traceInfos, delays);
        }
        std::cout << std::endl;
        std::cerr << "Trace: " << std::endl;
       	ConcreteMarking marking(initialMarking);

        if(options.XmlTrace())
        {
        	OutputTraceInXmlFormat(marking, tapn, traceInfos, delays);
        }
        else
        {
        	OutputTraceInNormalFormat(marking, tapn, traceInfos, delays);
        }
    }

	void TraceStore::CalculateDelays(const std::deque<TraceInfo>& traceInfos, std::vector<decimal>& delays) const
	{
		EntrySolver solver(options.GetKBound(), traceInfos);
		std::vector<decimal> calculatedDelays = solver.CalculateDelays(lastInvariants);
		delays.swap(calculatedDelays);
	}

	void TraceStore::OutputTraceInNormalFormat(ConcreteMarking& marking, const TAPN::TimedArcPetriNet& tapn, const std::deque<TraceInfo>& traceInfos, const std::vector<decimal>& delays) const
	{
		std::cerr << "\t" << marking;
		TAPN::TimedTransition::Vector transitions = tapn.GetTransitions();
		for(unsigned int i = 0;i < traceInfos.size();++i){
			const TraceInfo & traceInfo = traceInfos[i];
			int index = traceInfo.TransitionIndex();
			const TAPN::TimedTransition & transition = *transitions[index];
			if(delays[i] > decimal(0)){
				std::cerr << "\tDelay: " << delays[i] << std::endl;
				marking.Delay(delays[i]);
				std::cerr << "\t" << marking;
			}
			std::cerr << "\tTransition: " << transition.GetName() << std::endl;
			UpdateMarking(marking, traceInfo, tapn);
			std::cerr << "\t" << marking;
		}

	}

	void TraceStore::OutputTraceInXmlFormat(ConcreteMarking& marking, const TAPN::TimedArcPetriNet& tapn, const std::deque<TraceInfo>& traceInfos, const std::vector<decimal>& delays) const
	{
		using namespace rapidxml;
		xml_document<> doc;
		xml_node<>* root = doc.allocate_node(node_element, "trace");
		doc.append_node(root);

		const TAPN::TimedTransition::Vector& transitions = tapn.GetTransitions();
		for(unsigned int i = 0; i < traceInfos.size(); ++i)
		{
			const TraceInfo& traceInfo = traceInfos[i];

			decimal delay = delays[i];
			if(delay > decimal(0)){
				xml_node<>* node = doc.allocate_node(node_element, "delay", doc.allocate_string(ToString(delay).c_str()));
				root->append_node(node);
				marking.Delay(delay);
			}

			xml_node<>* transition_node = doc.allocate_node(node_element, "transition");
			xml_attribute<>* id = doc.allocate_attribute("id", doc.allocate_string(transitions[traceInfo.TransitionIndex()]->GetName().c_str()));
			transition_node->append_attribute(id);

			for(std::vector<Participant>::const_iterator it = traceInfo.Participants().begin(); it != traceInfo.Participants().end(); it++)
			{
				unsigned int index = it->TokenIndex();
				unsigned int indexAfterFiring = traceInfo.GetTransitionFiringMapping().MapForward(index);
				unsigned int symmetric_index = traceInfo.GetSymmetricMapping().MapForward(indexAfterFiring);
				unsigned int mapped_token_index = traceInfo.GetOriginalMapping()[symmetric_index];
				const Token& token = marking[mapped_token_index];
				if(token.Place() != TAPN::TimedPlace::BOTTOM_NAME)
				{
					xml_node<>* token_node = doc.allocate_node(node_element, "token");
					xml_attribute<>* place_attr = doc.allocate_attribute("place", doc.allocate_string(token.Place().c_str()));
					xml_attribute<>* age_attr = doc.allocate_attribute("age", doc.allocate_string(ToString(token.Age()).c_str()));
					token_node->append_attribute(place_attr);
					token_node->append_attribute(age_attr);
					transition_node->append_node(token_node);
				}
			}
			root->append_node(transition_node);
			UpdateMarking(marking, traceInfo, tapn);
		}

		std::cerr << doc;
	}
}
