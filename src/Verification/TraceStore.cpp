#include "TraceStore.hpp"
#include "../TAPN/TAPN.hpp"
#include "dbm/print.h"
#include "../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "boost/lexical_cast.hpp"

namespace VerifyTAPN{
	using namespace rapidxml;
	void TraceStore::OutputTraceTo(const SymMarking::id_type& id, const TAPN::TimedArcPetriNet& tapn) const
	{
		SymMarking::id_type currentId = id;
		std::vector<TraceInfo> traceInfos;
		while(currentId != 0){
			const TraceInfo& info = store.find(currentId)->second;
			traceInfos.push_back(info);
			currentId = info.PreviousStateId();
		}
		xml_document<>* doc = CreateInputDocForCTU(traceInfos);
		std::cout << std::endl << std::endl << *doc << std::endl << std::endl;

		TAPN::TimedTransition::Vector transitions = tapn.GetTransitions();
		std::cout << std::endl << "Trace: " << std::endl;
		for(std::vector<TraceInfo>::reverse_iterator iter = traceInfos.rbegin(); iter != traceInfos.rend(); ++iter)
		{
			int index = iter->TransitionIndex();
			const TAPN::TimedTransition& transition = *transitions[index];
			std::cout << "\tTransition: " << transition.GetName()  << std::endl;
			std::cout << "\t\tParticipants:" << std::endl;

			const std::vector<Participant>& participants = iter->Participants();
			for(std::vector<Participant>::const_iterator parIter = participants.begin(); parIter != participants.end(); ++parIter)
			{
				std::cout << "\t\t\t";
				std::cout << parIter->TokenIndex() << "," << parIter->GetTimeInterval() << "," << parIter->IndexAfterFiring();
				std::cout << std::endl;
			}

			std::cout << std::endl << iter->Marking().Zone() << std::endl;
		}
	}

	xml_document<>* TraceStore::CreateInputDocForCTU(const std::vector<TraceInfo>& traceInfos) const
	{
		xml_document<>* doc = new xml_document<>();
		xml_node<>* node = doc->allocate_node(node_element, "trace");
		doc->append_node(node);
		xml_attribute<>* attr = doc->allocate_attribute("initial_node", "State1");
		node->append_attribute(attr);
		attr = doc->allocate_attribute("trace_options", "symbolic");
		node->append_attribute(attr);

		GenerateSystemDescription(*doc, *node, traceInfos);

		return doc;
	}

	void TraceStore::GenerateSystemDescription(xml_document<>& doc, xml_node<>& root, const std::vector<TraceInfo>& traceInfos) const
	{
		/////////// SYSTEM
		xml_node<>* system = doc.allocate_node(node_element, "system");
		root.append_node(system);

		////////// CLOCKS
		xml_node<>* node = doc.allocate_node(node_element, "clock");
		xml_attribute<>* attr = doc.allocate_attribute("name", "t(0)");
		node->append_attribute(attr);
		attr = doc.allocate_attribute("id", "sys.t(0)");
		node->append_attribute(attr);
		system->append_node(node);

		for(int i = 0; i < kbound; ++i)
		{
			std::string clockname = "x" + boost::lexical_cast<std::string>(i);
			std::string clockid = "sys." + clockname;
			node = doc.allocate_node(node_element, "clock");
			attr = doc.allocate_attribute("name", doc.allocate_string(clockname.c_str()));
			node->append_attribute(attr);
			attr = doc.allocate_attribute("id", doc.allocate_string(clockid.c_str()));
			node->append_attribute(attr);
			system->append_node(node);
		}

		////////////// PROCESS
		std::string templateName("Template");
		xml_node<>* process = doc.allocate_node(node_element, "process");
		attr = doc.allocate_attribute("id", doc.allocate_string(templateName.c_str()));
		process->append_attribute(attr);
		attr = doc.allocate_attribute("name", doc.allocate_string(templateName.c_str()));
		process->append_attribute(attr);
		system->append_node(process);

		////////////// LOCATIONS
		for(unsigned int i = 0; i < traceInfos.size() + 1; ++i)
		{
			std::string locName = "_id" + boost::lexical_cast<std::string>(i);
			std::string locId = templateName + "." + locName;

			node = doc.allocate_node(node_element, "location");
			attr = doc.allocate_attribute("id", doc.allocate_string(locId.c_str()));
			node->append_attribute(attr);
			attr = doc.allocate_attribute("name", doc.allocate_string(locName.c_str()));
			node->append_attribute(attr);
			process->append_node(node);
		}

		////////////// EDGES
		for(int i = traceInfos.size()-1; i >= 0; --i)
		{
			const TraceInfo& traceInfo = traceInfos[i];
			std::string id = templateName + ".Edge" + boost::lexical_cast<std::string>(i);
			std::string from = templateName + "._id" + boost::lexical_cast<std::string>(i);
			std::string to = templateName + "._id" + boost::lexical_cast<std::string>(i+1);
			node = doc.allocate_node(node_element, "edge");
			attr = doc.allocate_attribute("id", doc.allocate_string(id.c_str()));
			node->append_attribute(attr);
			attr = doc.allocate_attribute("from", doc.allocate_string(from.c_str()));
			node->append_attribute(attr);
			attr = doc.allocate_attribute("to", doc.allocate_string(to.c_str()));
			node->append_attribute(attr);

			xml_node<>* subnode = doc.allocate_node(node_element, "guard", doc.allocate_string(GenerateGuardFromTraceInfo(traceInfo).c_str()));
			node->append_node(subnode);
			subnode = doc.allocate_node(node_element, "sync", "tau");
			node->append_node(subnode);
			subnode = doc.allocate_node(node_element, "update", doc.allocate_string(GenerateUpdateFromTraceInfo(traceInfo).c_str()));
			node->append_node(subnode);

			process->append_node(node);
		}
	}

	std::string TraceStore::GenerateGuardFromTraceInfo(const TraceInfo& traceInfo) const
	{
		std::string guard;
		const std::vector<Participant>& participants = traceInfo.Participants();
		for(std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); ++iter)
		{
			if(iter->TokenIndex() != -1)
			{
				if(!guard.empty()) guard += " && ";
				std::string clock = "x" + boost::lexical_cast<std::string>(iter->TokenIndex());
				const TAPN::TimeInterval& ti = iter->GetTimeInterval();

				guard += clock;
				guard += ti.IsLowerBoundStrict() ? " > " : " >= ";
				guard += boost::lexical_cast<std::string>(ti.GetLowerBound());

				if(ti.GetUpperBound() != std::numeric_limits<int>::max()){
					guard += " && ";
					guard += clock;
					guard += ti.IsLowerBoundStrict() ? " < " : " <= ";
					guard += boost::lexical_cast<std::string>(ti.GetUpperBound());
				}
			}
		}

		return guard.empty() ? "1" : guard;
	}

	std::string TraceStore::GenerateUpdateFromTraceInfo(const TraceInfo& traceInfo) const
		{
			return "";
		}
}
