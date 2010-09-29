#include "TAPNXmlParser.hpp"
#include <fstream>
#include <string>
#include <algorithm>

namespace VerifyTAPN {
	using namespace rapidxml;

	const std::string TAPNXmlParser::ReadFile(const std::string& filename) const
	{
		std::ifstream file (filename.c_str());
		if(!file){
			throw new std::exception();
		}
		std::string contents;
		std::string line;
		while(std::getline(file, line)){
			contents += line;
		}
		return contents;
	}

	boost::shared_ptr<TimedArcPetriNet> TAPNXmlParser::Parse(const std::string & filename) const
	{
		const std::string contents = ReadFile(filename); // not sure if this is a good idea, because it copies to string? Maybe the compiler is smart enough not to make a copy
		std::vector<char> charArray(contents.begin(), contents.end());
		charArray.push_back('\0');

		xml_document<> xmldoc;
		xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

		return ParseTAPN(*xmldoc.first_node()->first_node());
	}

	boost::shared_ptr<TimedArcPetriNet> TAPNXmlParser::ParseTAPN(const xml_node<>& root) const
	{
		boost::shared_ptr<TimedArcPetriNet> tapn = boost::make_shared<TimedArcPetriNet>();

		ParsePlaces(root, *tapn);
		ParseTransitions(root, *tapn);
		ParseArcs(root, *tapn);

		return tapn;
	}

	void TAPNXmlParser::ParsePlaces(const xml_node<>& root, TimedArcPetriNet& tapn) const
	{
		xml_node<>* placeNode = root.first_node("place");
		while(placeNode != NULL){
			TimedPlace* place = ParsePlace(*placeNode);
			tapn.AddPlace(place);
			placeNode = placeNode->next_sibling("place");
		}
	}

	TimedPlace* TAPNXmlParser::ParsePlace(const xml_node<>& placeNode) const
	{
		std::string name(placeNode.first_attribute("id")->value());

		xml_node<>* invariantNode = placeNode.first_node("invariant")->first_node("value");
		TimeInvariant timeInvariant = TimeInvariant::CreateFor(invariantNode->value());
		return new TimedPlace(name, timeInvariant);
	}

	void TAPNXmlParser::ParseTransitions(const xml_node<>& root, TimedArcPetriNet& tapn) const
	{
		xml_node<>* transitionNode = root.first_node("transition");
		while(transitionNode != NULL){
			TimedTransition* transition = ParseTransition(*transitionNode);
			tapn.AddTransition(transition);
			transitionNode = transitionNode->next_sibling("transition");
		}
	}

	TimedTransition* TAPNXmlParser::ParseTransition(const xml_node<>& transitionNode) const
	{
		std::string name(transitionNode.first_attribute("id")->value());
		return new TimedTransition(name);
	}

	void TAPNXmlParser::ParseArcs(const xml_node<>& root, TimedArcPetriNet& tapn) const
	{
		boost::shared_ptr<TimedInputArc::Vector> inputArcs = boost::make_shared<TimedInputArc::Vector>();
		boost::shared_ptr<OutputArc::Vector> outputArcs = boost::make_shared<OutputArc::Vector>();

		xml_node<>* arcNode = root.first_node("arc");
		while(arcNode != NULL){
			std::string strSource = arcNode->first_attribute("source")->value();

			if(tapn.ContainsPlaceByName(strSource)){
				TimedInputArc* arc = ParseInputArc(*arcNode, tapn);
				tapn.AddInputArc(arc);
			}else{
				OutputArc* arc = ParseOutputArc(*arcNode, tapn);
				tapn.AddOutputArc(arc);
			}

			arcNode = arcNode->next_sibling("arc");
		}
	}

	TimedInputArc* TAPNXmlParser::ParseInputArc(const rapidxml::xml_node<>& arcNode, TimedArcPetriNet& tapn) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();
		std::string interval = arcNode.first_node("inscription")->first_node("value")->value();

		const TimedPlace& place = tapn.FindPlaceByName(source);
		const TimedTransition& transition = tapn.FindTransitionByName(target);

		return new TimedInputArc(place, transition, TimeInterval::CreateFor(interval));
	}

	OutputArc* TAPNXmlParser::ParseOutputArc(const rapidxml::xml_node<>& arcNode, TimedArcPetriNet& tapn) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();

		const TimedTransition& transition = tapn.FindTransitionByName(source);
		const TimedPlace& place = tapn.FindPlaceByName(target);

		return new OutputArc(transition, place);

	}



}
