#include "TAPNXmlParser.hpp"
#include <string>
#include <algorithm>
#include "boost/bind.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "util.hpp"

namespace VerifyTAPN {
using namespace rapidxml;

boost::shared_ptr<TimedArcPetriNet> TAPNXmlParser::parse(const std::string & filename) const
{
	const std::string contents = VerifyTAPN::readFile(filename);
	std::vector<char> charArray(contents.begin(), contents.end());
	charArray.push_back('\0');

	xml_document<> xmldoc;
	xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

	xml_node<>* pnml = xmldoc.first_node("pnml");
	if(pnml == 0) throw std::string("invalid file.");

	xml_node<>* root = pnml->first_node("net");
	if(root == 0) throw std::string("invalid file.");

	return parseTAPN(*root);
}

std::vector<int> TAPNXmlParser::parseMarking(const std::string & filename, const TimedArcPetriNet& tapn) const
{
	const std::string contents = VerifyTAPN::readFile(filename); // not sure if this is a good idea, because it copies to string? Maybe the compiler is smart enough not to make a copy
	std::vector<char> charArray(contents.begin(), contents.end());
	charArray.push_back('\0');

	xml_document<> xmldoc;
	xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

	xml_node<>* pnml = xmldoc.first_node("pnml"); // we don't need error handling here, since we are parsing the same file as above
	xml_node<>* root = pnml->first_node("net");
	return parseInitialMarking(*root, tapn);
}

boost::shared_ptr<TimedArcPetriNet> TAPNXmlParser::parseTAPN(const xml_node<>& root) const
{
	TimedPlace::Vector places = parsePlaces(root);
	TimedTransition::Vector transitions = parseTransitions(root);

	TAPNXmlParser::ArcCollections arcs = parseArcs(root, places, transitions);

	boost::shared_ptr<TimedArcPetriNet> tapn = boost::make_shared<TimedArcPetriNet>(places, transitions, arcs.inputArcs, arcs.outputArcs, arcs.transportArcs, arcs.inhibitorArcs);

	return tapn;
}

TimedPlace::Vector TAPNXmlParser::parsePlaces(const xml_node<>& root) const
{
	TimedPlace::Vector places;

	xml_node<>* placeNode = root.first_node("place");
	while(placeNode != NULL){
		boost::shared_ptr<TimedPlace> place = parsePlace(*placeNode);
		places.push_back(place);
		placeNode = placeNode->next_sibling("place");
	}

	return places;
}

boost::shared_ptr<TimedPlace> TAPNXmlParser::parsePlace(const xml_node<>& placeNode) const
{
	std::string id(placeNode.first_attribute("id")->value());
	std::string name(placeNode.first_attribute("name")->value());

	std::string invariantNode = placeNode.first_attribute("invariant")->value();
	TimeInvariant timeInvariant = TimeInvariant::createFor(invariantNode);
	return boost::make_shared<TimedPlace>(name, id, timeInvariant);
}

TimedTransition::Vector TAPNXmlParser::parseTransitions(const xml_node<>& root) const
{
	TimedTransition::Vector transitions;

	xml_node<>* transitionNode = root.first_node("transition");
	while(transitionNode != NULL){
		boost::shared_ptr<TimedTransition> transition = parseTransition(*transitionNode);
		transitions.push_back(transition);
		transitionNode = transitionNode->next_sibling("transition");
	}

	return transitions;
}

boost::shared_ptr<TimedTransition> TAPNXmlParser::parseTransition(const xml_node<>& transitionNode) const
{
	std::string id(transitionNode.first_attribute("id")->value());
	std::string name(transitionNode.first_attribute("name")->value());
        xml_attribute<char>* urgenatt = transitionNode.first_attribute("urgent");
        bool urgent = false;
        if(urgenatt != NULL){
            std::string urgentStr = urgenatt->value();
            if(urgentStr.compare("true") == 0){
                urgent = true;
            }
        }
	return boost::make_shared<TimedTransition>(name, id,urgent );
}

TAPNXmlParser::ArcCollections TAPNXmlParser::parseArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{

	TimedInputArc::Vector inputArcs = parseInputArcs(root, places, transitions);
	TransportArc::Vector transportArcs = parseTransportArcs(root, places, transitions);
	InhibitorArc::Vector inhibitorArcs = parseInhibitorArcs(root,places,transitions);
	OutputArc::Vector outputArcs = parseOutputArcs(root, places, transitions);

	return ArcCollections(inputArcs, outputArcs, transportArcs, inhibitorArcs);
}

TransportArc::Vector TAPNXmlParser::parseTransportArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	TransportArc::Vector transportArcs;
	xml_node<>* arcNode = root.first_node("transportArc");
	while(arcNode != NULL){
		transportArcs.push_back(parseTransportArc(*arcNode, places, transitions));
		arcNode = arcNode->next_sibling("transportArc");
	}
	return transportArcs;
}

InhibitorArc::Vector TAPNXmlParser::parseInhibitorArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	InhibitorArc::Vector inhibitorArcs;
	xml_node<>* arcNode = root.first_node("inhibitorArc");
	while(arcNode != NULL){
		inhibitorArcs.push_back(parseInhibitorArc(*arcNode, places, transitions));
		arcNode = arcNode->next_sibling("inhibitorArc");
	}
	return inhibitorArcs;
}

TimedInputArc::Vector TAPNXmlParser::parseInputArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	TimedInputArc::Vector inputArcs;
	xml_node<>* arcNode = root.first_node("inputArc");
	while(arcNode != NULL){
		inputArcs.push_back(parseInputArc(*arcNode, places, transitions));
		arcNode = arcNode->next_sibling("inputArc");
	}

	return inputArcs;
}

OutputArc::Vector TAPNXmlParser::parseOutputArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	OutputArc::Vector outputArcs;
	xml_node<>* arcNode = root.first_node("outputArc");
	while(arcNode != NULL){
		outputArcs.push_back(parseOutputArc(*arcNode, places, transitions));
		arcNode = arcNode->next_sibling("outputArc");
	}

	return outputArcs;
}

boost::shared_ptr<TimedInputArc> TAPNXmlParser::parseInputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	std::string source = arcNode.first_attribute("source")->value();
	std::string target = arcNode.first_attribute("target")->value();
	std::string interval = arcNode.first_attribute("inscription")->value();
	int weight = 1;
	xml_attribute<char>* attribute = arcNode.first_attribute("weight");
	if(attribute != NULL){
		weight = atoi(attribute->value());
	}

	TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::getId), _1) == source);
	TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::getId), _1) == target);

	return boost::make_shared<TimedInputArc>(*place, *transition, weight, TimeInterval::createFor(interval));
}

boost::shared_ptr<TransportArc> TAPNXmlParser::parseTransportArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	std::string sourceName = arcNode.first_attribute("source")->value();
	std::string transitionName = arcNode.first_attribute("transition")->value();
	std::string targetName = arcNode.first_attribute("target")->value();
	std::string interval = arcNode.first_attribute("inscription")->value();
	int weight = 1;
	xml_attribute<char>* attribute = arcNode.first_attribute("weight");
	if(attribute != NULL){
		weight = atoi(attribute->value());
	}

	TimedPlace::Vector::const_iterator source = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::getId), _1) == sourceName);
	TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::getId), _1) == transitionName);
	TimedPlace::Vector::const_iterator target = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::getId), _1) == targetName);
	return boost::make_shared<TransportArc>(*source, *transition, *target, TimeInterval::createFor(interval), weight);
}

boost::shared_ptr<InhibitorArc> TAPNXmlParser::parseInhibitorArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	std::string source = arcNode.first_attribute("source")->value();
	std::string target = arcNode.first_attribute("target")->value();
	int weight = 1;
	xml_attribute<char>* attribute = arcNode.first_attribute("weight");
	if(attribute != NULL){
		weight = atoi(attribute->value());
	}

	TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::getId), _1) == source);
	TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::getId), _1) == target);

	return boost::make_shared<InhibitorArc>(*place, *transition, weight);
}

boost::shared_ptr<OutputArc> TAPNXmlParser::parseOutputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
{
	std::string source = arcNode.first_attribute("source")->value();
	std::string target = arcNode.first_attribute("target")->value();
	int weight = 1;
	xml_attribute<char>* attribute = arcNode.first_attribute("weight");
	if(attribute != NULL){
		weight = atoi(attribute->value());
	}

	TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::getId), _1) == source);
	TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::getId), _1) == target);

	return boost::make_shared<OutputArc>(*transition, *place, weight);

}

std::vector<int> TAPNXmlParser::parseInitialMarking(const rapidxml::xml_node<>& root, const TimedArcPetriNet& tapn) const
{
	std::vector<int> markedPlaces;
	xml_node<>* placeNode = root.first_node("place");
	int totalInitTokens = 0;
	while(placeNode != NULL)
	{
		std::string initialMarkingValue = placeNode->first_attribute("initialMarking")->value();
		std::string placeName(placeNode->first_attribute("name")->value());

		boost::algorithm::trim(initialMarkingValue);

		int nTokens = boost::lexical_cast<int>(initialMarkingValue);
		totalInitTokens += nTokens;

		if(nTokens > 0)
		{
			for(int i = 0; i < nTokens; i++) {
				markedPlaces.push_back(tapn.getPlaceIndex(placeName));
			}
		}

		placeNode = placeNode->next_sibling("place");
	}

	return markedPlaces;
}


}
