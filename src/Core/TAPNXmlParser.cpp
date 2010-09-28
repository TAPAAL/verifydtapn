#include "TAPNXmlParser.hpp"
#include <fstream>
#include <string>
#include <algorithm>
#include "boost/bind.hpp"

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
		boost::shared_ptr<TimedPlace::Vector> places = ParsePlaces(root);
		boost::shared_ptr<TimedTransition::Vector> transitions = ParseTransitions(root);

		TAPNXmlParser::ArcCollections arcs = ParseArcs(root, *places, *transitions);

		boost::shared_ptr<TimedArcPetriNet> tapn = boost::make_shared<TimedArcPetriNet>(places, transitions, arcs.inputArcs, arcs.outputArcs);

		return tapn;
	}

	boost::shared_ptr<TimedPlace::Vector> TAPNXmlParser::ParsePlaces(const xml_node<>& root) const
	{
		boost::shared_ptr<TimedPlace::Vector> places = boost::make_shared<TimedPlace::Vector>();

		xml_node<>* placeNode = root.first_node("place");
		while(placeNode != NULL){
			boost::shared_ptr<TimedPlace> place = ParsePlace(*placeNode);
			places->push_back(place);
			placeNode = placeNode->next_sibling("place");
		}

		return places;
	}

	boost::shared_ptr<TimedPlace> TAPNXmlParser::ParsePlace(const xml_node<>& placeNode) const
	{
		std::string name(placeNode.first_attribute("id")->value());

		xml_node<>* invariantNode = placeNode.first_node("invariant")->first_node("value");
		TimeInvariant timeInvariant = TimeInvariant::CreateFor(invariantNode->value());
		return boost::make_shared<TimedPlace>(name, timeInvariant);
	}

	boost::shared_ptr<TimedTransition::Vector> TAPNXmlParser::ParseTransitions(const xml_node<>& root) const
	{
		boost::shared_ptr<TimedTransition::Vector> transitions = boost::make_shared<TimedTransition::Vector>();

		xml_node<>* transitionNode = root.first_node("transition");
		while(transitionNode != NULL){
			boost::shared_ptr<TimedTransition> transition = ParseTransition(*transitionNode);
			transitions->push_back(transition);
			transitionNode = transitionNode->next_sibling("transition");
		}

		return transitions;
	}

	boost::shared_ptr<TimedTransition> TAPNXmlParser::ParseTransition(const xml_node<>& transitionNode) const
	{
		std::string name(transitionNode.first_attribute("id")->value());
		return boost::make_shared<TimedTransition>(name);
	}

	TAPNXmlParser::ArcCollections TAPNXmlParser::ParseArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		boost::shared_ptr<TimedInputArc::Vector> inputArcs = boost::make_shared<TimedInputArc::Vector>();
		boost::shared_ptr<OutputArc::Vector> outputArcs = boost::make_shared<OutputArc::Vector>();

		xml_node<>* arcNode = root.first_node("arc");
		while(arcNode != NULL){
			std::string strSource = arcNode->first_attribute("source")->value();

			TimedPlace::Vector::const_iterator it = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetName), _1) == strSource);
			if(it != places.end()){
				inputArcs->push_back(ParseInputArc(*arcNode, places, transitions));
			}else{
				outputArcs->push_back(ParseOutputArc(*arcNode, places, transitions));
			}

			arcNode = arcNode->next_sibling("arc");
		}

		return ArcCollections(inputArcs, outputArcs);
	}

	boost::shared_ptr<TimedInputArc> TAPNXmlParser::ParseInputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();
		std::string interval = arcNode.first_node("inscription")->first_node("value")->value();

		TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetName), _1) == source);
		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetName), _1) == target);

		return boost::make_shared<TimedInputArc>(**place, **transition, TimeInterval::CreateFor(interval));
	}

	boost::shared_ptr<OutputArc> TAPNXmlParser::ParseOutputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{

	}

}
