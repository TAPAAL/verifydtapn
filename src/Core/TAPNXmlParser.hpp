#ifndef VERIFYTAPN_TAPNXMLPARSER_HPP_
#define VERIFYTAPN_TAPNXMLPARSER_HPP_

#include "../TAPN/TAPN.hpp"
#include "boost/smart_ptr.hpp"
#include "../../lib/rapidxml-1.13/rapidxml.hpp"

namespace VerifyTAPN {
	using namespace VerifyTAPN::TAPN;

	class TAPNXmlParser
	{
	private:
		struct ArcCollections{
			boost::shared_ptr<TimedInputArc::Vector> inputArcs;
			boost::shared_ptr<OutputArc::Vector> outputArcs;

		public:
			ArcCollections(const boost::shared_ptr<TimedInputArc::Vector>& inputArcs, const boost::shared_ptr<OutputArc::Vector>& outputArcs) : inputArcs(inputArcs), outputArcs(outputArcs) {};
		};
	public: // construction
		TAPNXmlParser(){};
		virtual ~TAPNXmlParser() { /* empty */ };

	public:
		boost::shared_ptr<TimedArcPetriNet> Parse(const std::string & filename) const;
	private:
		boost::shared_ptr<TimedArcPetriNet> ParseTAPN(const rapidxml::xml_node<> & root) const;
		const std::string ReadFile(const std::string & filename) const;

		boost::shared_ptr<TimedPlace::Vector> ParsePlaces(const rapidxml::xml_node<>& root) const;
		boost::shared_ptr<TimedPlace> ParsePlace(const rapidxml::xml_node<>& placeNode) const;

		boost::shared_ptr<TimedTransition::Vector> ParseTransitions(const rapidxml::xml_node<>& root) const;
		boost::shared_ptr<TimedTransition> ParseTransition(const rapidxml::xml_node<>& transitionNode) const;

		ArcCollections ParseArcs(const rapidxml::xml_node<>& root) const;

	private: // data
	};
}

#endif /* VERIFYTAPN_TAPNXMLPARSER_HPP_ */
