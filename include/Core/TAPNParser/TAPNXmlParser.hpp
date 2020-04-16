#ifndef VERIFYTAPN_TAPNXMLPARSER_HPP_
#define VERIFYTAPN_TAPNXMLPARSER_HPP_

#include "Core/TAPN/TAPN.hpp"
#include <rapidxml.hpp>
#include <utility>

namespace VerifyTAPN {
    using namespace VerifyTAPN::TAPN;


    class TAPNXmlParser {
    private:
        struct ArcCollections {
            TimedInputArc::Vector inputArcs;
            OutputArc::Vector outputArcs;
            TransportArc::Vector transportArcs;
            InhibitorArc::Vector inhibitorArcs;

        public:
            ArcCollections(TimedInputArc::Vector inputArcs, OutputArc::Vector outputArcs,
                           TransportArc::Vector transportArcs, InhibitorArc::Vector inhibitorArcs)
                    : inputArcs(std::move(inputArcs)), outputArcs(std::move(outputArcs)), transportArcs(std::move(transportArcs)),
                      inhibitorArcs(std::move(inhibitorArcs)) {};
        };

    public: // construction
        explicit TAPNXmlParser(const std::map<std::string, int> &replace) : replace(replace) {};

        virtual ~TAPNXmlParser() { /* empty */ };

    public:
        TimedArcPetriNet *parse(const std::string &filename) const;

        std::vector<int> parseMarking(const std::string &filename, const TimedArcPetriNet &tapn) const;

    private:
        TimedArcPetriNet *parseTAPN(const rapidxml::xml_node<> &root) const;

        TimedPlace::Vector parsePlaces(const rapidxml::xml_node<> &root) const;

        TimedPlace *parsePlace(const rapidxml::xml_node<> &placeNode) const;

        TimedTransition::Vector parseTransitions(const rapidxml::xml_node<> &root) const;

        TimedTransition *parseTransition(const rapidxml::xml_node<> &transitionNode) const;

        ArcCollections parseArcs(const rapidxml::xml_node<> &root, const TimedPlace::Vector &places,
                                 const TimedTransition::Vector &transitions) const;

        TransportArc::Vector parseTransportArcs(const rapidxml::xml_node<> &root, const TimedPlace::Vector &places,
                                                const TimedTransition::Vector &transitions) const;

        InhibitorArc::Vector parseInhibitorArcs(const rapidxml::xml_node<> &root, const TimedPlace::Vector &places,
                                                const TimedTransition::Vector &transitions) const;

        TimedInputArc::Vector parseInputArcs(const rapidxml::xml_node<> &root, const TimedPlace::Vector &places,
                                             const TimedTransition::Vector &transitions) const;

        OutputArc::Vector parseOutputArcs(const rapidxml::xml_node<> &root, const TimedPlace::Vector &places,
                                          const TimedTransition::Vector &transitions) const;

        TimedInputArc *parseInputArc(const rapidxml::xml_node<> &arcNode, const TimedPlace::Vector &places,
                                     const TimedTransition::Vector &transitions) const;

        InhibitorArc *parseInhibitorArc(const rapidxml::xml_node<> &arcNode, const TimedPlace::Vector &places,
                                        const TimedTransition::Vector &transitions) const;

        TransportArc *parseTransportArc(const rapidxml::xml_node<> &arcNode, const TimedPlace::Vector &places,
                                        const TimedTransition::Vector &transitions) const;

        OutputArc *parseOutputArc(const rapidxml::xml_node<> &arcNode, const TimedPlace::Vector &places,
                                  const TimedTransition::Vector &transitions) const;

        std::vector<int> parseInitialMarking(const rapidxml::xml_node<> &root, const TimedArcPetriNet &tapn) const;

        int getWeight(rapidxml::xml_attribute<char> *attribute) const;

        const std::map<std::string, int> &replace;
    };
}

#endif /* VERIFYTAPN_TAPNXMLPARSER_HPP_ */
