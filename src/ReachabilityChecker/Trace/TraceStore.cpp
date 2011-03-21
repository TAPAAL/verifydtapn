#include "TraceStore.hpp"
#include "../../Core/TAPN/TAPN.hpp"
#include "dbm/print.h"
#include "../../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "boost/lexical_cast.hpp"
#include <stdio.h>
#include <fstream>
#include "boost/algorithm/string.hpp"
#include <sstream>
#include "boost/regex.hpp"

namespace VerifyTAPN{
	using namespace rapidxml;

	class Token{
		friend bool operator==(const Token&, const Token&);
		friend std::ostream& operator<<( std::ostream&, const Token& );
	public:
		Token(const std::string& place) : place(place), age(0) {};
		Token(const std::string& place, double age) : place(place), age(age) {};
		inline void Reset() { age = 0; }
		inline void Delay(double amount) { age += amount; }
		inline void SetPlace(const std::string& place) { this->place = place; }
	private:
		std::string place;
		double age;
	};

	std::ostream& operator<<( std::ostream& out, const Token& token )
	{
		out << "(" << token.place << "," << token.age << ")";
		return out;
	}

	bool operator==( const Token& a, const Token& b)
	{
		return a.place == b.place && a.age == b.age;
	}

	bool operator!=( const Token& a, const Token& b)
	{
		return !(a == b);
	}


	class ConcreteMarking{
		friend std::ostream& operator<<( std::ostream&, const ConcreteMarking& );
	public:
		ConcreteMarking() : tokens() { };

		inline void Add(const Token& token) { tokens.push_back(token); }
		inline void Remove(const Token& token) { tokens.erase(find(tokens.begin(), tokens.end(), token)); }
		inline void Remove(unsigned int i) { tokens.erase(tokens.begin() + i); };
		inline void Delay(double amount)
		{
			for(std::deque<Token>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
				iter->Delay(amount);
		}
		Token& operator[](int i)
		{
			return tokens[i];
		}
	private:
		std::deque<Token> tokens;
	};

	std::ostream& operator<<( std::ostream& out, const ConcreteMarking& marking )
	{
		out << "Marking: ";
		for(std::deque<Token>::const_iterator iter = marking.tokens.begin(); iter != marking.tokens.end(); ++iter)
		{
			out << *iter << " ";
		}
		out << std::endl;
		return out;
	}

//	ConcreteMarking CreateConcreteInitialMarking(const std::vector<int>& placement, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
//	{
//		ConcreteMarking marking;
//		for(std::vector<int>::const_iterator iter = placement.begin(); iter != placement.end(); ++iter)
//		{
//			Token token(tapn.GetPlace(*iter).GetName());
//			marking.Add(token);
//		}
//		return marking;
//	}
//
//	void UpdateMarking(ConcreteMarking& marking, const TraceInfo& traceInfo, const VerifyTAPN::TAPN::TimedArcPetriNet& tapn)
//	{
//		const std::vector<Participant>& participants = traceInfo.Participants();
//		for(std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); ++iter)
//		{
//			if(iter->TokenIndex() != -1)
//			{
//				if(iter->IndexAfterFiring() == -1)
//				{
//					marking.Remove(iter->TokenIndex());
//				}else{
//					Token& token = marking[iter->TokenIndex()];
//					int placeIndex = traceInfo.Marking().GetTokenPlacement(iter->IndexAfterFiring());
//					token.SetPlace(tapn.GetPlace(placeIndex).GetName());
//					token.Reset();
//				}
//			}
//			else
//			{
//				int placeIndex = traceInfo.Marking().GetTokenPlacement(iter->IndexAfterFiring());
//				marking.Add(Token(tapn.GetPlace(placeIndex).GetName()));
//			}
//		}
//	}
//
//	void TraceStore::OutputTraceTo(const TAPN::TimedArcPetriNet& tapn) const
//	{
//		SymMarking::id_type currentId = finalMarking->Id();
//		std::deque<TraceInfo> traceInfos;
//		while(currentId != 0){
//			const TraceInfo& info = store.find(currentId)->second;
//			traceInfos.push_front(info);
//			currentId = info.PreviousStateId();
//		}
//		xml_document<>* doc = CreateInputDocForCTU(traceInfos);
//		std::vector<double> delays;
//		RunCTUToObtainDelays(*doc, delays);
//
//		if(delays.size() > 0)
//		{
//			TAPN::TimedTransition::Vector transitions = tapn.GetTransitions();
//			std::cerr << std::endl << "Trace: " << std::endl;
//			ConcreteMarking marking = CreateConcreteInitialMarking(initialMarking->GetDiscretePart().GetTokenPlacementVector(), tapn);
//			std::cerr << "\t" << marking;
//			for(unsigned int i = 0; i < traceInfos.size(); ++i)
//			{
//				const TraceInfo& traceInfo = traceInfos[i];
//				int index = traceInfo.TransitionIndex();
//				const TAPN::TimedTransition& transition = *transitions[index];
//				std::cerr << "\tDelay: " << delays[i] << std::endl;
//				marking.Delay(delays[i]);
//				std::cerr << "\t" << marking;
//				std::cerr << "\tTransition: " << transition.GetName()  << std::endl;
//				UpdateMarking(marking, traceInfo, tapn);
//				std::cerr << "\t" << marking;
//			}
//		}
//	}
//
//	void TraceStore::RunCTUToObtainDelays(rapidxml::xml_document<>& doc, std::vector<double>& delays) const
//	{
//		char temp[] = "/tmp/verifytapn_trace_XXXXXX";
//		int fd = mkstemp(temp);
//		if(fd == -1){
//			std::cerr << "Could not generate temp file for CTU.";
//			return;
//		}
//		//FILE* fileptr = createTempFile(temp);
//		std::stringstream stream;
//		stream << doc;
//		std::string docAsString = stream.str();
//		boost::erase_all(docAsString, "\n");
//		boost::erase_all(docAsString, "\t");
//		write(fd, docAsString.c_str(), docAsString.size());
//
//		std::string tempFile(temp);
//		std::string output;
//		RunCTU(tempFile, output);
//
//		if(output.empty()){
//			std::cerr << "failed to run CTU. Are you sure it's located in the same directory as verifytapn?";
//			return;
//		}else{
//			ParseDelays(output, delays);
//		}
//		close(fd);
//		unlink(temp);
//	}
//
//	void TraceStore::RunCTU(const std::string& tempFile, std::string& output) const
//	{
//		std::string command = options.GetWorkingDirPath() + "/ctu -e 100 -f " + tempFile + " entry";
//		FILE* pid = popen(command.c_str(), "r");
//		if(pid == 0){
//			std::cerr << "Could not start ctu";
//			return;
//		}
//
//		std::stringstream stream;
//		const int BUFSIZE = 1000;
//		char buf[ BUFSIZE ];
//		while( fgets( buf, BUFSIZE,  pid ) ) {
//			stream << buf;
//		}
//
//		output = stream.str();
//		pclose( pid );
//	}
//
//	void TraceStore::ParseDelays(const std::string& input, std::vector<double>& delays) const
//	{
//		boost::regex e("\\d+\\s*Delay:\\s*(\\d+(?:\\.\\d+)?)");
//		boost::match_results<std::string::const_iterator> what;
//		std::string::const_iterator start = input.begin();
//		while(boost::regex_search(start, input.end(), what, e, boost::match_default))
//		{
//			std::string match(what[1].first, what[1].second);
//			double d = atof(match.c_str());
//			delays.push_back(d);
//			start = what[1].second;
//		}
//	}
//
//	xml_document<>* TraceStore::CreateInputDocForCTU(const std::deque<TraceInfo>& traceInfos) const
//	{
//		xml_document<>* doc = new xml_document<>();
//		xml_node<>* node = doc->allocate_node(node_element, "trace");
//		doc->append_node(node);
//		xml_attribute<>* attr = doc->allocate_attribute("initial_node", "State0");
//		node->append_attribute(attr);
//		attr = doc->allocate_attribute("trace_options", "symbolic");
//		node->append_attribute(attr);
//
//		GenerateSystemDescription(*doc, *node, traceInfos);
//		GenerateLocationVectors(*doc, *node, traceInfos);
//		xml_node<>* varVecNode = doc->allocate_node(node_element, "variable_vector");
//		attr = doc->allocate_attribute("id", "VarVec1");
//		varVecNode->append_attribute(attr);
//		node->append_node(varVecNode);
//		GenerateNodeDescriptions(*doc, *node, traceInfos);
//		GenerateDBMS(*doc, *node, traceInfos);
//		GenerateStates(*doc, *node, traceInfos);
//		return doc;
//	}
//
//	void TraceStore::GenerateSystemDescription(xml_document<>& doc, xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const
//	{
//		/////////// SYSTEM
//		xml_node<>* system = doc.allocate_node(node_element, "system");
//		root.append_node(system);
//
//		////////// CLOCKS
//		xml_node<>* node = doc.allocate_node(node_element, "clock");
//		xml_attribute<>* attr = doc.allocate_attribute("name", "t(0)");
//		node->append_attribute(attr);
//		attr = doc.allocate_attribute("id", "sys.t(0)");
//		node->append_attribute(attr);
//		system->append_node(node);
//
//		for(int i = 0; i < options.GetKBound(); ++i)
//		{
//			std::string clockname = "x" + boost::lexical_cast<std::string>(i);
//			std::string clockid = "sys." + clockname;
//			node = doc.allocate_node(node_element, "clock");
//			attr = doc.allocate_attribute("name", doc.allocate_string(clockname.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("id", doc.allocate_string(clockid.c_str()));
//			node->append_attribute(attr);
//			system->append_node(node);
//		}
//
//		////////////// PROCESS
//		std::string templateName("Template");
//		xml_node<>* process = doc.allocate_node(node_element, "process");
//		attr = doc.allocate_attribute("id", doc.allocate_string(templateName.c_str()));
//		process->append_attribute(attr);
//		attr = doc.allocate_attribute("name", doc.allocate_string(templateName.c_str()));
//		process->append_attribute(attr);
//		system->append_node(process);
//
//		////////////// LOCATIONS
//		for(unsigned int i = 0; i < traceInfos.size() + 1; ++i)
//		{
//			std::string locName = "_id" + boost::lexical_cast<std::string>(i);
//			std::string locId = templateName + "." + locName;
//
//			node = doc.allocate_node(node_element, "location");
//			attr = doc.allocate_attribute("id", doc.allocate_string(locId.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("name", doc.allocate_string(locName.c_str()));
//			node->append_attribute(attr);
//			process->append_node(node);
//		}
//
//		////////////// EDGES
//		for(unsigned int i = 0; i < traceInfos.size(); ++i)
//		{
//			const TraceInfo& traceInfo = traceInfos[i];
//			std::string id = templateName + ".Edge" + boost::lexical_cast<std::string>(i);
//			std::string from = templateName + "._id" + boost::lexical_cast<std::string>(i);
//			std::string to = templateName + "._id" + boost::lexical_cast<std::string>(i+1);
//			node = doc.allocate_node(node_element, "edge");
//			attr = doc.allocate_attribute("id", doc.allocate_string(id.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("from", doc.allocate_string(from.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("to", doc.allocate_string(to.c_str()));
//			node->append_attribute(attr);
//
//			xml_node<>* subnode = doc.allocate_node(node_element, "guard", doc.allocate_string(GenerateGuardFromTraceInfo(traceInfo).c_str()));
//			node->append_node(subnode);
//			subnode = doc.allocate_node(node_element, "sync", "tau");
//			node->append_node(subnode);
//			subnode = doc.allocate_node(node_element, "update", doc.allocate_string(GenerateUpdateFromTraceInfo(traceInfo).c_str()));
//			node->append_node(subnode);
//
//			process->append_node(node);
//		}
//	}
//
//	std::string TraceStore::GenerateGuardFromTraceInfo(const TraceInfo& traceInfo) const
//	{
//		std::string guard;
//		const std::vector<Participant>& participants = traceInfo.Participants();
//		for(std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); ++iter)
//		{
//			if(iter->TokenIndex() != -1)
//			{
//				if(!guard.empty()) guard += " && ";
//				std::string clock = "x" + boost::lexical_cast<std::string>(iter->TokenIndex());
//				const TAPN::TimeInterval& ti = iter->GetTimeInterval();
//
//				guard += clock;
//				guard += ti.IsLowerBoundStrict() ? " > " : " >= ";
//				guard += boost::lexical_cast<std::string>(ti.GetLowerBound());
//
//				if(ti.GetUpperBound() != std::numeric_limits<int>::max()){
//					guard += " && ";
//					guard += clock;
//					guard += ti.IsLowerBoundStrict() ? " < " : " <= ";
//					guard += boost::lexical_cast<std::string>(ti.GetUpperBound());
//				}
//			}
//		}
//
//		return guard.empty() ? "1" : guard;
//	}
//
//	std::string TraceStore::GenerateUpdateFromTraceInfo(const TraceInfo& traceInfo) const
//	{
//		std::string update;
//		const std::vector<Participant>& participants = traceInfo.Participants();
//		for(std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); ++iter)
//		{
//			if(!update.empty()) update += ", ";
//			int index = iter->TokenIndex();
//			if(index < 0) index = iter->IndexAfterFiring();
//			std::string clock = "x" + boost::lexical_cast<std::string>(index);
//			update += clock;
//			update += " := 0";
//		}
//
//		return update;
//	}
//
//	void TraceStore::GenerateLocationVectors(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const
//	{
//		for(unsigned int i = 0; i < traceInfos.size() +1; ++i)
//		{
//			xml_node<>* node = doc.allocate_node(node_element, "location_vector");
//			std::string id = "LocVec" + boost::lexical_cast<std::string>(i);
//			std::string locations = "Template._id" + boost::lexical_cast<std::string>(i);
//			xml_attribute<>* attr = doc.allocate_attribute("id", doc.allocate_string(id.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("locations", doc.allocate_string(locations.c_str()));
//			node->append_attribute(attr);
//			root.append_node(node);
//		}
//	}
//
//	void TraceStore::GenerateNodeDescriptions(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const
//	{
//		for(unsigned int i = 0; i < traceInfos.size() + 1; ++i)
//		{
//			std::string iAsString = boost::lexical_cast<std::string>(i);
//			std::string id = "State" + iAsString;
//			std::string locationVector = "LocVec" + iAsString;
//			std::string dbmInstance = "DBM" + iAsString;
//
//			xml_node<>* node = doc.allocate_node(node_element, "node");
//			xml_attribute<>* attr = doc.allocate_attribute("id", doc.allocate_string(id.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("location_vector", doc.allocate_string(locationVector.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("dbm_instance", doc.allocate_string(dbmInstance.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("variable_vector", "VarVec1");
//			node->append_attribute(attr);
//			root.append_node(node);
//		}
//	}
//
//	void TraceStore::GenerateDBMS(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const
//	{
//		xml_node<>* node = doc.allocate_node(node_element, "dbm_instance");
//		xml_attribute<>* attr = doc.allocate_attribute("id", "DBM0");
//		node->append_attribute(attr);
//		dbm::dbm_t initialDBM(options.GetKBound()+1);
//		initialDBM.setZero();
//		GenerateDBMDescription(doc, *node, initialDBM);
//		root.append_node(node);
//
//		int index = 1;
//		for(std::deque<TraceInfo>::const_iterator iter = traceInfos.begin(); iter != traceInfos.end(); ++iter)
//		{
//			std::string id = "DBM" + boost::lexical_cast<std::string>(index);
//			node = doc.allocate_node(node_element, "dbm_instance");
//			xml_attribute<>* attr = doc.allocate_attribute("id", doc.allocate_string(id.c_str()));
//			node->append_attribute(attr);
//			GenerateDBMDescription(doc, *node, iter->Marking().Zone());
//			root.append_node(node);
//			index++;
//		}
//
//		std::string id = "DBM" + boost::lexical_cast<std::string>(index);
//		node = doc.allocate_node(node_element, "dbm_instance");
//		attr = doc.allocate_attribute("id", doc.allocate_string(id.c_str()));
//		node->append_attribute(attr);
//		GenerateDBMDescription(doc, *node, finalMarking->Zone());
//		root.append_node(node);
//	}
//
//	void TraceStore::GenerateDBMDescription(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& dbmNode, const dbm::dbm_t& dbm) const
//	{
//		int dim = dbm.getDimension();
//		for(int i = 0; i < options.GetKBound() + 1; ++i)
//		{
//			for(int j = 0; j < options.GetKBound() + 1; ++j)
//			{
//				std::string clock1 = i == 0 ? "sys.t(0)" : "sys.x" + boost::lexical_cast<std::string>(i-1);
//				std::string clock2 = j == 0 ? "sys.t(0)" : "sys.x" + boost::lexical_cast<std::string>(j-1);
//				std::string bound = i >= dim || j >= dim || dbm_raw2bound(dbm[i][j]) == dbm_INFINITY ? "inf" : boost::lexical_cast<std::string>(dbm_raw2bound(dbm[i][j]));
//				std::string comparison = i >= dim || j >= dim ? "<" : (dbm_raw2strict(dbm[i][j]) == dbm_WEAK ? "<=" : "<");
//				if(i == 0 && j >= dim){
//					bound = "0";
//					comparison = "<=";
//				}
//
//				xml_node<>* node = doc.allocate_node(node_element, "clockbound");
//				xml_attribute<>* attr = doc.allocate_attribute("clock1", doc.allocate_string(clock1.c_str()));
//				node->append_attribute(attr);
//				attr = doc.allocate_attribute("clock2", doc.allocate_string(clock2.c_str()));
//				node->append_attribute(attr);
//				attr = doc.allocate_attribute("bound", doc.allocate_string(bound.c_str()));
//				node->append_attribute(attr);
//				attr = doc.allocate_attribute("comp", doc.allocate_string(comparison.c_str()));
//				node->append_attribute(attr);
//				dbmNode.append_node(node);
//			}
//		}
//	}
//
//	void TraceStore::GenerateStates(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& root, const std::deque<TraceInfo>& traceInfos) const
//	{
//		for(unsigned int i = 0; i < traceInfos.size(); ++i)
//		{
//			std::string from = "State" + boost::lexical_cast<std::string>(i);
//			std::string to = "State" + boost::lexical_cast<std::string>(i+1);
//			std::string edges = "Template.Edge" + boost::lexical_cast<std::string>(i);
//			xml_node<>* node = doc.allocate_node(node_element, "transition");
//			xml_attribute<>* attr = doc.allocate_attribute("from", doc.allocate_string(from.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("to", doc.allocate_string(to.c_str()));
//			node->append_attribute(attr);
//			attr = doc.allocate_attribute("edges", doc.allocate_string(edges.c_str()));
//			node->append_attribute(attr);
//
//			root.append_node(node);
//		}
//	}
}
