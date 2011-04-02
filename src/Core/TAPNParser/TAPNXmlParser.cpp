#include "TAPNXmlParser.hpp"
#include <string>
#include <algorithm>
#include "boost/bind.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "util.hpp"
#include "../SymbolicMarking/MarkingFactory.hpp"
#include "../SymbolicMarking/DiscretePart.hpp"

namespace VerifyTAPN {
	using namespace rapidxml;

	boost::shared_ptr<TimedArcPetriNet> TAPNXmlParser::Parse(const std::string & filename) const
	{
		const std::string contents = VerifyTAPN::ReadFile(filename);
		std::vector<char> charArray(contents.begin(), contents.end());
		charArray.push_back('\0');

		xml_document<> xmldoc;
		xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

		return ParseTAPN(*xmldoc.first_node()->first_node());
	}

	std::vector<int> TAPNXmlParser::ParseMarking(const std::string & filename, const TimedArcPetriNet& tapn) const
	{
		const std::string contents = VerifyTAPN::ReadFile(filename); // not sure if this is a good idea, because it copies to string? Maybe the compiler is smart enough not to make a copy
		std::vector<char> charArray(contents.begin(), contents.end());
		charArray.push_back('\0');

		xml_document<> xmldoc;
		xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

		return ParseInitialMarking(*xmldoc.first_node()->first_node(),tapn);
	}

	boost::shared_ptr<TimedArcPetriNet> TAPNXmlParser::ParseTAPN(const xml_node<>& root) const
	{
		TimedPlace::Vector places = ParsePlaces(root);
		TimedTransition::Vector transitions = ParseTransitions(root);

		TAPNXmlParser::ArcCollections arcs = ParseArcs(root, places, transitions);

		boost::shared_ptr<TimedArcPetriNet> tapn = boost::make_shared<TimedArcPetriNet>(places, transitions, arcs.inputArcs, arcs.outputArcs);

		return tapn;
	}

	TimedPlace::Vector TAPNXmlParser::ParsePlaces(const xml_node<>& root) const
	{
		TimedPlace::Vector places;

		xml_node<>* placeNode = root.first_node("place");
		while(placeNode != NULL){
			boost::shared_ptr<TimedPlace> place = ParsePlace(*placeNode);
			places.push_back(place);
			placeNode = placeNode->next_sibling("place");
		}

		return places;
	}

	boost::shared_ptr<TimedPlace> TAPNXmlParser::ParsePlace(const xml_node<>& placeNode) const
	{
		std::string id(placeNode.first_attribute("id")->value());
		std::string name(placeNode.first_attribute("name")->value());

		std::string invariantNode = placeNode.first_attribute("invariant")->value();
		TimeInvariant timeInvariant = TimeInvariant::CreateFor(invariantNode);
		return boost::make_shared<TimedPlace>(name, id, timeInvariant);
	}

	TimedTransition::Vector TAPNXmlParser::ParseTransitions(const xml_node<>& root) const
	{
		TimedTransition::Vector transitions;

		xml_node<>* transitionNode = root.first_node("transition");
		while(transitionNode != NULL){
			boost::shared_ptr<TimedTransition> transition = ParseTransition(*transitionNode);
			transitions.push_back(transition);
			transitionNode = transitionNode->next_sibling("transition");
		}

		return transitions;
	}

	boost::shared_ptr<TimedTransition> TAPNXmlParser::ParseTransition(const xml_node<>& transitionNode) const
	{
		std::string id(transitionNode.first_attribute("id")->value());
		std::string name(transitionNode.first_attribute("name")->value());
		return boost::make_shared<TimedTransition>(name, id);
	}

	TAPNXmlParser::ArcCollections TAPNXmlParser::ParseArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{

		TimedInputArc::Vector inputArcs = ParseInputArcs(root, places, transitions);
		OutputArc::Vector outputArcs = ParseOutputArcs(root, places, transitions);

		return ArcCollections(inputArcs, outputArcs);
	}

	TimedInputArc::Vector TAPNXmlParser::ParseInputArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		TimedInputArc::Vector inputArcs;
		xml_node<>* arcNode = root.first_node("inputArc");
		while(arcNode != NULL){
			inputArcs.push_back(ParseInputArc(*arcNode, places, transitions));
			arcNode = arcNode->next_sibling("inputArc");
		}

		return inputArcs;
	}

	OutputArc::Vector TAPNXmlParser::ParseOutputArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		OutputArc::Vector outputArcs;
		xml_node<>* arcNode = root.first_node("outputArc");
		while(arcNode != NULL){
			outputArcs.push_back(ParseOutputArc(*arcNode, places, transitions));
			arcNode = arcNode->next_sibling("outputArc");
		}

		return outputArcs;
	}

	boost::shared_ptr<TimedInputArc> TAPNXmlParser::ParseInputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();
		std::string interval = arcNode.first_attribute("inscription")->value();

		TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == source);
		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetId), _1) == target);

		return boost::make_shared<TimedInputArc>(*place, *transition, TimeInterval::CreateFor(interval));
	}

	boost::shared_ptr<OutputArc> TAPNXmlParser::ParseOutputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();

		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetId), _1) == source);
		TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == target);

		return boost::make_shared<OutputArc>(*transition, *place);

	}

	std::vector<int> TAPNXmlParser::ParseInitialMarking(const rapidxml::xml_node<>& root, const TimedArcPetriNet& tapn) const
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
					markedPlaces.push_back(tapn.GetPlaceIndex(placeName));
				}
			}

			placeNode = placeNode->next_sibling("place");
		}

		return markedPlaces;
	}


}
